/* SPDX-License-Identifier: GPL-3.0-only
 * Original, GPL-compatible native wavetable morph planner.
 */
#include "Soundengine/DSPAudio/JimbonesNativeWave.h"
#include <stdint.h>

uint32_t jb_native_wave_pack(float normalized,uint8_t interpolation_on) {
    if(!interpolation_on || !(normalized>=0.f))return 0u; /* incl NaN */
    if(normalized>1.f)normalized=1.f;
    const float pos=normalized*6.f; /* native waveform enum has six values */
    if(!(pos<3.f))return 0u; /* no costly interpolation into NOISE/PWM */
    const uint8_t base=(uint8_t)pos;
    const float part=pos-(float)base;
    const uint32_t q=(uint32_t)(part*32767.f+.5f);
    if(!q || q>=32767u) return 0u;
    return JB_NATIVE_WAVE_MORPH_ON
         | ((uint32_t)base<<JB_NATIVE_WAVE_BASE_SHIFT)
         | ((uint32_t)(base+1u)<<JB_NATIVE_WAVE_NEXT_SHIFT)
         | q;
}

int16_t jb_native_wave_blend(int16_t a,int16_t b,uint32_t state) {
    if(!(state&JB_NATIVE_WAVE_MORPH_ON))return a;
    const int32_t mix=(int32_t)(state&JB_NATIVE_WAVE_Q15_MASK);
    int32_t out=(int32_t)a+(((int32_t)b-(int32_t)a)*mix>>15);
    if(out>32767)out=32767;
    if(out< -32768)out=-32768;
    return (int16_t)out;
}
