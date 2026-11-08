# JB-07B — Step FX button toggle + flashing active LED
**Assigned:** Step FX Implementer (virtual engineering role) and independent UI/firmware Reviewer.
**Branch:** agents/stepfx-led-toggle-implementation
**Status:** assigned; not yet implemented or tested. No autonomous background execution.

## User-approved interaction contract
- Press a step button once: turn its assigned Step FX effect **ON**.
- Press that same button again: turn **OFF** only that effect.
- The LED of each active effect **flashes** at a visible consistent rate; when inactive, its LED is dark.
- Multiple effects may be active at once; buttons are independent.
- Step 1 is no longer a global OFF control: all 16 buttons can be assigned effects.
- Preserve the ability to see the sequencer playhead/step indication without confusing it with the active-effect flash.
- LED blink must be nonblocking; no delay/sleep inside audio or input interrupt; avoid additional DSP load.
- Keep the existing step 16 morph LFO assignment where appropriate, without hard-coding it if assignment is configurable.

## Implementer checklist
1. Inspect actual button event dispatch, StepFX state and LED render ownership; identify mode conflicts.
2. Replace one-shot/step-1-off behavior with edge-triggered same-button toggle; debounce/held-key handling.
3. Maintain independent effect-active bits and define blink phase from existing UI timing tick.
4. Merge blinking with sequencer LEDs safely; no LED writes in audio render.
5. Add host tests for ON->OFF, independence, step 1, long press, mode transitions, and flash phase.
6. Commit implementation on this branch, then request independent review.

## Reviewer acceptance
- Host regressions pass; ARM firmware compiles on exact commit.
- Hardware test on LXR-02 confirms LEDs flash only for enabled effects, and each button toggles reliably.
- No regressions to six legacy voices, kit compatibility, audio timing, existing sequencer UI.
- Preserve working v1.8.02; integration branch only after review.
