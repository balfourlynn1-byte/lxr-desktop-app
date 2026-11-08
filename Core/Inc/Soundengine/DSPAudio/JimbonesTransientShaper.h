/* JB-14 experimental transient shaping primitive, GPL-3.0-only.
 * Independent of LXR voice/kit structures until CLICK menu integration.
 * Disabled state is bit-identical; no heap, trigonometry or lookahead. */
#ifndef JIMBONES_TRANSIENT_SHAPER_H
#define JIMBONES_TRANSIENT_SHAPER_H
#include <stdint.h>
typedef struct { float fast,slow; } JbTransientState;
static inline void jb_transient_reset(JbTransientState *s) { s->fast=0.f; s->slow=0.f; }
static inline float jb_transient_process(JbTransientState *s,float input,
 float attack,float sustain) {
 /* attack/sustain in [-1,1]. Keep envelope followers updated even at bypass
    to avoid a discontinuity if the controls change during playback. */
 const float x=input<0.f?-input:input;
 s->fast+=.24f*(x-s->fast);
 s->slow+=.006f*(x-s->slow);
 if(attack==0.f && sustain==0.f) return input;
 const float transient=s->fast>s->slow?s->fast-s->slow:0.f;
 const float ratio=transient/(s->fast+.0001f);
 float gain=1.f+attack*ratio*.65f+sustain*(1.f-ratio)*.45f;
 if(gain<.2f)gain=.2f;
 if(gain>1.75f)gain=1.75f;
 return input*gain;
}
#endif
