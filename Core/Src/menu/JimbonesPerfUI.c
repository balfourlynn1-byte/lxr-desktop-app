/* JIMBONES Performance mode: one active Step FX action at a time.
 * The 16 physical buttons each toggle ON on first press, OFF on second.
 * Step 1 (index 0) is DUB SEND, NOT a master-off button.
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */
#include "menu/JimbonesPerfUI.h"
#include "Soundengine/StepFX.h"
#include "Soundengine/StepFxSend.h"

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
    /* Index 0 uses a separate audio send gate; existing repeat/stutter
     * scheduler keeps zero as its internal OFF value. */
    stepfx_select(index);
    if (index == 0u && !dub_levels_initialised) {
        /* Conservative first-use dub level, leaving kick protection to
         * StepFxSend mask (voices 1..5 enabled, voice 0 excluded). */
        for (uint8_t v=1u;v<STEPFX_SEND_VOICES;v++)
            stepfx_send_set_level(v,64u);
        dub_levels_initialised = 1u;
    }
    stepfx_send_set_enabled((uint8_t)(index==0u));
    return 1;
}
uint8_t jb_perfui_selected(void) { return selected_fx; }
uint8_t jb_perfui_active(void) { return armed_fx; }
const char *jb_perfui_effect_name(uint8_t index) {
    return index == 0u ? "DUBSEND" : stepfx_name(index);
}
