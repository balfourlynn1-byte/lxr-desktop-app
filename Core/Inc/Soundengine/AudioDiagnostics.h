/* JIMBONES audio diagnostics: rail samples, rendering cycles, reset.
 * This is intentionally a read-only monitor; it does not change DSP audio. */
#ifndef JIMBONES_AUDIO_DIAGNOSTICS_H
#define JIMBONES_AUDIO_DIAGNOSTICS_H
#include <stdint.h>
extern volatile uint32_t jb_audioMaxCycles, jb_audioLastCycles, jb_audioOverruns;
extern volatile uint8_t jb_audioLoadPercent, jb_audioPeakPercent;
extern volatile uint32_t jb_audioNearRailHits;
extern volatile uint32_t jb_layerSaturationHits;
extern volatile uint32_t jb_dmaPendingCount, jb_averageLoad;
extern volatile uint32_t jb_layerPeakCycles[6];
void jb_audioLoadSample(uint32_t load);
void jb_audioDiagScan(const int16_t *out1,const int16_t *out2,unsigned frames);
void jb_audioDiagReset(void);
/* Clear the private render-cycle telemetry while interrupts are masked. */
void jb_audioTelemetryResetCounters(void);
#endif
