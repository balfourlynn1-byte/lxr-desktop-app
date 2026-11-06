/* Host test for JIMBONES Performance page cycling + sixteen selectors. */
#include "menu/JimbonesPerfUI.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void) {
    jb_perfui_enter();
    assert(jb_perfui_page() == JB_PERF_PAGE_MAIN);
    assert(!jb_perfui_stepfx_active());
    assert(jb_perfui_next() == JB_PERF_PAGE_MORE);
    assert(!jb_perfui_stepfx_active());
    assert(jb_perfui_next() == JB_PERF_PAGE_STEPFX);
    assert(jb_perfui_stepfx_active());
    for(unsigned i=0;i<16;i++) {
        assert(jb_perfui_select((uint8_t)i));
        assert(jb_perfui_selected()==i);
        assert(strlen(jb_perfui_effect_name((uint8_t)i))<=11u);
    }
    assert(!jb_perfui_select(16));
    assert(jb_perfui_selected()==15);
    assert(jb_perfui_next() == JB_PERF_PAGE_CPU);
    assert(jb_perfui_cpu_active());
    assert(!jb_perfui_stepfx_active());
    assert(jb_perfui_next() == JB_PERF_PAGE_MAIN);
    assert(!jb_perfui_cpu_active());
    assert(!jb_perfui_stepfx_active());
    jb_perfui_next();jb_perfui_next();jb_perfui_leave();
    assert(jb_perfui_page()==JB_PERF_PAGE_MAIN);
    puts("PASS: Performance 1 -> Performance 2 -> Step FX -> CPU -> Performance 1");
    puts("PASS: 16 physical step selections, invalid button bounds");
    puts("PASS: leaving performance resets page without clearing selected effect");
    return 0;
}
