/* JIMBONES Step FX UI state — pure, allocation-free, host-testable. */
#ifndef JIMBONES_STEPFX_UI_STATE_H
#define JIMBONES_STEPFX_UI_STATE_H
#include <stdint.h>
typedef struct { uint16_t enabled; } JbStepFxUiState;
static inline void jb_stepfx_ui_init(JbStepFxUiState *s) { s->enabled=0; }
static inline void jb_stepfx_ui_toggle(JbStepFxUiState *s,unsigned step) {
 if(step<16u) s->enabled ^= (uint16_t)(1u<<step);
}
static inline unsigned jb_stepfx_ui_active(const JbStepFxUiState *s,unsigned step) {
 return step<16u && (s->enabled & (uint16_t)(1u<<step))!=0u;
}
/* blink_period_ticks >= 2; caller supplies UI tick, not audio cycles. */
static inline unsigned jb_stepfx_ui_led(const JbStepFxUiState *s,unsigned step,
 uint32_t ui_tick,uint32_t blink_period_ticks) {
 return blink_period_ticks>=2u && jb_stepfx_ui_active(s,step) &&
  (ui_tick%blink_period_ticks)<(blink_period_ticks/2u);
}
#endif
