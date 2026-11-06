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
uint8_t jb_perfui_select(uint8_t index); /* physical step index 0..15 */
uint8_t jb_perfui_selected(void);
const char *jb_perfui_effect_name(uint8_t index);

#endif
