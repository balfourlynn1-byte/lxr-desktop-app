/* JIMBONES experimental layers. SPDX-License-Identifier: GPL-3.0-only */
#include "Soundengine/DSPAudio/Jimbones.h"
#include "Soundengine/DSPAudio/wavetable.h"
#include "Soundengine/config.h"
#include "Soundengine/MIDI/MidiNoteNumbers.h"
#include <math.h>
#include <string.h>
uint8_t jb_parameters[JB_VOICES][JB_PARAMS];
typedef struct {
 float carrier, modulator, wave, motion, envelope, transient, feedback, lowNoise;
 float frequency, ratio, depth, fb, detune, metal, noise, aggression, texture;
 float decay, transientDecay, velocity, random, last, tail;
 uint32_t rng;
 uint8_t table, restart, editPending, fadeLeft;
 float lastAdded, editStep;
 float hitVelocity;
} Layer;
static Layer layers[JB_VOICES];
volatile uint32_t jb_layerSaturationHits;
static float clamp(float x, float a, float b) { return x<a?a:(x>b?b:x); }
static float unit(uint8_t x) { return x*(1.f/127.f); }
/* All callers pass bounded phases. Integer conversion avoids libm floorf per sample. */
static float wrap(float x) { float f=x-(int)x; if(f<0) f+=1; return f>=1?0:f; }
/* Digitone II / Syntakt-inspired techniques, independently implemented.
 * LXR02Open similarly crossfades adjacent waveform IDs during LFO sweeps.
 * This implementation keeps the existing LXR oscillator tables and saves
 * both compute and memory by using the current wavetable layer only. */
static float jb_phaseWarp(float phase,float shape) {
 const float pivot=clamp(.5f+shape*.41f,.09f,.91f);
 return phase<pivot?phase*(.5f/pivot):.5f+(phase-pivot)*(.5f/(1.f-pivot));
}
/* Triangle wavefold in fixed work: bounded for input within [-1,1] and
 * drive <= 6.5; no expensive sin/tanh or additional wavetable lookups. */
static float jb_waveFold(float input,float drive) {
 float x=input*drive;
 for(unsigned n=0;n<5;n++) {
  if(x>1.f) x=2.f-x;
  else if(x< -1.f) x=-2.f-x;
  else break;
 }
 return clamp(x,-1.f,1.f);
}
/* Reflect wavetable scans at both ends to avoid abrupt sweep wraparound.
 * The modulated position is at most 0..3 plus <=3.5, safely within 0..6.5. */
