/* JIMBONES C10: built-in immutable waveform bank.
 * The four waveforms are constructed from fixed integer samples on the host
 * and firmware alike; no heap allocation or borrowed transient pointers.
 * Wave 0: bipolar triangle; 1: saw; 2: square; 3: stepped pulse.
 */
#ifndef JB_NATIVE_WAVE_BANK_H
#define JB_NATIVE_WAVE_BANK_H
#include <stdint.h>
extern const int16_t jb_builtinWaveBank[4][256];
#endif
