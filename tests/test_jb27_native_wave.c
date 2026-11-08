/* JB-27 independently authored waveform planner host parity and bounds. */
#include "Soundengine/DSPAudio/JimbonesNativeWave.h"
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
int main(void) {
 assert(jb_native_wave_pack(0.25f,0u)==0u);
 assert(jb_native_wave_pack(-1.f,1u)==0u);
 assert(jb_native_wave_pack(NAN,1u)==0u);
 assert(jb_native_wave_pack(0.f,1u)==0u);
 assert(jb_native_wave_pack(1.f/6.f,1u)==0u);
 assert(jb_native_wave_pack(2.f/6.f,1u)==0u);
 assert(jb_native_wave_pack(3.f/6.f,1u)==0u);
 assert(jb_native_wave_pack(5.f/6.f,1u)==0u);
 assert(jb_native_wave_pack(1.f,1u)==0u);
 for(unsigned i=0u;i<300u;i++){
  const float x=(float)i/600.f;
  const uint32_t state=jb_native_wave_pack(x,1u);
  if(state){
    assert(state&JB_NATIVE_WAVE_MORPH_ON);
    const unsigned base=(state>>JB_NATIVE_WAVE_BASE_SHIFT)&3u;
    const unsigned next=(state>>JB_NATIVE_WAVE_NEXT_SHIFT)&3u;
    assert(base<3u&&next==base+1u);
    assert((state&JB_NATIVE_WAVE_Q15_MASK)>0u);
    const int16_t value=jb_native_wave_blend(-32000,32000,state);
    assert(value>=-32000 && value<=32000);
    assert(jb_native_wave_blend(7000,7000,state)==7000);
  }
 }
 const uint32_t a=jb_native_wave_pack(.25f,1u); /* triangle→saw */
 assert(a&JB_NATIVE_WAVE_MORPH_ON);
 assert(((a>>JB_NATIVE_WAVE_BASE_SHIFT)&3u)==1u);
 assert(((a>>JB_NATIVE_WAVE_NEXT_SHIFT)&3u)==2u);
 const int16_t blend=jb_native_wave_blend(-10000,10000,a);
 assert(blend> -100 && blend<100);
 assert(jb_native_wave_blend(-4000,4000,0)==-4000);
 puts("PASS JB-27: normalized LFO waveform scan, native 4 periodic waves, 16-bit bounded interpolation, nonperiodic bypass");
 return 0;
}