static float jb_waveScan(float x) {
 if(x<0.f) x=-x;
 if(x>6.f) x=12.f-x;
 return x>3.f?6.f-x:x;
}
static uint32_t lock(void) { uint32_t mask=__get_PRIMASK(); __disable_irq(); return mask; }
static void unlock(uint32_t mask) { __DMB(); __set_PRIMASK(mask); }
static float noise(Layer *s) {
 uint32_t x=s->rng; x^=x<<13; x^=x>>17; x^=x<<5; s->rng=x;
 return (float)(x>>8)*(1.f/8388608.f)-1.f;
}
static float sine(float phase) {
 float pos=wrap(phase)*4096.f; unsigned i=(unsigned)pos;
 return (sine_table[i]+(pos-i)*(sine_table[i+1]-sine_table[i]))*(1.f/32768.f);
}
/* Crossfade existing, octave-banded oscillator tables; no duplicated wave bank. */
static volatile uint8_t jb_waveInterpEnabled=1;
void jb_setWaveInterpolation(uint8_t enabled) { jb_waveInterpEnabled=(enabled!=0); }
uint8_t jb_getWaveInterpolation(void) { return jb_waveInterpEnabled; }
static float wave(float phase, float position, unsigned band) {
 float p=wrap(phase)*1024.f; unsigned i=(unsigned)p, j=(i+1)&1023;
 float f=p-i, a, b; position=clamp(position,0.f,3.f);
 if(!jb_waveInterpEnabled) position=clamp((float)((unsigned)(position+0.5f)),0.f,3.f);
 unsigned w=(unsigned)position; float mix=position-w;
 if(w==0) { a=sine(phase); b=(triTable[band][i]+f*(triTable[band][j]-triTable[band][i]))/32768.f; }
 else if(w==1) { a=(triTable[band][i]+f*(triTable[band][j]-triTable[band][i]))/32768.f; b=(sawTable[band][i]+f*(sawTable[band][j]-sawTable[band][i]))/32768.f; }
 else { a=(sawTable[band][i]+f*(sawTable[band][j]-sawTable[band][i]))/32768.f; b=(recTable[band][i]+f*(recTable[band][j]-recTable[band][i]))/32768.f; if(w==3) return b; }
 return a+mix*(b-a);
}
uint8_t jb_limit(unsigned p) { return p==JB_MODE?3:127; }
void jb_setControl(unsigned v,unsigned p,uint8_t val) {
 if(v>=JB_VOICES || p>=JB_PARAMS || val>jb_limit(p)) return;
 uint32_t mask=lock();
 if(jb_parameters[v][p]!=val) {
  jb_parameters[v][p]=val;
  if(p==JB_METAL_LEVEL || p==JB_WAVE_LEVEL || p==JB_POSITION || p==JB_SHAPE ||
     p==JB_DRIVE || p==JB_BLEND || p==JB_VBLEND)
   layers[v].editPending=1;
 }
 unlock(mask);
}
void jb_adjust(unsigned v, unsigned p, int delta) {
 if(v>=JB_VOICES || p>=JB_PARAMS) return;
 int n=jb_parameters[v][p]+delta;
 jb_setControl(v,p,n<0?0:(n>jb_limit(p)?jb_limit(p):n));

}
void jb_defaults(uint8_t *kit) {
 static const uint8_t defaults[JB_PARAMS]={0,64,64,48, 32,0,0,0, 64,64,64,64,
 48,24,32,0, 64,0,64,20, 0,0,0,127, 0,0,0,64, 127,64,127,64};
 for(unsigned v=0;v<JB_VOICES;v++) memcpy(kit+v*JB_PARAMS,defaults,JB_PARAMS);
}
void jb_silence(void) { uint32_t m=lock(); memset(layers,0,sizeof(layers)); unlock(m); }
void jb_loadKit(const uint8_t *kit) {
 uint32_t m=lock(); memcpy(jb_parameters,kit,JB_KIT_BYTES);
 memset(layers,0,sizeof(layers)); unlock(m);
}
void jb_init(void) { jb_defaults(&jb_parameters[0][0]); jb_silence(); }
void jb_trigger(uint8_t v,uint8_t velocity,uint8_t note,uint8_t open) {
 if(v>=JB_VOICES) return;
 Layer next={0}, *s=&next; const uint8_t *p=jb_parameters[v];
 if(!(p[JB_MODE]&2u) || !velocity) {
  uint32_t m=lock(); layers[v]=next; unlock(m); return;
 }
 uint32_t seed=layers[v].rng?layers[v].rng:0x917b15u+v*31337u;
 s->rng=seed; s->restart=1;
 s->carrier=s->modulator=s->wave=.25f;
 float vel=unit(velocity>127?127:velocity);
 s->hitVelocity=vel;
#if JB_METALLIC_ENABLED
 s->metal=clamp(unit(p[JB_METAL])+(p[JB_VMETAL]-64)/63.f*vel,0,1);
#endif
 s->noise=clamp(unit(p[JB_NOISE])+(p[JB_VNOISE]-64)/63.f*vel,0,1);
 s->aggression=clamp(unit(p[JB_AGGRESSION])+(p[JB_VAGGRESSION]-64)/63.f*vel,0,1);
 s->texture=clamp(unit(p[JB_TEXTURE])+(p[JB_VTEXTURE]-64)/63.f*vel,0,1);
 s->random=noise(s)*unit(p[JB_RANDOM]);
 int pitch=(int)p[JB_PITCH]+(int)note-48; pitch=pitch<0?0:(pitch>127?127:pitch);
 s->frequency=clamp(MidiNoteFrequencies[pitch]*(1.f+s->random*.025f),10,12000)/REAL_FS;
 s->table=(pitch+11)/12; if(s->table>10) s->table=10;
#if JB_METALLIC_ENABLED
 s->ratio=.25f+unit(p[JB_RATIO])*13.75f+s->metal*4.41421356f;
 s->depth=unit(p[JB_DEPTH])*(.25f+8.5f*s->metal);
 s->fb=unit(p[JB_FEEDBACK])*.78f;
#endif
 s->detune=1.f+(p[JB_DETUNE]-64)*(.03f/64.f);
 float seconds=.015f+unit(p[JB_DECAY])*unit(p[JB_DECAY])*4.f;
 if(v==5 && !open) seconds=fminf(seconds,.12f); // closed hit replaces open tail
 s->decay=expf(-6.907755f/(REAL_FS*seconds));
 s->transientDecay=expf(-6.907755f/(REAL_FS*(.001f+unit(p[JB_TRANSIENT])*.15f)));
 s->envelope=1; s->transient=1;
 s->velocity=1-unit(p[JB_VELOCITY])*(1-vel);
 /* Expensive coefficient math runs with interrupts enabled. Publish one bounded state. */
 uint32_t m=lock(); next.last=layers[v].last; layers[v]=next; unlock(m);
}
static float blend(unsigned v) {
 const uint8_t *p=jb_parameters[v];
 float depth=(p[JB_VBLEND]-64)/64.f;
 float velocity=layers[v].hitVelocity;
 return unit(p[JB_BLEND])*(1.f-(depth>=0?depth*(1.f-velocity):-depth*velocity));
}
static float activeLevel(const uint8_t *p) {
 /* FM-only presets retain their data but must not attenuate native audio. */
 return (p[JB_MODE]&2u)?unit(p[JB_WAVE_LEVEL]):0.f;
}
/* Fixed fader law before summing. Depends on control positions only. */
uint8_t jb_liveBalance;
static float kitWeight;
void jb_prepareBalance(void) {
 kitWeight=0;
 if(!jb_liveBalance) return;
 for(unsigned v=0;v<JB_VOICES;v++) {
  const uint8_t *p=jb_parameters[v];
  float weight=fminf(1.f,2.f*activeLevel(p))*unit(p[JB_BLEND]);
  kitWeight=fmaxf(kitWeight,weight);
 }
}
void jb_stageVoice(uint8_t v,int16_t *samples,unsigned size,float fader) {
 if(fader<=.161f) return;
 float active=fminf(1.f,activeLevel(jb_parameters[v])*2)*unit(jb_parameters[v][JB_BLEND]);
 if(v!=0 && kitWeight>0) active=fmaxf(active,fminf(1.f,kitWeight*8.f));
 if(!active) return;
 float staged=.161f+.04f*(fader-.161f);
 float trim=1.f-active*(1.f-staged/fader);
 for(unsigned i=0;i<size;i++) samples[i]=(int16_t)(samples[i]*trim);
}
void jb_render(uint8_t v,int16_t *buffer,unsigned size,float gain) {
 if(v>=JB_VOICES) return;
 Layer *s=&layers[v]; const uint8_t *p=jb_parameters[v];
 if(!(p[JB_MODE]&2u)) { s->envelope=0; s->tail=0; s->last=0; return; } // FM-only: bit-exact native bypass
#if JB_METALLIC_ENABLED
 float fmGain=(p[JB_MODE]&1u)?unit(p[JB_METAL_LEVEL]):0.f;
#endif
 float wtGain=(p[JB_MODE]&2u)?unit(p[JB_WAVE_LEVEL]):0.f;
 float mix=blend(v);
 if(wtGain==0 || mix==0) { s->last=s->tail=s->envelope=0; return; }
 float shape=fminf(1.f,p[JB_SHAPE]/64.f);
 float extraShape=fmaxf(0.f,(p[JB_SHAPE]-64)/63.f);
 float drive=1+unit(p[JB_DRIVE])*12+s->aggression*15;
 float crush=1+(unsigned)(unit(p[JB_BITS])*11+s->aggression*3);
 float steps=(float)(1u<<(unsigned)clamp(16-crush,2,15));
  float pitchEnv=(p[JB_PITCH_ENV]-64)/64.f;
 float move=unit(p[JB_MOVEMENT]);
 float motionInc=(.2f+8*move+12*s->texture)/REAL_FS;
 float position=unit(p[JB_POSITION])*3;
 float wet=clamp(unit(p[JB_DRIVE])+s->aggression,0,1);
 float character=unit(p[JB_CHARACTER]);
 int needsMotion=s->texture>0 || (wtGain>0 && move>0);
 for(unsigned i=0;i<size;i++) {
  if(s->envelope<.00003f && fabsf(s->tail)<.00003f) { s->last=0; break; }
  float inc=clamp(s->frequency*(1+pitchEnv*s->transient*5),.00001f,.4f);
#if JB_METALLIC_ENABLED
  s->carrier=wrap(s->carrier+inc);
  s->modulator=wrap(s->modulator+fminf(inc*s->ratio,.45f));
#endif
  s->motion=wrap(s->motion+motionInc);
  float movement=needsMotion?sine(s->motion):0;
  if(wtGain>0) s->wave=wrap(s->wave+inc*s->detune*(1+s->texture*.025f*movement));
  float out=0;
#if JB_METALLIC_ENABLED
  if(fmGain>0) {
   float mod=sine(s->modulator+s->feedback*s->fb);
   s->feedback=mod;
   float metal=sine(s->carrier+mod*s->depth*(.25f+.75f*s->envelope)*(1+s->texture*.8f*movement)+s->texture*.35f*movement);
   /* More prominent inharmonic ring/fold texture without raising output level. */
   if(s->aggression>0.f) {
    const float fold=fabsf(metal*3.f)-1.f;
    const float ring=metal*mod;
    const float amount=s->aggression*.8f;
    metal=(1.f-amount)*metal+amount*(ring+fold*.6f);
   }
   out+=fmGain*metal;
  }
#endif
  if(wtGain>0) {
   float pos=position+(move+2.5f*s->texture)*movement+s->random*.3f;
   /* LXR02Open-style adjacent waveform interpolation (already native to
    * this layer), plus phase distortion and a post-waveform wavefolder. */
   const float scan=jb_waveScan(pos);
   /* CHAR=0 is neutral, preserving older patches at zero; higher CHAR
    * increases phase asymmetry even when the overdrive is disabled.
    * Texture adds bipolar movement synchronized to the internal motion. */
   const float warp=clamp(character*.82f+s->texture*.15f*movement,-.88f,.88f);
   float shaped=wave(jb_phaseWarp(s->wave,warp*shape+(.88f-warp)*extraShape),scan,s->table);
   const float foldDepth=s->aggression*shape+(1.f-s->aggression)*extraShape;
   if(foldDepth>0.f) {
    const float fold=jb_waveFold(shaped,1.f+foldDepth*5.5f);
    shaped+=(fold-shaped)*(foldDepth*.95f);
   }
   out+=wtGain*shaped;
  }
  if(wet>0) {
   float driven=out*drive;
   float soft=driven/(1+fabsf(driven));
   float hard=clamp(driven,-1,1);
   out+=wet*((soft+(hard-soft)*character)-out);
  }
  if(p[JB_BITS] || s->aggression>0) { float q=out*steps; out=(int)(q+(q<0?-.5f:.5f))/steps; }
  out=out*s->envelope*s->velocity;
  /* Match the previous output at the boundary, rather than adding it to the
     new attack. The correction decays in ~2.5 ms; there is no event delay. */
  if(s->restart) { s->tail=s->last-out; s->restart=0; }
  out+=s->tail;
  s->tail*=.94f; s->last=out;
  s->envelope*=s->decay; s->transient*=s->transientDecay;
  /* Spend the headroom on the side of the dry waveform the layer moves
     toward. A silent layer therefore preserves even full-scale dry audio. */
  float added=out*gain*15500.f*mix;
  if(s->editPending) {
   s->editStep=(s->lastAdded-added)/64.f;
   s->fadeLeft=64; s->editPending=0;
  }
  if(s->fadeLeft) { added+=s->editStep*s->fadeLeft; --s->fadeLeft; }
  s->lastAdded=added;
  float mag=fabsf(added);
  if(mag>24000.f) {
   ++jb_layerSaturationHits;
   float excess=mag-24000.f;
   added=copysignf(24000.f+excess*8192.f/(8192.f+excess),added);
  }
  float dry=buffer[i];
  float room=1.f-fmaxf(0.f,(added<0.f?-dry:dry)/32768.f);
  float mixed=dry+added*room;
  buffer[i]=(int16_t)clamp(mixed,-32768.f,32767.f);
 }
}

