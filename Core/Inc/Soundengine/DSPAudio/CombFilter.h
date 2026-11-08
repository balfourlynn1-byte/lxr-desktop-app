/* SPDX-License-Identifier: GPL-3.0-only
 * JB-12: bounded, per-voice, fixed-point comb resonance for the original
 * LXR-02 FILTER page. No new kit parameters or FX bus allocations.
 */
#ifndef JIMBONES_COMB_FILTER_H
#define JIMBONES_COMB_FILTER_H

#include <stdint.h>

#define JB_COMB_TAPS 256u
#define JB_COMB_MASK (JB_COMB_TAPS - 1u)
#define JB_COMB_MIN_DELAY 16u
#define JB_COMB_MAX_DELAY 255u
#define JB_COMB_MAX_FEEDBACK_Q8 220u /* 86% feedback limit */

typedef struct {
    int16_t history[JB_COMB_TAPS];
    uint16_t write;
    uint16_t delay;
} JbCombFilter;

/* Init/reset are safe to call only from voice startup or reset context.
 * Processing is audio-thread-only and entirely bounded, with no heap,
 * dynamic RAM or second global FX delay.
 */
void jb_comb_init(JbCombFilter *comb);
void jb_comb_reset(JbCombFilter *comb);

/* The existing filter's FREQ and RESO determine pitch/feedback:
 * normalizedFreq is SVF filter->f, range 0..0.45 (lower -> longer delay);
 * normalizedReso is 1-filter->q, range 0..1.
 * Reso=0 yields bit-identical dry audio; output always saturates to int16.
 */
void jb_comb_process(JbCombFilter *comb, int16_t *samples, uint8_t frames,
                     float normalizedFreq, float normalizedReso);
#endif
