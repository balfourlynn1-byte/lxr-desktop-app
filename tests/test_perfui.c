/* JB-21 host regression: sixteen physical buttons toggle and dub send
 * envelopes remain separate from legacy trigger Step FX.
 * Tests real production sources without STM32 hardware.
 */
#include "menu/JimbonesPerfUI.h"
#include "Soundengine/StepFX.h"
#include "Soundengine/StepFxSend.h"
#include "Soundengine/StepFxSendMix.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    stepfx_init();
    stepfx_send_init();
    jb_perfui_enter();
    assert(jb_perfui_page()==JB_PERF_PAGE_MAIN);
    assert(jb_perfui_active()==255u);
    assert(jb_perfui_next()==JB_PERF_PAGE_MORE);
    assert(jb_perfui_next()==JB_PERF_PAGE_STEPFX);
    assert(jb_perfui_stepfx_active());

    /* Every step including Step 1 has ON on first press, OFF on second.
     * Only one effect is active at a time: preserve the old DSP budget. */
    for(unsigned i=0;i<16u;i++) {
        assert(jb_perfui_select((uint8_t)i));
        assert(jb_perfui_selected()==i);
        assert(jb_perfui_active()==i);
        assert(strlen(jb_perfui_effect_name((uint8_t)i))<=11u);
        if(i==0u)assert(strcmp(jb_perfui_effect_name(0),"DUBSEND")==0);
        if(i)assert(stepfx_slot()==i);
        else assert(stepfx_slot()==0u);
        assert(jb_perfui_select((uint8_t)i));
        assert(jb_perfui_active()==255u);
        assert(stepfx_slot()==0u);
    }

    /* Switching directly from one active effect arms new and deactivates old.
     * Encoder browsing changes focus, never an armed effect. */
    assert(jb_perfui_select(6u));
    assert(jb_perfui_active()==6u);
    assert(jb_perfui_focus(0u));
    assert(jb_perfui_selected()==0u);
    assert(jb_perfui_active()==6u);
    assert(jb_perfui_select(0u));
    assert(jb_perfui_active()==0u);
    assert(stepfx_slot()==0u);
    assert(!jb_perfui_select(16u));
    assert(!jb_perfui_focus(16u));
    assert(jb_perfui_active()==0u);

    /* Step 1 enables parallel audio send; only selected dry hits start
     * a short timed copy. Snare V4 is armed; kick V1 is protected. */
    assert(stepfx_send_mask()==0x3eu);
    assert(stepfx_send_level(3)==64u);
    assert(stepfx_send_division()==16u);
    stepfx_send_on_note(3,115u,120u);
    stepfx_send_on_note(0,115u,120u);
    stepfx_send_begin_block(3u);
    stepfx_send_begin_block(0u);
    assert(stepfx_send_next_gain_q15(3u)>0u);
    assert(stepfx_send_next_gain_q15(0u)==0u);
    for(unsigned n=0;n<5800u;n++)
        (void)stepfx_send_next_gain_q15(3u);
    assert(stepfx_send_next_gain_q15(3u)==0u);

    stepfx_send_adjust_division(1);
    assert(stepfx_send_division()==32u);
    stepfx_send_adjust_division(1);
    assert(stepfx_send_division()==32u);
    stepfx_send_adjust_division(-1);
    assert(stepfx_send_division()==16u);
    stepfx_send_adjust_division(-1);
    stepfx_send_adjust_division(-1);
    assert(stepfx_send_division()==4u);
    stepfx_send_adjust_division(-1);
    assert(stepfx_send_division()==4u);
    stepfx_send_adjust_division(1);
    stepfx_send_adjust_division(1);
    assert(stepfx_send_division()==16u);

    /* Step 1 SECOND press disables new sends immediately.
     * The real echo tail belongs to the existing FX engine (JB-20). */
    assert(jb_perfui_select(0u));
    assert(jb_perfui_active()==255u);
    stepfx_send_on_note(3,100u,120u);
    stepfx_send_begin_block(3u);
    assert(stepfx_send_next_gain_q15(3u)==0u);

    /* Legacy trigger FX still works in its own selected slots. */
    assert(jb_perfui_select(2u));
    assert(stepfx_slot()==2u);
    assert(jb_perfui_active()==2u);
    assert(jb_perfui_next()==JB_PERF_PAGE_CPU);
    assert(jb_perfui_cpu_active());
    assert(jb_perfui_next()==JB_PERF_PAGE_MAIN);
    assert(!jb_perfui_cpu_active());
    jb_perfui_next();jb_perfui_next();jb_perfui_leave();
    assert(jb_perfui_page()==JB_PERF_PAGE_MAIN);
    assert(jb_perfui_active()==2u); /* leaving page does not stop live FX */
    /* Full production UI -> gated mixer copy (FX select is an argument).
     * This simulates an actual Step 1 armed hit without synthesizing audio. */
    assert(jb_perfui_select(0u));
    assert(jb_perfui_active()==0u);
    int16_t dry[64], fx[128];
    for(unsigned i=0;i<64u;i++)dry[i]=12000;
    memset(fx,0,sizeof fx);
    stepfx_send_on_note(3u,100u,120u);
    stepfx_send_mix_voice(3u,0u,1u,dry,64u,1.0f,0.5f,fx);
    assert(fx[2u*63u]>0);
    assert(fx[2u*63u]>fx[2u*63u+1u]);
    for(unsigned i=0;i<64u;i++)assert(dry[i]==12000);
    assert(jb_perfui_select(0u));
    memset(fx,0,sizeof fx);
    stepfx_send_mix_voice(3u,0u,1u,dry,64u,1.0f,0.5f,fx);
    for(unsigned i=0;i<128u;i++)assert(fx[i]==0);

    puts("PASS JB-21: all 16 toggle parity (including Step 1), exclusive state, UI-to-FX-bus copy, dry parity, protected kick, note divisions and pages");
    return 0;
}
