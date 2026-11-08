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
static void interpolation_audio(void) {
 int16_t a[2048],b[2048],c[2048];
 jb_init();
 jb_parameters[0][JB_MODE]=2;
 jb_parameters[0][JB_POSITION]=63;
 jb_parameters[0][JB_MOVEMENT]=0;
 jb_parameters[0][JB_TEXTURE]=0;
 jb_setWaveInterpolation(1);
 jb_trigger(0,100,48,0); render(0,a,2048);
 jb_silence(); jb_setWaveInterpolation(0);
 jb_trigger(0,100,48,0); render(0,b,2048);
 assert(memcmp(a,b,sizeof(a))!=0);
 jb_silence(); jb_setWaveInterpolation(1);
 jb_trigger(0,100,48,0); render(0,c,2048);
 assert(memcmp(a,c,sizeof(a))==0);
 puts("PASS waveform interpolation ON/OFF changes sound and ON restores exact waveform");
}
static void lofi_live_toggle_smoke(void) {
 int16_t a[256],b[256];
 jb_init();
 jb_parameters[0][JB_MODE]=2;
 jb_trigger(0,110,48,0);
 render(0,a,256);
 jb_setSampleRateReduction(0,16);
 render(0,b,256);
 jb_setSampleRateReduction(0,0);
 render(0,b,256);
 jb_setSampleRateReduction(0,32);
 render(0,b,256);
 jb_setSampleRateReduction(0,0);
 render(0,b,256);
 /* Deterministic host smoke; audio quality still requires hardware listening. */
 puts("PASS live LOFI bypass and factor-change render smoke");
}
static void kit_revision_reset(void) {
 uint8_t kit[JB_KIT_BYTES];
 jb_defaults(kit);
 uint32_t before=jb_kitRevision();
 jb_init();
 assert(jb_kitRevision()!=before);
 before=jb_kitRevision();
 jb_setTransient(0,32,-24);
 jb_setSampleRateReduction(0,8);
 jb_loadKit(kit);
 assert(jb_kitRevision()!=before);
 assert(jb_parameters[0][JB_MODE]==0);
 puts("PASS DSP kit revision changes on init/load; UI reset requires separate integration test");
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
/* JB-23: deterministic additive four-voice PCM fixture with a conservative
 * 4 x 0.25 gain. This tests mixed output rather than per-voice buffers only.
 * The actual LXR hardware mixer and DAC remain unverified. */
static void four_voice_mix_regression(void) {
 int16_t voice[4][64];
 unsigned rails=0,blocks=0,active=0;
 jb_init();
 for(unsigned v=0;v<4;v++) {
  jb_parameters[v][JB_MODE]=3;
  jb_parameters[v][JB_DECAY]=127;
  jb_setTransient(v,32,24);
  jb_setSampleRateReduction(v,4);
  jb_trigger(v,100,45+v*3,0);
 }
 for(unsigned block=0;block<300;block++) {
  for(unsigned v=0;v<4;v++) render(v,voice[v],64);
  for(unsigned i=0;i<64;i++) {
   int32_t sum=0;
   for(unsigned v=0;v<4;v++) sum+=voice[v][i];
   int32_t mixed=sum/4;
   if(mixed>=32767 || mixed<=-32768)rails++;
   if(mixed!=0)active++;
  }
  blocks++;
 }
 assert(active>0);
 assert(rails==0);
 printf("PASS conservative 4-voice PCM mix: %u blocks, %u rail samples (hardware mixer unverified)\n",blocks,rails);
}
static void transient_four_voice_stress(void) {
 int16_t dry[4][64],wet[4][64];
 jb_init();
 for(unsigned v=0;v<4;v++) {
  jb_parameters[v][JB_MODE]=3;
  jb_parameters[v][JB_DECAY]=127;
  jb_setTransient(v,64,64);
  jb_trigger(v,127,48+v*4,0);
 }
 unsigned nonzero=0,rails=0;
 for(unsigned block=0;block<300;block++) {
  for(unsigned v=0;v<4;v++) {
   for(unsigned i=0;i<64;i++)dry[v][i]=wet[v][i]=(int16_t)((i%2)?12000:-12000);
   jb_render(v,wet[v],64,1.f);
   for(unsigned i=0;i<64;i++) {
    nonzero+=(wet[v][i]!=dry[v][i]);
    rails+=(wet[v][i]==32767 || wet[v][i]==-32768);
   }
  }
 }
 assert(nonzero>0);
 /* Per-voice renderer must not drive a pre-existing 12k PCM fixture to rails. */
 assert(rails==0);
 jb_init();
 for(unsigned v=0;v<4;v++) {
  jb_parameters[v][JB_MODE]=3;
  jb_trigger(v,127,48+v*4,0);
  render(v,dry[v],64);
 }
 puts("PASS four concurrent transient-enabled voices, long decays, no output rails");
}
static void transient_integration(void) {
 int16_t baseline[2048],bypass[2048],boost[2048];
 jb_init();jb_parameters[1][JB_MODE]=1;
 jb_trigger(1,100,48,0);render(1,baseline,2048);
 jb_init();jb_parameters[1][JB_MODE]=1;
 jb_setTransient(1,0,0);jb_trigger(1,100,48,0);render(1,bypass,2048);
 assert(memcmp(baseline,bypass,sizeof(baseline))==0);
 jb_init();jb_parameters[1][JB_MODE]=1;
 jb_setTransient(1,64,0);jb_trigger(1,100,48,0);render(1,boost,2048);
 assert(memcmp(baseline,boost,sizeof(baseline))!=0);
 jb_setTransient(9,64,64);jb_setTransient(1,100,0);
 jb_init();jb_parameters[1][JB_MODE]=1;
 jb_trigger(1,100,48,0);render(1,bypass,2048);
 assert(memcmp(baseline,bypass,sizeof(baseline))==0);
 puts("PASS transient default-off bit identity, optional audio change, init reset");
}
static void srr_integration(void) {
 int16_t baseline[2048],bypass[2048],reduced[2048];
 jb_init();jb_parameters[1][JB_MODE]=1;
 jb_trigger(1,100,48,0);render(1,baseline,2048);
 jb_init();jb_parameters[1][JB_MODE]=1;
 jb_setSampleRateReduction(1,1);jb_trigger(1,100,48,0);render(1,bypass,2048);
 assert(!memcmp(baseline,bypass,sizeof(baseline)));
 jb_init();jb_parameters[1][JB_MODE]=1;
 jb_setSampleRateReduction(1,8);jb_trigger(1,100,48,0);render(1,reduced,2048);
 assert(memcmp(baseline,reduced,sizeof(baseline)));
 jb_setSampleRateReduction(1,33); /* invalid input leaves setting unchanged */
 jb_init();jb_parameters[1][JB_MODE]=1;
 jb_trigger(1,100,48,0);render(1,bypass,2048);
 assert(!memcmp(baseline,bypass,sizeof(baseline)));
 puts("PASS SRR bypass identity, opt-in audio change, init reset");
}
int main(int argc,char **argv) { four_voice_mix_regression(); lofi_live_toggle_smoke();
 kit_revision_reset(); srr_integration(); transient_four_voice_stress(); transient_integration(); interpolation_audio();
 interpolation_control(); bypass(); storage(); voices(); reproducible(); stress(); if(argc>1) audio(argv[1]); return 0; }