/* Added experimental noise is injected ahead of each voice's native filter.
   Mode 0 bypasses this entirely; jb_render is the only envelope-time clock. */
void jb_preFilterNoise(uint8_t v,int16_t *buffer,unsigned size) {
 if(v>=JB_VOICES || !buffer) return;
 const uint8_t *p=jb_parameters[v];
 if(!(p[JB_MODE]&2u)) return;
 Layer *s=&layers[v];
 if(s->envelope<.00003f) return;
 float level=fmaxf(unit(p[JB_NOISE_LEVEL]),s->noise)*fminf(1.f,2.f*activeLevel(p))*blend(v);
 if(level<=0.f) return;
 const float colour=clamp(unit(p[JB_COLOUR]),0.f,1.f);
 const float smooth=.015f+.38f*colour;
 const float wet=clamp(level*s->envelope*s->velocity,0.f,1.f);
 for(unsigned i=0;i<size;i++) {
  const float white=noise(s);
  s->lowNoise+=smooth*(white-s->lowNoise);
  const float shaped=colour<.5f?s->lowNoise:(white-s->lowNoise);
  /* Bounded gain staging prior to original filter stage. */
  const float mixed=buffer[i]*(1.f-wet*.28f)+shaped*wet*12500.f;
  buffer[i]=(int16_t)clamp(mixed,-32768.f,32767.f);
 }
}
