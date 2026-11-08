/* JB-03: isolated 4-voice summed PCM stress, NOT the hardware mixer.
 * Captures headroom needed before the final int16_t conversion.
 * SPDX-License-Identifier: GPL-3.0-only */
#include "Soundengine/DSPAudio/Jimbones.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#define FRAMES 64u
#define BLOCKS 256u
static void setup(void) {
 jb_init();
 for(unsigned v=0;v<4;v++) {
  jb_parameters[v][JB_MODE]=3;
  jb_parameters[v][JB_METAL_LEVEL]=115;
  jb_parameters[v][JB_WAVE_LEVEL]=110;
  jb_parameters[v][JB_MOVEMENT]=110;
  jb_parameters[v][JB_TEXTURE]=105;
  jb_parameters[v][JB_AGGRESSION]=80;
  jb_parameters[v][JB_BLEND]=110;
  jb_parameters[v][JB_DECAY]=112;
 }
}
static void measure(unsigned voices, uint64_t *exceeded, uint32_t *peak) {
 int32_t summed[FRAMES]; int16_t voice[FRAMES];
 *exceeded=0; *peak=0; setup();
 for(unsigned t=0;t<BLOCKS;t++) {
  memset(summed,0,sizeof(summed));
  for(unsigned v=0;v<voices;v++) {
   if(t==0 || (t%43==0 && (v==1 || v==2)))
    jb_trigger((uint8_t)v,(uint8_t)(105+v*5),(uint8_t)(48+v*4),1);
   memset(voice,0,sizeof(voice));
   jb_render((uint8_t)v,voice,FRAMES,1.f);
   for(unsigned i=0;i<FRAMES;i++) summed[i]+=(int32_t)voice[i];
  }
  for(unsigned i=0;i<FRAMES;i++) {
   int64_t x=summed[i]; uint32_t mag=(uint32_t)(x<0?-x:x);
   if(mag>*peak) *peak=mag;
   if(x>32767 || x< -32768) ++*exceeded;
  }
 }
}
int main(void) {
 for(unsigned n=1;n<=4;n++) {
  uint64_t a=0,b=0; uint32_t peak1=0,peak2=0;
  measure(n,&a,&peak1); measure(n,&b,&peak2);
  assert(a==b && peak1==peak2);
  printf("PASS isolated summed %u voices: above-int16=%llu/%u peak=%u (not final mixer)\n",
    n,(unsigned long long)a,FRAMES*BLOCKS,peak1);
 }
 return 0;
}
