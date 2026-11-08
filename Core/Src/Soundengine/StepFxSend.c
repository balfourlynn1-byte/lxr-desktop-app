/* SPDX-License-Identifier: GPL-3.0-only
 * JB-19A/B: Step FX send gates feeding an optional, separate mixer tap.
 *
 * Per-voice aligned atomic command mailbox is written by a single control
 * producer; only the audio thread owns the render state. Retrigger extends
 * the hold without resetting the envelope (therefore no gain discontinuity).
 *
 * There is no timing dependency on UI paint or on the Step FX note scheduler.
 */
#include "Soundengine/StepFxSend.h"
#include <stdint.h>

#define SEND_ATTACK_STEP 373u  /* ~2 ms from zero at 44.14 kHz */
#define SEND_RELEASE_STEP 187u /* ~4 ms from full scale */
#define SEND_LEVEL_STEP 256u   /* ~3 ms level modulation smoothing */
#define SEND_DEFAULT_MASK 0x3eu /* all except voice zero (kick) */

typedef struct {
    uint32_t request_frames;
    uint32_t request_seq;
    uint32_t seen_seq;
    uint32_t held_frames;
    uint16_t env;
    uint16_t smoothed_level;
    uint16_t target_level;
    uint8_t block_enabled;
} SendLane;

/* 32-bit access remains atomic on the target Cortex-M7. GCC __atomic_* is
 * used for inter-context visibility; no mutex or interrupt lock is needed. */
static SendLane lanes[STEPFX_SEND_VOICES];
static uint8_t send_enabled;
static uint8_t send_mask;
static uint8_t send_levels[STEPFX_SEND_VOICES];
static uint8_t send_division = 16u; /* 1/16-note snippet by default */

void stepfx_send_init(void) {
    __atomic_store_n(&send_enabled, 0u, __ATOMIC_RELAXED);
    __atomic_store_n(&send_mask, SEND_DEFAULT_MASK, __ATOMIC_RELAXED);
    __atomic_store_n(&send_division, 16u, __ATOMIC_RELAXED);
    for (unsigned v = 0; v < STEPFX_SEND_VOICES; ++v) {
        SendLane *s = &lanes[v];
        s->request_frames = 0;
        s->request_seq = 0;
        s->seen_seq = 0;
        s->held_frames = 0;
        s->env = 0;
        s->smoothed_level = 0;
        s->target_level = 0;
        s->block_enabled = 0;
        __atomic_store_n(&send_levels[v], 127u, __ATOMIC_RELAXED);
    }
}

void stepfx_send_set_enabled(uint8_t enabled) {
    __atomic_store_n(&send_enabled, (uint8_t)(enabled != 0), __ATOMIC_RELAXED);
}

void stepfx_send_set_mask(uint8_t mask) {
    __atomic_store_n(&send_mask, (uint8_t)(mask & 0x3fu), __ATOMIC_RELAXED);
}

void stepfx_send_set_level(uint8_t voice, uint8_t level) {
    if (voice >= STEPFX_SEND_VOICES) return;
    if (level > 127u) level = 127u;
    __atomic_store_n(&send_levels[voice], level, __ATOMIC_RELAXED);
}

uint8_t stepfx_send_mask(void) {
    return __atomic_load_n(&send_mask, __ATOMIC_RELAXED);
}
uint8_t stepfx_send_level(uint8_t voice) {
    if (voice >= STEPFX_SEND_VOICES) return 0u;
    return __atomic_load_n(&send_levels[voice], __ATOMIC_RELAXED);
}
uint8_t stepfx_send_division(void) {
    return __atomic_load_n(&send_division, __ATOMIC_RELAXED);
}
void stepfx_send_adjust_division(int delta) {
    if (!delta) return;
    const uint8_t current = stepfx_send_division();
    uint8_t d = current;
    /* Faster (right turn) = shorter snippets: 1/4 -> 1/8 -> 1/16 -> 1/32.
     * Input is an encoder delta but each call changes one subdivision only. */
    if (delta > 0 && d < 32u) d = (uint8_t)(d * 2u);
    if (delta < 0 && d > 4u) d = (uint8_t)(d / 2u);
    __atomic_store_n(&send_division, d, __ATOMIC_RELAXED);
}

