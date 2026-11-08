/* SPDX-License-Identifier: GPL-3.0-only
 * JB-12: bounded, low-cost recursive comb filter, completely independent
 * of the shared FxBus delay. Six filter objects own separate history.
 *
 * Difference equation (positive-feedback, normalised for unity DC gain):
 *   y[n] = (1-a)*x[n] + a*y[n-delay], 0 <= a <= 220/256.
 *
 * The acoustic effect is a comb of attenuated spectral notches, rather
 * than a second reverb/delay. Q8 coefficients avoid per-sample float ops.
 */
#include "Soundengine/DSPAudio/CombFilter.h"
#include <limits.h>
#include <stddef.h>

void jb_comb_reset(JbCombFilter *c)
{
    if(!c) return;
    for(uint32_t i=0; i<JB_COMB_TAPS; ++i) c->history[i]=0;
    c->write=0u;
    c->delay=0u;
    c->active=0u;
}
void jb_comb_init(JbCombFilter *c) { jb_comb_reset(c); }

static uint16_t comb_delay(float f)
{
    if(!(f>=0.f))f=0.f;
    if(f>0.45f)f=0.45f;
    const float fraction=(0.45f-f)*(1.f/0.45f);
    const uint32_t delay=JB_COMB_MIN_DELAY+
        (uint32_t)(fraction*(JB_COMB_MAX_DELAY-JB_COMB_MIN_DELAY)+0.5f);
    return (uint16_t)(delay>JB_COMB_MAX_DELAY?JB_COMB_MAX_DELAY:delay);
}
static uint16_t comb_feedback(float r)
{
    if(!(r>0.f))return 0u;
    if(r>=1.f)return JB_COMB_MAX_FEEDBACK_Q8;
    return (uint16_t)(r*JB_COMB_MAX_FEEDBACK_Q8+0.5f);
}

void jb_comb_process(JbCombFilter *c, int16_t *samples, uint8_t frames,
                     float normalizedFreq, float normalizedReso)
{
    if(!c || !samples || !frames) return;
    const uint16_t target=comb_delay(normalizedFreq);
    const uint16_t feedback=comb_feedback(normalizedReso);
    if(!feedback) { /* exactly bit-identical bypass at zero resonance */
        c->active=0u; /* next activation flushes stale echoes once */
        c->delay=target;
        return;
    }
    if(!c->active) {
        for(uint32_t k=0;k<JB_COMB_TAPS;k++) c->history[k]=0;
        c->write=0u;
        c->delay=target;
        c->active=1u;
    }
    /* Discrete delay selection is smoothed at one sample/block. This
     * limits sudden flams during knob moves without a float per-sample
     * interpolation. First activation starts at requested pitch. */
    uint16_t d=c->delay;
    if(!d)d=target;
    else if(d<target)d++;
    else if(d>target)d--;
    c->delay=d;

    uint16_t pos=c->write;
    for(uint32_t i=0;i<frames;i++) {
        const uint16_t read=(uint16_t)((pos-d)&JB_COMB_MASK);
        const int32_t dry=samples[i];
        const int32_t wet=c->history[read];
        /* Q8 normalisation bounds output within signed 16-bit when
         * input and history are both signed 16-bit; 32-bit math is safe. */
        const int32_t value=(dry*(256-(int32_t)feedback)+wet*(int32_t)feedback)/256;
        const int16_t sample=(int16_t)(value>INT16_MAX?INT16_MAX:
                                    (value<INT16_MIN?INT16_MIN:value));
        c->history[pos]=sample;
        samples[i]=sample;
        pos=(uint16_t)((pos+1u)&JB_COMB_MASK);
    }
    c->write=pos;
}
