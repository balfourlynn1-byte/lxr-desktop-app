/* SPDX-License-Identifier: GPL-3.0-only
 * JB-19B: parallel audio send from existing rendered voices.
 * No additional synth voice, DSP effects processor, or delay memory.
 */
#include "Soundengine/StepFxSendMix.h"
#include "Soundengine/StepFxSend.h"
#include <stdint.h>
#include <limits.h>

static uint16_t q15_pan(float value)
{
    /* Also handles NaN by treating it as zero. */
    if (!(value > 0.0f)) return 0u;
    if (value >= 1.0f) return STEPFX_SEND_Q15_MAX;
    return (uint16_t)(value * (float)STEPFX_SEND_Q15_MAX + 0.5f);
}

static int16_t sat_add16(int16_t old, int32_t incoming)
{
    const int32_t sum = (int32_t)old + incoming;
    if (sum > INT16_MAX) return INT16_MAX;
    if (sum < INT16_MIN) return INT16_MIN;
    return (int16_t)sum;
}

void stepfx_send_mix_voice(uint8_t voice,
                           uint8_t already_routed_to_fx,
                           uint8_t fx_selected,
                           const int16_t *rendered_mono,
                           uint32_t frames,
                           float pan_left, float pan_right,
                           int16_t *fx_stereo)
{
    if (voice >= STEPFX_SEND_VOICES) return;
    stepfx_send_begin_block(voice);
    if (!stepfx_send_active(voice) || !rendered_mono || !fx_stereo) return;

    /* Even when no new send is allowed, consume the active gate timeline so
     * its hold/release state cannot be replayed on a later routing change. */
    const uint8_t copy_allowed = (uint8_t)(!already_routed_to_fx && fx_selected);
    const uint16_t pan_l = copy_allowed ? q15_pan(pan_left) : 0u;
    const uint16_t pan_r = copy_allowed ? q15_pan(pan_right) : 0u;

    for (uint32_t i = 0; i < frames; ++i) {
        const uint16_t gain = stepfx_send_next_gain_q15(voice);
        if (!copy_allowed || !gain) continue;

        /* Each multiply fits a signed 32-bit intermediate; divide by 32768
         * before the next multiply. Saturate the existing shared FX bus. */
        const int32_t wet = ((int32_t)rendered_mono[i] * gain) / 32768;
        const int32_t left = (wet * pan_l) / 32768;
        const int32_t right = (wet * pan_r) / 32768;
        fx_stereo[i * 2u] = sat_add16(fx_stereo[i * 2u], left);
        fx_stereo[i * 2u + 1u] = sat_add16(fx_stereo[i * 2u + 1u], right);
    }
}
