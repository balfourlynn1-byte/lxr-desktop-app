/*
 * Click-reduction on retrigger for the LXR's three primary drum voices.
 * Copyright (C) 2026 LXR-02 Retrigger Lab contributors
 * SPDX-License-Identifier: GPL-3.0-only
 * Modified firmware, not an official Erica Synths release.
 */
#include "Soundengine/DSPAudio/RetriggerSmoother.h"

void retrigger_smoother_init(RetriggerSmoother *s)
{
    s->correction = 0.0f;
    s->decrement = 0.0f;
    s->last_sample = 0;
    s->remaining = 0;
    s->pending = 0;
}

void retrigger_smoother_note_on(RetriggerSmoother *s, uint8_t overlap)
{
    /* On a fresh note do not soften the original attack. */
    if (overlap) {
        s->pending = 1;
    }
}

static int16_t clamp_sample(float sample)
{
    if (sample > 32767.0f) return 32767;
    if (sample < -32768.0f) return -32768;
    return (int16_t)sample;
}

void retrigger_smoother_process(RetriggerSmoother *s, int16_t *audio, uint8_t count)
{
    if (!count) return;

    if (s->pending) {
        s->pending = 0;
        /* Sample zero of the new hit equals the last output of the old hit.
         * The offset then decays to zero. The NEW oscillator and envelope
         * are still triggered immediately (no latency / no delayed hit). */
        s->correction = (float)((int32_t)s->last_sample - (int32_t)audio[0]);
        s->decrement = s->correction / (float)(LXR_RETRIGGER_FADE_SAMPLES - 1u);
        s->remaining = LXR_RETRIGGER_FADE_SAMPLES;
    }

    if (s->remaining) {
        for (uint8_t i = 0; i < count; ++i) {
            if (!s->remaining) break;
            audio[i] = clamp_sample((float)audio[i] + s->correction);
            --s->remaining;
            if (s->remaining) {
                s->correction -= s->decrement;
            } else {
                s->correction = 0.0f;
            }
        }
    }
    s->last_sample = audio[count - 1u];
}
