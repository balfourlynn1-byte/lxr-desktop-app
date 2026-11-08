/* Bounded live-MIDI trigger effects, independent of synthesis. GPL-3.0-only. */
#include "Soundengine/StepFX.h"
#include "stm32f7xx_hal.h"
#include <string.h>
#define Q 65536u
#define FS 44140u
typedef struct { uint32_t due; uint16_t remaining; uint8_t voice,note,velocity; } Pending;
typedef struct { const char *name; uint8_t div,prob,variation,bars; uint16_t repeats; } Slot;
static const Slot slots[16]={
 {"OFF",2,100,0,0,0}, {"REP/2",2,100,0,0,3}, {"REP/4",4,100,0,0,3},
 {"REP/8",8,100,0,0,7}, {"REP/16",16,100,0,0,7},
 {"STUT/4",4,100,0,0,15}, {"STUT/8",8,100,0,0,31}, {"STUT16",16,100,0,0,31},
 {"PROB/4",4,50,0,0,15}, {"PROB/8",8,50,0,0,31},
 {"VEL/4",4,100,48,0,15}, {"VEL/8",8,100,48,0,31},
 {"2BAR",4,100,0,2,32}, {"4BAR",8,100,0,4,128},
 {"8BAR",8,100,0,8,256}, {"RND8B",16,50,64,8,512}};
static Pending pending[6];
static uint8_t selected,divisor=2,probability=100,mask=40;
/* Dub control masks only; existing native FX audio routing is unchanged. */
static uint8_t dubEnabled,dubSendMask=0x3eu,dubThrowMask;
static uint32_t phase,externalPhase,clockAge,clockRemainder,rng=0x53465831u;
static uint64_t fraction;
static uint8_t clockSeen,wasExternal;
static uint32_t lock(void) { uint32_t m=__get_PRIMASK();__disable_irq();return m; }
static void unlock(uint32_t m) { __DMB();__set_PRIMASK(m); }
static uint32_t random32(void) { rng^=rng<<13;rng^=rng>>17;rng^=rng<<5;return rng; }
static void cancel(void) { memset(pending,0,sizeof(pending)); }
void stepfx_init(void) { uint32_t m=lock();dubEnabled=0;dubSendMask=0x3eu;dubThrowMask=0;cancel();selected=0;divisor=2;probability=100;mask=40;phase=externalPhase=clockAge=clockRemainder=0;fraction=0;clockSeen=wasExternal=0;rng=0x53465831u;unlock(m); }
void stepfx_select(uint8_t slot) { if(slot>15)return;uint32_t m=lock();cancel();selected=slot;divisor=slots[slot].div;probability=slots[slot].prob;unlock(m); }
void stepfx_rate(int delta) { uint32_t m=lock();if(delta>0 && divisor<16)divisor*=2;if(delta<0 && divisor>2)divisor/=2;cancel();unlock(m); }
void stepfx_probability(int delta) { uint32_t m=lock();int p=probability+delta;probability=p<0?0:(p>100?100:p);unlock(m); }
void stepfx_include(uint8_t voice,uint8_t yes) { if(voice>5)return;uint32_t m=lock();if(yes)mask|=1u<<voice;else {mask&=~(1u<<voice);pending[voice].remaining=0;}unlock(m); }
uint8_t stepfx_slot(void){return selected;}
uint8_t stepfx_divisor(void){return divisor;}
uint8_t stepfx_chance(void){return probability;}
uint8_t stepfx_mask(void){return mask;}
const char *stepfx_name(uint8_t s){return slots[s<16?s:0].name;}
unsigned stepfx_pending(void){unsigned n=0;for(unsigned i=0;i<6;i++)n+=pending[i].remaining!=0;return n;}
void stepfx_stop(void){uint32_t m=lock();cancel();clockSeen=0;unlock(m);}
void stepfx_clock(void){
 uint32_t m=lock();
 if(!clockSeen) {externalPhase=phase;clockRemainder=0;clockSeen=1;}
 else {clockRemainder+=Q;externalPhase+=clockRemainder/24;clockRemainder%=24;}
 clockAge=0;unlock(m);
}
void stepfx_live(uint8_t voice,uint8_t note,uint8_t velocity){
 if(voice>6 || !velocity)return;
 uint8_t channel=voice>5?5:voice; uint32_t m=lock();
 /* Both hat articulations replace the same pending lane, even if excluded. */
 pending[channel].remaining=0;
 if(selected && (mask&(1u<<channel)) && (!wasExternal || clockSeen)) {
  uint32_t period=Q/divisor;
  pending[channel]=(Pending){(phase/period+1)*period,slots[selected].repeats,voice,note,velocity};
 }
 unlock(m);
}
void stepfx_process(uint32_t frames,uint16_t bpm,uint8_t external,StepFxEmit emit){
 Pending events[6];unsigned count=0;uint32_t m=lock();
 if(external!=wasExternal){cancel();clockSeen=0;wasExternal=external;fraction=0;}
 if(bpm<10)bpm=10;
 if(bpm>400)bpm=400;
 uint64_t step=(uint64_t)frames*bpm*Q;
 if(external){
  clockAge+=frames;
  if(clockAge>FS/2){cancel();clockSeen=0;}
  if(clockSeen){uint32_t advance=(uint64_t)clockAge*bpm*Q/(60u*FS);if(advance>Q/24)advance=Q/24;phase=externalPhase+advance;}
 }else{fraction+=step;phase+=(uint32_t)(fraction/(60u*FS));fraction%=60u*FS;}
 if(selected && (!external || clockSeen))for(unsigned v=0;v<6;v++) {
  Pending *p=&pending[v];
  if(p->remaining && (int32_t)(phase-p->due)>=0){
   /* At most one emission per lane per audio block; never catch up in a loop. */
   --p->remaining;p->due+=Q/divisor;
   if((int32_t)(phase-p->due)>=0)p->due=phase+Q/divisor;
   const uint32_t beat=phase/Q;
   const uint32_t bar=beat/4u;
   const uint32_t division=(phase/(Q/4u))%16u;
   const uint8_t phraseBars=slots[selected].bars;
   uint8_t sceneActive=1u;
   if(phraseBars) {
    const uint8_t sceneBar=(uint8_t)(bar%phraseBars);
    const uint8_t slot16=(uint8_t)division;
    const uint8_t dense=(sceneBar==phraseBars-2u);
    sceneActive=(sceneBar==phraseBars-1u)?(slot16==0u):
       (dense?((slot16&1u)==0u):(slot16==0u||slot16==7u||slot16==10u));
   }
   if(sceneActive && random32()%100<probability){
    events[count]=*p;
    int amount=slots[selected].variation;
    int vel=p->velocity;
    if(amount)vel+=(int)(random32()%(2*amount+1))-amount;
    events[count].velocity=vel<1?1:(vel>127?127:vel);
    /* Note transposition affects pitched oscillators; snare noise needs DSP pitch work. */
    if(selected>=4u) {
     static const int8_t intervals[8]={0,5,12,-7,7,19,-12,3};
     const uint8_t pos=(uint8_t)((phase/(Q/4u))&7u);
     int note=(int)p->note+intervals[(pos+selected)&7u];
     if(selected>=12u)note+=(int)((phase/(4u*Q))%4u)*3;
     if(note<0)note=0;
     if(note>127)note=127;
     events[count].note=(uint8_t)note;
    }
    count++;
   }
  }
 }
 unlock(m);
 for(unsigned i=0;i<count;i++)emit(events[i].voice,events[i].note,events[i].velocity);
}

