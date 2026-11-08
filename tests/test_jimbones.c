/* Host checks use the production DSP and the production wave tables. */
#include "Soundengine/DSPAudio/Jimbones.h"
#include "Preset/JimbonesPreset.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
static void render(unsigned v,int16_t *b,unsigned n) { memset(b,0,n*sizeof(*b)); jb_render(v,b,n,1); }
static double energy(const int16_t *b,unsigned n) { double e=0; for(unsigned i=0;i<n;i++) e+=(double)b[i]*b[i]; return e; }
static void interpolation_control(void) {
 jb_setWaveInterpolation(1);
 assert(jb_getWaveInterpolation()==1);
 jb_setWaveInterpolation(0);
 assert(jb_getWaveInterpolation()==0);
 jb_setWaveInterpolation(2);
 assert(jb_getWaveInterpolation()==1);
 jb_setWaveInterpolation(1);
 puts("PASS interpolation global control ON/OFF and normalization");
}
static void bypass(void) {
 jb_init(); int16_t b[256],ref[256];
 for(unsigned i=0;i<256;i++) b[i]=ref[i]=(int16_t)(i*257-32768);
 for(unsigned v=0;v<6;v++) { assert(!jb_parameters[v][JB_MODE]); jb_trigger(v,127,48,0); jb_render(v,b,256,1); assert(!memcmp(b,ref,sizeof(b))); }
 puts("PASS default/off buffer bit identity on all six voices");
}
static void storage(void) {
 uint8_t src[JB_KIT_BYTES],dst[JB_KIT_BYTES],def[JB_KIT_BYTES],rec[JB_RECORD_BYTES],bad[JB_RECORD_BYTES];
 jb_defaults(def); jb_defaults(src);
 for(unsigned i=0;i<JB_KIT_BYTES;i++) src[i]=i%(jb_limit(i%JB_PARAMS)+1);
 jb_pack(rec,src); assert(jb_unpack(dst,rec,sizeof(rec))); assert(!memcmp(dst,src,sizeof(src)));
 for(unsigned i=0;i<sizeof(rec);i++) {
  assert(!jb_unpack(dst,rec,i)); assert(!memcmp(dst,def,sizeof(dst)));
  memcpy(bad,rec,sizeof(rec)); bad[i]^=1; assert(!jb_unpack(dst,bad,sizeof(bad))); assert(!memcmp(dst,def,sizeof(dst)));
 }
 uint8_t legacy[180]={'J','B','9','1',1,6,28,0};
 for(unsigned v=0;v<6;v++) memcpy(legacy+8+28*v,src+v*JB_PARAMS,28);
 uint32_t hash=2166136261u;
 for(unsigned i=0;i<176;i++) hash=(hash^legacy[i])*16777619u;
 for(unsigned i=0;i<4;i++) legacy[176+i]=hash>>(8*i);
 assert(jb_unpack(dst,legacy,sizeof(legacy)));
 for(unsigned v=0;v<6;v++){
  assert(!memcmp(dst+v*JB_PARAMS,src+v*JB_PARAMS,28));
  assert(!memcmp(dst+v*JB_PARAMS+28,def+v*JB_PARAMS+28,4));
 }
 src[JB_MODE]=127; jb_pack(rec,src); assert(!jb_unpack(dst,rec,sizeof(rec)));
 puts("PASS kit roundtrip; every truncation and single-byte corruption defaults OFF; invalid ranges rejected");
}
static void voices(void) {
 int16_t a[4096],b[4096];
 for(unsigned v=0;v<6;v++) for(unsigned mode=1;mode<=3;mode++) {
  jb_init(); jb_parameters[v][JB_MODE]=mode; jb_trigger(v,127,48,1); render(v,a,4096); assert(energy(a,4096)>10000);
  for(unsigned other=0;other<6;other++) if(other!=v) { render(other,b,4096); assert(energy(b,4096)==0); }
  jb_parameters[v][JB_METAL_LEVEL]=0; jb_parameters[v][JB_WAVE_LEVEL]=0;
  jb_silence(); jb_trigger(v,127,48,1); render(v,b,4096); assert(energy(b,4096)==0);
 }
 jb_init(); jb_parameters[0][JB_MODE]=1;
 jb_trigger(0,16,48,0); render(0,a,4096);
 jb_silence(); jb_trigger(0,127,48,0); render(0,b,4096); assert(energy(b,4096)>energy(a,4096)*10);
 jb_init(); jb_parameters[5][JB_MODE]=3; jb_parameters[5][JB_DECAY]=127;
 jb_trigger(5,127,48,1); render(5,a,4096); assert(energy(a,4096)>1000);
 jb_trigger(5,127,48,0); for(unsigned i=0;i<10;i++) render(5,b,4096); assert(energy(b,4096)==0);
 puts("PASS all voice/mode combinations, isolation, zero levels, velocity response, open-to-closed tail replacement");
}
static void reproducible(void) {
 int16_t a[4096],b[4096];
 jb_init(); jb_parameters[1][JB_MODE]=3; jb_parameters[1][JB_RANDOM]=127;
 jb_trigger(1,90,48,0); render(1,a,4096);
 jb_init(); jb_parameters[1][JB_MODE]=3; jb_parameters[1][JB_RANDOM]=127;
 jb_trigger(1,90,48,0); render(1,b,4096); assert(!memcmp(a,b,sizeof(a)));
 jb_parameters[1][JB_MODE]=0; render(1,b,4096); assert(energy(b,4096)==0);
 jb_parameters[1][JB_MODE]=3; render(1,b,4096); assert(energy(b,4096)==0);
 puts("PASS deterministic seeded render and disable/re-enable without stale tail");
}
static void stress(void) {
 int16_t b[64]; unsigned clips=0; unsigned seed=19;
 for(unsigned scenario=0;scenario<100;scenario++) {
  jb_init();
  for(unsigned v=0;v<6;v++) {
   for(unsigned p=0;p<JB_PARAMS;p++) { seed=seed*1664525u+1013904223u; jb_parameters[v][p]=scenario==0?0:(scenario==1?jb_limit(p):(seed>>24)%(jb_limit(p)+1)); }
   jb_parameters[v][JB_MODE]=3;
  }
  for(unsigned block=0;block<300;block++) for(unsigned v=0;v<6;v++) {
   if(block%3==0) jb_trigger(v,(block%127)+1,(block+v*19)%128,block%2);
   render(v,b,64);
   for(unsigned i=0;i<64;i++) clips+=b[i]==32767 || b[i]==-32768;
  }
 }
 printf("PASS 11,520,000 stress samples, rapid retriggers, all voices; clipped samples: %u\n",clips);
}
static void audio(const char *path) {
 FILE *f=fopen(path,"wb"); assert(f); int16_t b[64];
 for(unsigned mode=1;mode<=3;mode++) {
  jb_init(); jb_parameters[0][JB_MODE]=mode; jb_parameters[0][JB_POSITION]=70; jb_parameters[0][JB_MOVEMENT]=80;
  jb_parameters[0][JB_TEXTURE]=75; jb_parameters[0][JB_VMETAL]=100;
  for(unsigned hit=0;hit<4;hit++) {
   jb_trigger(0,31+hit*32,48,0);
   for(unsigned block=0;block<345;block++) { render(0,b,64); fwrite(b,sizeof(*b),64,f); }
  }
 }
 fclose(f);
}
int main(int argc,char **argv) {
 interpolation_control(); bypass(); storage(); voices(); reproducible(); stress(); if(argc>1) audio(argv[1]); return 0; }
