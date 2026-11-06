#include "Soundengine/AudioDiagnostics.h"
#include <stdint.h>
#ifndef LXR_EMU
#include "stm32f7xx_hal.h"
#endif
volatile uint32_t jb_audioNearRailHits;
/* Called after FX mixing, in the same audio render budget.
 * 'RAIL' measures near-full-scale samples, not definite clipping.
 * Only integer comparisons on two stereo output pairs, no allocations. */
void jb_audioDiagScan(const int16_t *out1,const int16_t *out2,unsigned frames) {
    if(!out1 || !out2) return;
    uint32_t hits=0;
    for(unsigned i=0;i<frames*2u;i++) {
        const int x=out1[i], y=out2[i];
        hits+=(x>=32760 || x<=-32760);
        hits+=(y>=32760 || y<=-32760);
    }
    if(hits) {
        const uint32_t prior=jb_audioNearRailHits;
        jb_audioNearRailHits=UINT32_MAX-prior<hits?UINT32_MAX:prior+hits;
    }
}
/* Clear peaks/incidents while allowing current DSP load to keep updating. */
void jb_audioDiagReset(void) {
#ifndef LXR_EMU
    const uint32_t saved=__get_PRIMASK();
    __disable_irq();
#endif
    jb_audioMaxCycles=0;
    jb_audioPeakPercent=0;
    jb_audioOverruns=0;
    jb_audioNearRailHits=0;
    jb_layerSaturationHits=0;
#ifndef LXR_EMU
    __DMB();
    __set_PRIMASK(saved);
#endif
}
