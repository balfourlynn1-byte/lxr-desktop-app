/* JB-03: deterministic overlapping four-voice modulation regression.
 * Exercises production Jimbones DSP, not the entire hardware mixer. */
#include "Soundengine/DSPAudio/Jimbones.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#define N 64
#define BLOCKS 256
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
static uint64_t run(unsigned active,unsigned *clipped) {
 int16_t block[N]; uint64_t hash=1469598103934665603ULL;
 setup(); *clipped=0;
 for(unsigned t=0;t<BLOCKS;t++) {
  for(unsigned v=0;v<active;v++) {
   if(t==0 || (t%43==0 && (v==1 || v==2)))
    jb_trigger(v,105+(v*5),48+v*4,1);
   memset(block,0,sizeof(block));
   jb_render(v,block,N,1.f);
   for(unsigned i=0;i<N;i++) {
    uint16_t sample=(uint16_t)block[i];
    *clipped+=(block[i]==32767 || block[i]==-32768);
    hash=(hash^(sample&255))*1099511628211ULL;
    hash=(hash^(sample>>8))*1099511628211ULL;
   }
  }
 }
 return hash;
}
int main(void) {
 for(unsigned n=1;n<=4;n++) {
  unsigned a=0,b=0;
  uint64_t h1=run(n,&a),h2=run(n,&b);
  assert(h1==h2 && a==b);
  printf("PASS %u modulated voices, deterministic %u samples, layer-buffer clips=%u, hash=%016llx\n",
         n,n*BLOCKS*N,a,(unsigned long long)h1);
 }
 puts("NOTE: does not measure final hardware mixer, post-FX or CPU deadlines.");
 return 0;
}
