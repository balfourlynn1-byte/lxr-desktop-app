/* JIMBONES Performance page state (host-testable, no hardware dependencies).
 * SPDX-License-Identifier: GPL-3.0-only
 */
#ifndef JIMBONES_PERF_UI_H
#define JIMBONES_PERF_UI_H
#include <stdint.h>

enum {
    JB_PERF_PAGE_MAIN = 0,    /* Original Performance: parameter bank 1 */
    JB_PERF_PAGE_MORE = 1,    /* Original Performance: parameter bank 2 */
    JB_PERF_PAGE_STEPFX = 2, /* Third press: JIMBONES Step FX */
    JB_PERF_PAGE_CPU = 3     /* Fourth press: CPU / output diagnostics */
};

void jb_perfui_enter(void);
uint8_t jb_perfui_next(void);
void jb_perfui_leave(void);
uint8_t jb_perfui_page(void);
uint8_t jb_perfui_stepfx_active(void);
uint8_t jb_perfui_cpu_active(void);
uint8_t jb_perfui_select(uint8_t index); /* physical step index 0..15: toggle arm/off */
uint8_t jb_perfui_selected(void); /* focused button for editing */
uint8_t jb_perfui_active(void);   /* armed button 0..15, or 255 = all OFF */
uint8_t jb_perfui_focus(uint8_t index); /* knob browse without arming */
const char *jb_perfui_effect_name(uint8_t index);

#endif
