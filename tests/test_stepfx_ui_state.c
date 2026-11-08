#include "Soundengine/JimbonesStepFxUiState.h"
#include <assert.h>
int main(void) {
 JbStepFxUiState s; jb_stepfx_ui_init(&s);
 assert(!jb_stepfx_ui_active(&s,0));
 jb_stepfx_ui_toggle(&s,0);assert(jb_stepfx_ui_active(&s,0));
 assert(jb_stepfx_ui_led(&s,0,0,8));assert(!jb_stepfx_ui_led(&s,0,4,8));
 jb_stepfx_ui_toggle(&s,2);assert(jb_stepfx_ui_active(&s,0));assert(jb_stepfx_ui_active(&s,2));
 jb_stepfx_ui_toggle(&s,0);assert(!jb_stepfx_ui_active(&s,0));assert(jb_stepfx_ui_active(&s,2));
 assert(!jb_stepfx_ui_led(&s,0,0,8));
 jb_stepfx_ui_toggle(&s,15);assert(jb_stepfx_ui_active(&s,15));
 jb_stepfx_ui_toggle(&s,16);assert(jb_stepfx_ui_active(&s,15));
 assert(!jb_stepfx_ui_led(&s,2,0,0));
 return 0;
}
