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
 uint8_t table, restart;
} Layer;
static Layer layers[JB_VOICES];
static float clamp(float x, float a, float b) { return x<a?a:(x>b?b:x); }
static float unit(uint8_t x) { return x*(1.f/127.f); }
/* All callers pass bounded phases. Integer conversion avoids libm floorf per sample. */
static float wrap(float x) { float f=x-(int)x; if(f<0) f+=1; return f>=1?0:f; }
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
static float wave(float phase, float position, unsigned band) {
 float p=wrap(phase)*1024.f; unsigned i=(unsigned)p, j=(i+1)&1023;
 float f=p-i, a, b; position=clamp(position,0.f,3.f);
 unsigned w=(unsigned)position; float mix=position-w;
 if(w==0) { a=sine(phase); b=(triTable[band][i]+f*(triTable[band][j]-triTable[band][i]))/32768.f; }
 else if(w==1) { a=(triTable[band][i]+f*(triTable[band][j]-triTable[band][i]))/32768.f; b=(sawTable[band][i]+f*(sawTable[band][j]-sawTable[band][i]))/32768.f; }
 else { a=(sawTable[band][i]+f*(sawTable[band][j]-sawTable[band][i]))/32768.f; b=(recTable[band][i]+f*(recTable[band][j]-recTable[band][i]))/32768.f; if(w==3) return b; }
 return a+mix*(b-a);
}
uint8_t jb_limit(unsigned p) { return p==JB_MODE?3:127; }
void jb_adjust(unsigned v, unsigned p, int delta) {
 if(v>=JB_VOICES || p>=JB_PARAMS) return;
 int n=jb_parameters[v][p]+delta;
 jb_parameters[v][p]=n<0?0:(n>jb_limit(p)?jb_limit(p):n);

}
void jb_defaults(uint8_t *kit) {
 static const uint8_t defaults[JB_PARAMS]={0,64,64,48, 32,0,0,0, 64,64,64,64,
 48,24,32,0, 64,0,64,20, 0,0,0,127, 0,0,0,64};
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
 if(!p[JB_MODE] || !velocity) {
  uint32_t m=lock(); layers[v]=next; unlock(m); return;
 }
 uint32_t seed=layers[v].rng?layers[v].rng:0x917b15u+v*31337u;
 s->rng=seed; s->restart=1;
 s->carrier=s->modulator=s->wave=.25f;
 float vel=unit(velocity>127?127:velocity);
 s->metal=clamp(unit(p[JB_METAL])+(p[JB_VMETAL]-64)/63.f*vel,0,1);
 s->noise=clamp(unit(p[JB_NOISE])+(p[JB_VNOISE]-64)/63.f*vel,0,1);
 s->aggression=clamp(unit(p[JB_AGGRESSION])+(p[JB_VAGGRESSION]-64)/63.f*vel,0,1);
 s->texture=clamp(unit(p[JB_TEXTURE])+(p[JB_VTEXTURE]-64)/63.f*vel,0,1);
 s->random=noise(s)*unit(p[JB_RANDOM]);
 int pitch=(int)p[JB_PITCH]+(int)note-48; pitch=pitch<0?0:(pitch>127?127:pitch);
 s->frequency=clamp(MidiNoteFrequencies[pitch]*(1.f+s->random*.025f),10,12000)/REAL_FS;
 s->table=(pitch+11)/12; if(s->table>10) s->table=10;
 s->ratio=.25f+unit(p[JB_RATIO])*7.75f+s->metal*1.41421356f;
 s->depth=unit(p[JB_DEPTH])*(.1f+2.f*s->metal);
 s->fb=unit(p[JB_FEEDBACK])*.45f; // aggression does not change FM ratios/depth
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
void jb_render(uint8_t v,int16_t *buffer,unsigned size,float gain) {
 if(v>=JB_VOICES) return;
 Layer *s=&layers[v]; const uint8_t *p=jb_parameters[v];
 if(!p[JB_MODE]) { s->envelope=0; s->tail=0; s->last=0; return; } // bit-exact buffer bypass
 float fmGain=(p[JB_MODE]&1)?unit(p[JB_METAL_LEVEL]):0;
 float wtGain=(p[JB_MODE]&2)?unit(p[JB_WAVE_LEVEL]):0;
 float drive=1+unit(p[JB_DRIVE])*12+s->aggression*15;
 float crush=1+(unsigned)(unit(p[JB_BITS])*11+s->aggression*3);
 float steps=(float)(1u<<(unsigned)clamp(16-crush,2,15));
 float colour=clamp(unit(p[JB_COLOUR])+s->noise*.25f,0,1);
 float noiseGain=clamp(unit(p[JB_NOISE_LEVEL])+s->noise,0,1);
 float pitchEnv=(p[JB_PITCH_ENV]-64)/64.f;
 float move=unit(p[JB_MOVEMENT]);
 float motionInc=(.2f+8*move)/REAL_FS;
 float position=unit(p[JB_POSITION])*3;
 float wet=clamp(unit(p[JB_DRIVE])+s->aggression,0,1);
 float character=unit(p[JB_CHARACTER]);
 int needsMotion=s->texture>0 || (wtGain>0 && move>0);
 for(unsigned i=0;i<size;i++) {
  if(s->envelope<.00003f && fabsf(s->tail)<.00003f) { s->last=0; break; }
  float inc=clamp(s->frequency*(1+pitchEnv*s->transient*3),.00001f,.4f);
  s->carrier=wrap(s->carrier+inc);
  s->modulator=wrap(s->modulator+fminf(inc*s->ratio,.45f));
  s->motion=wrap(s->motion+motionInc);
  float movement=needsMotion?sine(s->motion):0;
  if(wtGain>0) s->wave=wrap(s->wave+inc*s->detune*(1+s->texture*.002f*movement));
  float out=0;
  if(fmGain>0) {
   float mod=sine(s->modulator+s->feedback*s->fb);
   s->feedback=mod;
   float metal=sine(s->carrier+mod*s->depth*(.25f+.75f*s->envelope)+s->texture*.03f*movement);
   if(noiseGain>0) {
    float n=noise(s); s->lowNoise+=(.005f+.45f*colour)*(n-s->lowNoise);
    n=colour<.5f?s->lowNoise:(n-s->lowNoise);
    metal+=n*noiseGain*(.3f+.7f*s->transient);
   }
   out+=fmGain*metal;
  }
  if(wtGain>0) {
   float pos=position+(move+s->texture)*movement+s->random*.3f;
   out+=wtGain*wave(s->wave,pos,s->table);
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
  float mixed=buffer[i]+out*gain*12000.f;
  buffer[i]=(int16_t)clamp(mixed,-32768,32767);
 }
}