void stepfx_send_on_note(uint8_t voice, uint8_t velocity, uint16_t bpm) {
    if (!velocity || voice > 6u ||
        !__atomic_load_n(&send_enabled, __ATOMIC_RELAXED)) return;
    const uint8_t lane = voice > 5u ? 5u : voice;
    if (!(__atomic_load_n(&send_mask, __ATOMIC_RELAXED) & (1u << lane)))
        return;
    if (!__atomic_load_n(&send_levels[lane], __ATOMIC_RELAXED)) return;
    /* If external BPM is not yet available, default to 120 for the
     * *snippet gate only*. Beat-synced feedback is Job JB-20. */
    if (bpm < 10u) bpm = 120u;
    if (bpm > 400u) bpm = 400u;
    const uint8_t division = stepfx_send_division();
    uint32_t frames = (44140u * 60u * 4u) / ((uint32_t)bpm * division);
    if (frames > STEPFX_SEND_MAX_FRAMES) frames = STEPFX_SEND_MAX_FRAMES;
    stepfx_send_gate(lane, frames);
}

void stepfx_send_gate(uint8_t voice, uint32_t hold_frames) {
    if (voice >= STEPFX_SEND_VOICES) return;
    if (hold_frames > STEPFX_SEND_MAX_FRAMES)
        hold_frames = STEPFX_SEND_MAX_FRAMES;
    SendLane *s = &lanes[voice];
    /* The sequence is a release marker: the audio thread reads frames
     * only after observing a new sequence. One producer per voice. */
    __atomic_store_n(&s->request_frames, hold_frames, __ATOMIC_RELAXED);
    (void)__atomic_add_fetch(&s->request_seq, 1u, __ATOMIC_RELEASE);
}

void stepfx_send_begin_block(uint8_t voice) {
    if (voice >= STEPFX_SEND_VOICES) return;
    SendLane *s = &lanes[voice];
    const uint32_t seq = __atomic_load_n(&s->request_seq, __ATOMIC_ACQUIRE);
    if (seq != s->seen_seq) {
        const uint32_t frames =
            __atomic_load_n(&s->request_frames, __ATOMIC_RELAXED);
        s->seen_seq = seq;
        s->held_frames = frames;
    }

    const uint8_t mask = __atomic_load_n(&send_mask, __ATOMIC_RELAXED);
    const uint8_t enabled = __atomic_load_n(&send_enabled, __ATOMIC_RELAXED);
    const uint8_t level =
        __atomic_load_n(&send_levels[voice], __ATOMIC_RELAXED);
    s->block_enabled = (uint8_t)(enabled && (mask & (1u << voice)));
    if (!s->block_enabled) {
        s->held_frames = 0;
        s->env = 0;
        s->smoothed_level = 0;
    }
    s->target_level = (uint16_t)(((uint32_t)level * STEPFX_SEND_Q15_MAX) / 127u);
}

uint8_t stepfx_send_active(uint8_t voice) {
    if (voice >= STEPFX_SEND_VOICES) return 0u;
    const SendLane *s = &lanes[voice];
    /* Keep stepping while a zero-level gate is held so a later level
     * adjustment cannot resurrect an expired send window. */
    return (uint8_t)(s->block_enabled && (s->held_frames || s->env));
}

uint16_t stepfx_send_next_gain_q15(uint8_t voice) {
    if (voice >= STEPFX_SEND_VOICES) return 0;
    SendLane *s = &lanes[voice];

    /* Gate and level evolve on the audio timeline, not at UI poll speed. */
    if (s->block_enabled && s->held_frames) {
        --s->held_frames;
        const uint32_t raised = (uint32_t)s->env + SEND_ATTACK_STEP;
        s->env = (uint16_t)(raised > STEPFX_SEND_Q15_MAX
                                ? STEPFX_SEND_Q15_MAX : raised);
    } else {
        s->env = (uint16_t)(s->env > SEND_RELEASE_STEP
                                ? s->env - SEND_RELEASE_STEP : 0u);
    }

    if (s->smoothed_level < s->target_level) {
        const uint32_t raised = (uint32_t)s->smoothed_level + SEND_LEVEL_STEP;
        s->smoothed_level = (uint16_t)(raised > s->target_level
                                           ? s->target_level : raised);
    } else if (s->smoothed_level > s->target_level) {
        s->smoothed_level = (uint16_t)(
            ((uint32_t)s->smoothed_level - s->target_level) > SEND_LEVEL_STEP
                ? s->smoothed_level - SEND_LEVEL_STEP
                : s->target_level);
    }

    /* Global OFF or excluded voice is an exact zero-cost audio bypass.
     * Send level zero also yields a strict zero regardless of ramp state. */
    if (!s->block_enabled || !s->target_level) return 0u;
    return (uint16_t)(((uint32_t)s->env * s->smoothed_level) >> 15);
}
