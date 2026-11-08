/* SPDX-License-Identifier: GPL-3.0-only
 * JB-27: compact independent native oscillator waveform-scan state.
 *
 * Inspired by LXR02Open's user-facing idea of smooth LFO-to-waveform
 * selection, but contains no copied upstream source. No new wavetable ROM,
 * FM layer, or sample-loading engine.
 */
#ifndef JIMBONES_NATIVE_WAVE_H
#define JIMBONES_NATIVE_WAVE_H
#include <stdint.h>
#define JB_NATIVE_WAVE_MORPH_ON 0x80000000u
#define JB_NATIVE_WAVE_BASE_SHIFT 16u
#define JB_NATIVE_WAVE_NEXT_SHIFT 18u
#define JB_NATIVE_WAVE_Q15_MASK 0x7FFFu

/* normalized LFO-modulated waveform position (0..1, six native waveform
 * choices) maps smoothly across SINE->TRI->SAW->REC. Noise and PWM remain
 * original discrete engines, avoiding doubled expensive per-hit rendering.
 * Disabled, nonperiodic, NaN, and exact waveform endpoints return 0,
 * retaining the native untouched render path. */
uint32_t jb_native_wave_pack(float normalized, uint8_t interpolation_on);

/* Deterministic 16-bit signed linear crossfade; input/output bounded.
 * This function is for pure host regressions. Real sample mix inlines the
 * same one multiply in Oscillator.c to avoid a function call per sample. */
int16_t jb_native_wave_blend(int16_t a,int16_t b,uint32_t state);
#endif
