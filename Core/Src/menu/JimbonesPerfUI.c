/* JIMBONES Performance mode: one active Step FX action at a time.
 * The 16 physical buttons each toggle ON on first press, OFF on second.
 * Steps 1-5 are five tempo-division DUB SEND presets, not master OFF.
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */
#include "menu/JimbonesPerfUI.h"
#include "Soundengine/StepFX.h"
#include "Soundengine/StepFxSend.h"
#include "Soundengine/DubDelaySync.h"

static uint8_t active_page = JB_PERF_PAGE_MAIN;
static uint8_t selected_fx = 0; /* browsing / edit focus */
static uint8_t armed_fx = 255u;  /* no active button */
static uint8_t dub_levels_initialised;

void jb_perfui_enter(void) { active_page = JB_PERF_PAGE_MAIN; }
uint8_t jb_perfui_next(void) {
    active_page = (uint8_t)((active_page + 1u) % 4u);
    return active_page;
}
void jb_perfui_leave(void) { active_page = JB_PERF_PAGE_MAIN; }
uint8_t jb_perfui_page(void) { return active_page; }
uint8_t jb_perfui_stepfx_active(void) { return active_page == JB_PERF_PAGE_STEPFX; }
uint8_t jb_perfui_cpu_active(void) { return active_page == JB_PERF_PAGE_CPU; }
uint8_t jb_perfui_focus(uint8_t index) {
    if (index >= 16u) return 0;
    selected_fx = index;
    return 1;
}
uint8_t jb_perfui_select(uint8_t index) {
    if (!jb_perfui_focus(index)) return 0;
    if (armed_fx == index) {
        armed_fx = 255u;
        stepfx_select(0u); /* releases scheduled repeats */
        stepfx_send_set_enabled(0u); /* stops new sends */
        return 1;
    }

    armed_fx = index;
    /* First 5 pads control a single existing dub processor, with one
     * exclusive effect armed at a time. The legacy repeat scheduler stays
     * OFF for these five; buttons 6-16 keep their original effects. */
    static const uint8_t dub_divisions[5]={2u,1u,4u,0u,5u};
    const uint8_t dub=(uint8_t)(index<5u);
    stepfx_select(dub?0u:index);
    if (dub) dub_delay_set_division(dub_divisions[index]);
    if (dub && !dub_levels_initialised) {
        /* Conservative first-use dub level, leaving kick protection to
         * StepFxSend mask (voices 1..5 enabled, voice 0 excluded). */
        for (uint8_t v=1u;v<STEPFX_SEND_VOICES;v++)
            stepfx_send_set_level(v,64u);
        dub_levels_initialised = 1u;
    }
    stepfx_send_set_enabled(dub);
    return 1;
}
uint8_t jb_perfui_selected(void) { return selected_fx; }
uint8_t jb_perfui_active(void) { return armed_fx; }
const char *jb_perfui_effect_name(uint8_t index) {
    static const char *const dub_names[5]={
        "DUB 1/4","DUB 1/8","DUB T8","DUB 1/16","DUB 1/32"
    };
    return index<5u?dub_names[index]:stepfx_name(index);
}
