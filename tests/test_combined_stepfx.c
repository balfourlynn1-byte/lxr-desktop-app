/* Host regression for merged JIMBONES trigger FX. Not physical DSP proof. */
#include "Soundengine/StepFX.h"
#include <stdint.h>
#include <assert.h>
#include <stdio.h>
static unsigned hits,changedPitch,hatHits;
static uint8_t onlySnare=1;
static void emit(uint8_t voice,uint8_t note,uint8_t velocity) {
    assert(voice<=6 && note<=127 && velocity>=1 && velocity<=127);
    if(onlySnare) assert(voice==3);
    hits++;
    if(note!=48) changedPitch++;
    if(voice>=5) hatHits++;
}
static void blocks(unsigned n) {
    for(unsigned i=0;i<n;i++) stepfx_process(64,120,0,emit);
}
int main(void) {
    stepfx_init();
    assert(stepfx_mask()==((1u<<3)|(1u<<5)));
    stepfx_live(3,48,110);blocks(500);assert(hits==0);
    stepfx_select(1);stepfx_live(3,48,110);blocks(800);
    assert(hits==3 && changedPitch==0);
    stepfx_select(15);stepfx_live(3,48,127);
    hits=changedPitch=0;blocks(12000);
    assert(hits>1 && hits<512 && changedPitch>0);
    stepfx_select(0);unsigned prev=hits;blocks(1200);assert(hits==prev);
    stepfx_select(4);onlySnare=0;stepfx_live(6,48,127);
    assert(stepfx_pending()==1);stepfx_stop();assert(!stepfx_pending());
    puts("PASS: bypass, snare/hat default mask, pitched multi-bar phrases, bounded events and stop");
    return 0;
}
