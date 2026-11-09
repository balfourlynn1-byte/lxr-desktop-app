/*
 * JIMBONES C03: Erica-native, opt-in periodic wavetable scan prototype.
 * Independently authored; no non-commercial upstream source included.
 * Host-testable without changing OscInfo, native waveform IDs or saved kits.
 */
#ifndef JB_NATIVE_WAVE_SCAN_H
#define JB_NATIVE_WAVE_SCAN_H
#include <stdint.h>
#include <stddef.h>
/* The caller supplies four single-cycle tables of 256 signed samples each.
 * The position is bounded [0, 3]; phase wraps through the top 8 bits.
 * This function does not advance oscillator phase or alter legacy state.
 */
static inline int16_t jb_wave_scan_sample(const int16_t waves[4][256],
                                          uint32_t phase, float position)
{
    if (!(position >= 0.f)) position = 0.f; /* includes NaN */
    if (position > 3.f) position = 3.f;
    const unsigned a = (unsigned)position;
    const unsigned b = a < 3u ? a + 1u : a;
    const float mix = position - (float)a;
    const unsigned i = phase >> 24;
    const unsigned next = (i + 1u) & 255u;
    const float fraction = (float)(phase & 0xFFFFFFu) * (1.f / 16777216.f);
    const float va = (float)waves[a][i] +
                     ((float)waves[a][next] - (float)waves[a][i]) * fraction;
    const float vb = (float)waves[b][i] +
                     ((float)waves[b][next] - (float)waves[b][i]) * fraction;
    float v = va + (vb - va) * mix;
    if (v > 32767.f) v = 32767.f;
    if (v < -32768.f) v = -32768.f;
    return (int16_t)v;
}
#endif
