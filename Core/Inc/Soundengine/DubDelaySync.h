/* SPDX-License-Identifier: GPL-3.0-only
 * JB-20: tempo-divided timing for the one existing global FX delay.
 * No additional DSP delay line or allocated audio buffer.
 */
#ifndef JIMBONES_DUB_DELAY_SYNC_H
#define JIMBONES_DUB_DELAY_SYNC_H
#include <stdint.h>
struct DelayStruct;

enum DubDelayStatus {
    DUB_DELAY_OFF=0,
    DUB_DELAY_READY,
    DUB_DELAY_NEEDS_FX,
    DUB_DELAY_NO_CLOCK,
    DUB_DELAY_TOO_LONG,
    DUB_DELAY_TOO_SHORT
};

void dub_delay_init(void);
uint8_t dub_delay_division(void); /* 0..4 */
void dub_delay_adjust_division(int delta);
const char *dub_delay_division_name(void);
const char *dub_delay_status_name(void);
uint8_t dub_delay_status(void);
uint8_t dub_delay_can_send(void);
uint8_t dub_delay_wet_only(void);
/* Explicitly marks manual FX parameter changes; re-assert sync next block. */
void dub_delay_mark_dirty(void);

/* Bounded timing calculation, also used by host regression. Returns status;
 * normalized delay parameter is only written for DUB_DELAY_READY.
 * mode: 0 = mono, 1 = ping-pong. */
uint8_t dub_delay_plan(uint16_t bpm, uint8_t mode, uint8_t division,
                       uint32_t *samples, float *normalized);

/* Called ONCE per audio block, before the six per-voice FX sends.
 * FX parameters remain kit-owned. Returning to normal eventually restores
 * them without clearing the existing feedback ring. Clock freshness is
 * checked by the mixer for external MIDI/analog clocks. */
void dub_delay_prepare(struct DelayStruct *delay, uint8_t fx_is_delay,
                       uint16_t bpm, uint8_t external_clock,
                       uint8_t clock_fresh,
                       float manual_time, float manual_range,
                       float manual_feedback, uint32_t frames);
#endif
