/*
 * Optional click-reduction on repeated drum hits.
 * Copyright (C) 2026 LXR-02 Retrigger Lab contributors
 * SPDX-License-Identifier: GPL-3.0-only
 * Modified firmware, not an official Erica Synths release.
 */
#ifndef RETRIGGER_SMOOTHER_H
#define RETRIGGER_SMOOTHER_H

#include <stdint.h>

/* Approx. 3 ms at the LXR audio rate (REAL_FS ~= 44140 Hz).
 * No allocation, lookahead or additional latency. */
#define LXR_RETRIGGER_FADE_SAMPLES 132u

typedef struct RetriggerSmoother {
    float correction;
    float decrement;
    int16_t last_sample;
    uint16_t remaining;
    volatile uint8_t pending;
} RetriggerSmoother;

void retrigger_smoother_init(RetriggerSmoother *s);
/* Set pending only when the voice's preceding envelope is still active. */
void retrigger_smoother_note_on(RetriggerSmoother *s, uint8_t overlap);
/* Apply to the finished MONO voice buffer before decimation and routing. */
void retrigger_smoother_process(RetriggerSmoother *s, int16_t *audio, uint8_t count);

#endif
