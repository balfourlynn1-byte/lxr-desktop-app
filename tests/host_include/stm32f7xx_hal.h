/* Host-only HAL shim for isolated JIMBONES DSP tests.
 * NEVER include this directory in ARM firmware builds. */
#ifndef JIMBONES_HOST_HAL_SHIM_H
#define JIMBONES_HOST_HAL_SHIM_H
#include <stdint.h>
#define I2S_AUDIOFREQ_44K 44100u
static inline uint32_t __get_PRIMASK(void) { return 0u; }
static inline void __disable_irq(void) {}
static inline void __DMB(void) {}
static inline void __set_PRIMASK(uint32_t mask) { (void)mask; }
#endif
