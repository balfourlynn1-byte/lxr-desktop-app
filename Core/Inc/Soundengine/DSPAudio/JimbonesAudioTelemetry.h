/* JIMBONES audio deadline telemetry: independently implemented from
 * LXR02Open's documented DWT/queue-pressure concept.
 * SPDX-License-Identifier: GPL-3.0-only */
#ifndef JIMBONES_AUDIO_TELEMETRY_H
#define JIMBONES_AUDIO_TELEMETRY_H
#include <stdint.h>
typedef struct {
 uint32_t budget_cycles;
 uint32_t last_cycles;
 uint32_t peak_cycles;
 uint32_t over_budget;
 uint32_t blocks;
 uint32_t pressure_sum;
 uint32_t pressure_samples;
 uint8_t peak_pressure_percent;
} jb_audio_telemetry;
static inline void jb_audio_telemetry_init(jb_audio_telemetry *m,
 uint32_t clock_hz, uint32_t frames, uint32_t sample_rate) {
 *m=(jb_audio_telemetry){0};
 if(sample_rate) m->budget_cycles=(uint32_t)(((uint64_t)clock_hz*frames)/sample_rate);
}
static inline void jb_audio_telemetry_record(jb_audio_telemetry *m,
 uint32_t start_cycles,uint32_t end_cycles) {
 uint32_t delta=end_cycles-start_cycles; /* modulo 2^32 handles wrap */
 m->last_cycles=delta;
 if(delta>m->peak_cycles) m->peak_cycles=delta;
 if(m->budget_cycles && delta>m->budget_cycles) ++m->over_budget;
 ++m->blocks;
}
static inline uint8_t jb_audio_telemetry_load(const jb_audio_telemetry *m) {
 if(!m->budget_cycles) return 0;
 uint64_t pct=(uint64_t)m->last_cycles*100u/m->budget_cycles;
 return (uint8_t)(pct>100?100:pct);
}
/* queue_pressure=100 means no free render slot; 0 means all slots free.
 * This is not a general CPU utilization metric. */
static inline void jb_audio_telemetry_queue(jb_audio_telemetry *m,
 uint32_t free_slots,uint32_t capacity) {
 if(!capacity) return;
 if(free_slots>capacity) free_slots=capacity;
 uint8_t p=(uint8_t)(100u-(free_slots*100u/capacity));
 if(p>m->peak_pressure_percent) m->peak_pressure_percent=p;
 m->pressure_sum+=p;
 ++m->pressure_samples;
}
#endif