/* UI-safe atomic state for future per-voice dub sends. Never activates the
 * global delay by itself or clears native delay tails. */
void stepfx_dubEnable(uint8_t enabled) {
 uint32_t m=lock(); dubEnabled=enabled?1u:0u; unlock(m);
}
void stepfx_dubVoice(uint8_t voice,uint8_t enabled) {
 if(voice>=6u)return;
 uint32_t m=lock();
 if(enabled)dubSendMask|=(uint8_t)(1u<<voice);
 else dubSendMask&=(uint8_t)~(1u<<voice);
 unlock(m);
}
void stepfx_dubThrow(uint8_t voice,uint8_t enabled) {
 if(voice>=6u)return;
 uint32_t m=lock();
 if(enabled)dubThrowMask|=(uint8_t)(1u<<voice);
 else dubThrowMask&=(uint8_t)~(1u<<voice);
 unlock(m);
}
uint8_t stepfx_dubEnabled(void){return dubEnabled;}
uint8_t stepfx_dubSendMask(void){return dubSendMask;}
uint8_t stepfx_dubThrowMask(void){return dubThrowMask;}
uint8_t stepfx_dubEffectiveMask(void){
 return dubEnabled?(uint8_t)(dubSendMask&dubThrowMask):0u;
}

/* Low-cost mono-to-stereo send tap. No separate delay memory and no changes
 * when master is disabled. Deliberately fixed -12 dB send to leave headroom.
 * Existing FX delay remains responsible for feedback and tail processing. */
void stepfx_dubTap(uint8_t voice,const int16_t *samples,int16_t *fxStereo,unsigned frames) {
 if(voice>=6u || !samples || !fxStereo || !(stepfx_dubEffectiveMask()&(1u<<voice)))return;
 for(unsigned i=0;i<frames;i++) {
  const int32_t add=(int32_t)samples[i]/4;
  for(unsigned ch=0;ch<2;ch++) {
   const unsigned k=2u*i+ch;
   int32_t mixed=(int32_t)fxStereo[k]+add;
   if(mixed>32767)mixed=32767;
   else if(mixed< -32768)mixed=-32768;
   fxStereo[k]=(int16_t)mixed;
  }
 }
}
