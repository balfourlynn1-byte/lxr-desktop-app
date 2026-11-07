/* Offline WaveShape regression. Genuine production synthesis source, no device. */
#include "Soundengine/DSPAudio/Jimbones.h"
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define SAMPLES 11025
static int16_t bufA[SAMPLES],bufB[SAMPLES],bufC[SAMPLES],dry[SAMPLES];
static void render(uint8_t mode, uint8_t character, uint8_t aggression,
                   uint8_t position, int16_t *dst) {
    jb_init();
    jb_parameters[1][JB_MODE]=mode;
    jb_parameters[1][JB_CHARACTER]=character;
    jb_parameters[1][JB_AGGRESSION]=aggression;
    jb_parameters[1][JB_POSITION]=position;
    jb_parameters[1][JB_MOVEMENT]=55;
    jb_parameters[1][JB_TEXTURE]=0;
    jb_parameters[1][JB_DRIVE]=0;
    jb_parameters[1][JB_BITS]=0;
    jb_parameters[1][JB_DECAY]=105;
    jb_parameters[1][JB_WAVE_LEVEL]=112;
    jb_parameters[1][JB_METAL_LEVEL]=112;
    jb_trigger(1,115,48,0);
    for(unsigned i=0;i<SAMPLES;i++){
        dst[i]=0;
        jb_render(1,&dst[i],1,0.70f);
    }
}
static double rms(const int16_t *a) {
    double s=0;
    for(unsigned i=0;i<SAMPLES;i++)s+=(double)a[i]*a[i];
    return sqrt(s/SAMPLES);
}
static double changeRms(const int16_t *a,const int16_t *b) {
    double s=0;
    for(unsigned i=0;i<SAMPLES;i++) {
        const double d=(double)a[i]-b[i];s+=d*d;
    }
    return sqrt(s/SAMPLES);
}
int main(void) {
    /* Phase shape changes Wavetable at zero distortion drive. */
    render(2,0,0,35,bufA);
    render(2,127,0,35,bufB);
    const double phaseDelta=changeRms(bufA,bufB);
    const double ref=rms(bufA);
    assert(ref>200.0);
    assert(phaseDelta/ref>.15);
    /* Wavefolding changes harmonics without requiring the master fader boost. */
    render(2,0,127,35,bufC);
    assert(changeRms(bufA,bufC)/ref>.20);
    /* Every waveform position must stay audible. */
    render(2,127,127,127,dry);
    assert(rms(dry)>150.0);
    /* Mode 0 remains untouched even when experimental controls are maxed. */
    jb_init();
    jb_parameters[1][JB_MODE]=0;
    jb_parameters[1][JB_AGGRESSION]=127;
    jb_parameters[1][JB_CHARACTER]=127;
    int16_t samples[64],reference[64];
    for(unsigned i=0;i<64;i++)samples[i]=reference[i]=(int16_t)((int)i*999-32768);
    jb_trigger(1,127,48,0);
    jb_render(1,samples,64,1);
    assert(!memcmp(samples,reference,sizeof(samples)));
    for(unsigned mode=1;mode<=3;mode++) {
        jb_init();
        jb_parameters[1][JB_MODE]=mode;
        jb_parameters[1][JB_METAL_LEVEL]=0;
        jb_parameters[1][JB_WAVE_LEVEL]=0;
        jb_trigger(1,110,48,0);
        jb_render(1,samples,64,1);
        assert(!memcmp(samples,reference,sizeof(samples)));
    }
    printf("PASS WaveShape: phase RMS delta %.3f, fold RMS delta %.3f, Mode 0 identical\n",
           phaseDelta/ref,changeRms(bufA,bufC)/ref);
    return 0;
}
