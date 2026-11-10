#ifndef JB_HOST_DSP_INTRINSICS_H
#define JB_HOST_DSP_INTRINSICS_H

/* Host syntax/render probes only. The ARM firmware uses CMSIS intrinsics. */
#if defined(__arm__) || defined(__thumb__) || defined(__aarch64__)
#error "host DSP compatibility must not enter an ARM build"
#endif

#include <stdint.h>

static inline uint32_t jb_host_saturate_add16(uint32_t a, uint32_t b)
{
    uint32_t out = 0;
    for (unsigned shift = 0; shift <= 16; shift += 16) {
        int32_t x = (int16_t)(a >> shift);
        int32_t y = (int16_t)(b >> shift);
        int32_t sum = x + y;
        if (sum > INT16_MAX) sum = INT16_MAX;
        if (sum < INT16_MIN) sum = INT16_MIN;
        out |= (uint32_t)(uint16_t)sum << shift;
    }
    return out;
}

static inline uint32_t jb_host_saturate_sub16(uint32_t a, uint32_t b)
{
    uint32_t out = 0;
    for (unsigned shift = 0; shift <= 16; shift += 16) {
        int32_t x = (int16_t)(a >> shift);
        int32_t y = (int16_t)(b >> shift);
        int32_t difference = x - y;
        if (difference > INT16_MAX) difference = INT16_MAX;
        if (difference < INT16_MIN) difference = INT16_MIN;
        out |= (uint32_t)(uint16_t)difference << shift;
    }
    return out;
}

#define __QADD16(a, b) jb_host_saturate_add16((uint32_t)(a), (uint32_t)(b))
#define __QSUB16(a, b) jb_host_saturate_sub16((uint32_t)(a), (uint32_t)(b))

#endif
