#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include "Soundengine/DSPAudio/JimbonesNativeWaveScan.h"

/* C09 host PCM fixture: actual C scan kernel with four distinct periodic waves.
 * This does NOT exercise the complete DrumVoice/Oscillator renderer.
 */
static int16_t waves[4][256];
static void make_waves(void) {
    for (unsigned i=0; i<256; i++) {
        float p=(float)i/256.f;
        waves[0][i]=(int16_t)(10000.f*sinf(6.28318530718f*p));
        waves[1][i]=(int16_t)(10000.f*(2.f*p-1.f));
        waves[2][i]=i<128 ? 10000 : -10000;
        waves[3][i]=(int16_t)(10000.f*(1.f-4.f*fabsf(p-0.5f)));
    }
}
static uint64_t render(float base,float depth,unsigned velocity,float lfoDepth,float lfo) {
    float pos=jb_wave_velocity_position(base,depth,(uint8_t)velocity)+lfoDepth*lfo;
    uint32_t phase=0;
    uint64_t signature=1469598103934665603ULL;
    for(unsigned i=0;i<1024;i++) {
        int16_t sample=jb_wave_scan_sample(waves,phase,pos);
        signature^=(uint16_t)sample;
        signature*=1099511628211ULL;
        phase+=0x01800000u;
    }
    return signature;
}
int main(void) {
    make_waves();
    uint64_t a=render(0.f,0.f,64,0.f,0.f);
    uint64_t b=render(2.f,0.f,64,0.f,0.f);
    assert(a!=b); /* changing scan position changes PCM */
    uint64_t low=render(1.5f,1.5f,20,0.f,0.f);
    uint64_t high=render(1.5f,1.5f,110,0.f,0.f);
    assert(low!=high); /* note-on velocity affects PCM */
    uint64_t lfoA=render(1.5f,0.f,64,1.f,-0.5f);
    uint64_t lfoB=render(1.5f,0.f,64,1.f,0.5f);
    assert(lfoA!=lfoB); /* changing LFO position affects PCM */
    assert(render(1.5f,0.f,20,0.f,0.f)==render(1.5f,0.f,110,0.f,0.f));
    printf("PASS C09 C kernel PCM: position, velocity and LFO change samples; depth=0 bypasses velocity\n");
    return 0;
}
