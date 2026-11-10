/* Host-only integration entrypoint. Links real drum DSP; board controls below
 * are fixed inputs. This first P1 case renders one original main-drum voice. */
#ifndef JB_HOST_PROBE
#error "host renderer must not enter firmware"
#endif
#include "Soundengine/DSPAudio/DrumVoice.h"
#include "Soundengine/DSPAudio/Snare.h"
#include "Soundengine/DSPAudio/CymbalVoice.h"
#include "Soundengine/DSPAudio/HiHat.h"
#include "Soundengine/DSPAudio/random.h"
#include "Soundengine/MIDI/ParameterArray.h"
#include "Soundengine/MIDI/MidiParser.h"
#include "menu/menu.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/* Hardware and UI boundaries. Unused modulation destinations remain null;
 * reaching a MIDI parameter write is a harness error. */
RNG_HandleTypeDef hrng;
SnareVoice snareVoice;
CymbalVoice cymbalVoice;
HiHatVoice hatVoice;
Parameter parameterArray[END_OF_SOUND_PARAMETERS];
uint8_t activeParameters[NUM_PARAMS];
float adc_getFaderVolume(uint8_t voice) { (void)voice; return 1.f; }
uint16_t seq_getBpm(void) { return 120; }
void paramArray_setParameter(uint16_t index, ptrValue value)
{
    (void)value;
    if (index < END_OF_SOUND_PARAMETERS && !parameterArray[index].ptr) return;
    abort();
}
void midiParser_ccHandler(MidiMsg msg, uint8_t original, uint8_t mod, float value)
{ (void)msg; (void)original; (void)mod; (void)value; abort(); }
/* The MIDI note detune control law is copied from MidiParser.c; it is not
 * an oscillator, envelope, filter, or voice DSP substitute. */
float midiParser_calcDetune(uint8_t value)
{
    const float frac = value/127.f - 0.5f;
    float cent = 1.f;
    cent += frac*(1.0594630943592952645618252949463f - 1.f);
    return cent;
}
uint32_t HAL_RNG_GetRandomNumber(RNG_HandleTypeDef *handle)
{
    static uint32_t state = 0x918fe273u;
    (void)handle;
    state ^= state << 13; state ^= state >> 17; state ^= state << 5;
    return state;
}

int main(int argc, char **argv)
{
    enum { FRAMES = 4096, BLOCK = 16 };
    if (argc < 2 || argc > 3) { fprintf(stderr, "usage: %s output.pcm [kick|tom1|tom2|fm|retrigger|long|overlap]\n", argv[0]); return 2; }
    const char *scenario = argc == 3 ? argv[2] : "kick";
    const int kick = !strcmp(scenario, "kick");
    const int tom1 = !strcmp(scenario, "tom1");
    const int tom2 = !strcmp(scenario, "tom2");
    const int fm = !strcmp(scenario, "fm");
    const int retrigger = !strcmp(scenario, "retrigger");
    const int long_decay = !strcmp(scenario, "long");
    const int overlap = !strcmp(scenario, "overlap");
    if (!(kick || tom1 || tom2 || fm || retrigger || long_decay || overlap)) {
        fprintf(stderr, "unknown scenario: %s\n", scenario); return 2;
    }
    FILE *out = fopen(argv[1], "wb");
    if (!out) { perror(argv[1]); return 2; }
    initDrumVoice();
    for (unsigned voice = 0; voice < NUM_DRUM_VOICES; ++voice) {
        voiceArray[voice].osc.waveform = voice == 0 ? SINE : (voice == 1 ? TRI : SAW);
        voiceArray[voice].modOsc.waveform = SINE;
        voiceArray[voice].filterType = FILTER_OFF;
        voiceArray[voice].oscVolEg.attack = 0.05;
        voiceArray[voice].oscVolEg.decay = long_decay || overlap ? 0.001 : 0.004;
        voiceArray[voice].fmModAmount = 0.25f;
    }
    if (fm) voiceArray[0].mixOscs = 0;
    unsigned selected = tom1 ? 1u : tom2 ? 2u : 0u;
    Drum_trigger(selected, 100, selected ? 52 : 48);
    int peak = 0, clipped = 0, nonzero = 0;
    for (unsigned block = 0; block < FRAMES / BLOCK; ++block) {
        int16_t pcm[BLOCK] = {0};
        if (retrigger && (block == 32 || block == 64 || block == 96))
            Drum_trigger(0, 115, 48);
        if (overlap && block == 16) Drum_trigger(1, 105, 52);
        if (overlap && block == 32) Drum_trigger(2, 120, 55);
        if (overlap && block == 48) Drum_trigger(0, 110, 48);
        for (unsigned voice = 0; voice < (overlap ? 3u : 1u); ++voice) {
            int16_t part[BLOCK] = {0};
            unsigned index = overlap ? voice : selected;
            calcDrumVoiceAsync(index);
            calcDrumVoiceSyncBlock(index, part, BLOCK);
            for (unsigned i = 0; i < BLOCK; ++i) {
                int sum = pcm[i] + part[i];
                pcm[i] = sum > 32767 ? 32767 : sum < -32768 ? -32768 : sum;
            }
        }
        for (unsigned i = 0; i < BLOCK; ++i) {
            int sample = pcm[i];
            int absolute = sample < 0 ? -sample : sample;
            if (absolute > peak) peak = absolute;
            if (absolute >= 32767) clipped++;
            if (sample) nonzero++;
        }
        if (fwrite(pcm, sizeof(pcm), 1, out) != 1) { perror("write"); fclose(out); return 2; }
    }
    if (fclose(out)) { perror("close"); return 2; }
    printf("scenario=%s samples=%u peak=%d clipped=%d nonzero=%d\n",
           scenario, FRAMES, peak, clipped, nonzero);
    return nonzero && peak && !clipped ? 0 : 1;
}
