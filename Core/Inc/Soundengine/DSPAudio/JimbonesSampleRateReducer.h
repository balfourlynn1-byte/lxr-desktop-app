/* JB-18: per-voice sample hold prototype; defaults to transparent bypass. */
#ifndef JIMBONES_SAMPLE_RATE_REDUCER_H
#define JIMBONES_SAMPLE_RATE_REDUCER_H
#include <stdint.h>
typedef struct { float held; uint16_t counter; } JbSampleHold;
static inline void jb_sample_hold_reset(JbSampleHold *s){s->held=0.f;s->counter=0;}
static inline float jb_sample_hold_process(JbSampleHold *s,float input,uint16_t factor){
 if(factor<=1u){s->held=input;s->counter=0;return input;}
 if(s->counter==0){s->held=input;s->counter=factor;}
 --s->counter;
 return s->held;
}
#endif
