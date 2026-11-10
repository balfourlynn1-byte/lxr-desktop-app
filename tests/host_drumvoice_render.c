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
    if (argc != 2) { fprintf(stderr, "usage: %s output.pcm\n", argv[0]); return 2; }
    FILE *out = fopen(argv[1], "wb");
    if (!out) { perror(argv[1]); return 2; }
    initDrumVoice();
    voiceArray[0].osc.waveform = SINE;
    voiceArray[0].modOsc.waveform = SINE;
    voiceArray[0].filterType = FILTER_OFF;
    voiceArray[0].oscVolEg.attack = 0.05;
    voiceArray[0].oscVolEg.decay = 0.004;
    voiceArray[0].fmModAmount = 0.25f;
    Drum_trigger(0, 100, 48);
    int peak = 0, clipped = 0, nonzero = 0;
    for (unsigned block = 0; block < FRAMES / BLOCK; ++block) {
        int16_t pcm[BLOCK] = {0};
        calcDrumVoiceAsync(0);
        calcDrumVoiceSyncBlock(0, pcm, BLOCK);
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
    printf("samples=%u peak=%d clipped=%d nonzero=%d\n", FRAMES, peak, clipped, nonzero);
    return nonzero && peak && !clipped ? 0 : 1;
}
