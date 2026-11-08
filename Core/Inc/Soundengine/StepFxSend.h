/* SPDX-License-Identifier: GPL-3.0-only
 * JB-19A: optional per-voice gate for a future parallel FX send.
 *
 * Control producers write atomic requests. Only the audio renderer calls
 * begin_block/next_gain, so envelope state has one writer. No ISR locks,
 * memory allocation, voice retriggers, or FX buffer changes.
 */
#ifndef JIMBONES_STEP_FX_SEND_H
#define JIMBONES_STEP_FX_SEND_H

#include <stdint.h>

#define STEPFX_SEND_VOICES 6u
#define STEPFX_SEND_Q15_MAX 32767u
#define STEPFX_SEND_MAX_FRAMES 88280u /* 2 seconds at nominal 44.14 kHz */

/* Startup only; must not race the audio callback. Defaults fully bypassed. */
void stepfx_send_init(void);

/* Control/UI/MIDI context: one producer per voice, atomic 32-bit request. */
void stepfx_send_set_enabled(uint8_t enabled);
void stepfx_send_set_mask(uint8_t voice_mask);
void stepfx_send_set_level(uint8_t voice, uint8_t level); /* 0..127 */
void stepfx_send_gate(uint8_t voice, uint32_t hold_frames); /* 0 = release */

/* Audio context: call begin_block once per voice and next_gain once per sample.
 * The returned Q15 gain applies to an ADDITIVE COPY, never to the dry voice.
 * Gates do not touch a delay buffer or stop existing effect feedback tails.
 */
void stepfx_send_begin_block(uint8_t voice);
uint16_t stepfx_send_next_gain_q15(uint8_t voice);

#endif
