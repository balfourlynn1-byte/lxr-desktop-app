/* JIMBONES Performance mode third-page selector.
 * Selectors arm the bounded live-MIDI Step FX scheduler.
 * SPDX-License-Identifier: GPL-3.0-only
 */
#include "menu/JimbonesPerfUI.h"
#include "Soundengine/StepFX.h"

static uint8_t active_page = JB_PERF_PAGE_MAIN;
static uint8_t selected_fx = 0;

void jb_perfui_enter(void) { active_page = JB_PERF_PAGE_MAIN; }
uint8_t jb_perfui_next(void) {
    active_page = (uint8_t)((active_page + 1u) % 3u);
    return active_page;
}
void jb_perfui_leave(void) { active_page = JB_PERF_PAGE_MAIN; }
uint8_t jb_perfui_page(void) { return active_page; }
uint8_t jb_perfui_stepfx_active(void) { return active_page == JB_PERF_PAGE_STEPFX; }
uint8_t jb_perfui_select(uint8_t index) {
    if (index >= 16u) return 0;
    selected_fx = index;
    stepfx_select(index);
    return 1;
}
uint8_t jb_perfui_selected(void) { return selected_fx; }
const char *jb_perfui_effect_name(uint8_t index) {
    return stepfx_name(index);
}
