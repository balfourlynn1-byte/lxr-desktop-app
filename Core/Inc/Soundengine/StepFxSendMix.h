/* SPDX-License-Identifier: GPL-3.0-only
 * JB-19B: add an optional, gated copy of a rendered mono voice to the
 * existing interleaved stereo FX accumulator. The dry voice is untouched.
 */
#ifndef JIMBONES_STEP_FX_SEND_MIX_H
#define JIMBONES_STEP_FX_SEND_MIX_H

#include <stdint.h>

/* Invoke once per rendered voice, per audio block, AFTER rendering/decimation
 * and BEFORE the regular output-routing call. Does not trigger voices or
 * create/clear FX delay buffers.
 *
 * already_routed_to_fx: legacy MIX OUT=FX; suppress a second copy.
 * fx_selected: zero for global FX OFF; suppress an unintended dry duplicate.
 * pan_left/right: existing voice's stereo pan gains in [0, 1].
 * fx_stereo: existing [L,R,L,R,...] saturating FX input accumulator.
 *
 * On complete bypass this returns before looping over any samples.
 */
void stepfx_send_mix_voice(uint8_t voice,
                           uint8_t already_routed_to_fx,
                           uint8_t fx_selected,
                           const int16_t *rendered_mono,
                           uint32_t frames,
                           float pan_left, float pan_right,
                           int16_t *fx_stereo);

#endif
