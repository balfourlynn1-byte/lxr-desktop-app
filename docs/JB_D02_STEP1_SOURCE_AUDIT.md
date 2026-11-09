# D02 — Step 1 LED and dub timing source audit (2026-11-09)

Status: PARTIAL, SOURCE VERIFIED, TEST EXECUTION NOT RUN, HARDWARE NOT VERIFIED.

## Evidence from current branch
- `Core/Src/ledHandler.c`: `led_idx2StepLed(idx)` uses `idx%16`, and case 0 maps to `LED_STEP1`; `led_initSequencerLeds` loops over 16 steps and writes `seq_isMainStepActive(..., menu_activeBar*16+i,...)` to the mapped LED. Thus the generic index-to-LED map does not itself skip Step 1.
- `Core/Src/buttonHandler.c`: sequencer step toggling calls `seq_toggleMainStep`, checks `seq_isMainStepActive`, and writes the mapped LED. Performance Step FX has a distinct branch: `menu_stepFxSelect(seqButtonPressed)`, clears sequencer LEDs and lights the currently armed slot using `led_idx2StepLed(menu_stepFxActive())`. CPU diagnostics intercepts Step 1 to reset diagnostics. This mode-specific interception is a plausible cause of *some* Step 1 reports but is **not proven** to explain the user's ordinary sequencer-mode observation.
- `Core/Src/menu/JimbonesPerfUI.c`: indices 0–4 select the five dub timings `{2,1,4,0,5}`, corresponding to 1/4, 1/8, triplet eighth, 1/16, 1/32; the same button toggles armed to OFF. Indices 5–15 select legacy Step FX. Kick is excluded from the default dub-send mask.
- `tests/test_perfui.c` already asserts all 16 button toggles, five timing divisions, dry parity and kick protection; `tests/test_stepfx_ui_source.py` asserts LED mapping and Step FX routing. Both are wired via `tests/run_stepfx_ui.sh` in `tests/run_host.sh`. **Not executed in this audit.**

## Remaining D02 acceptance gates
1. Trace front-panel LED transport and any special-case Step 1 clear/reset, blink ownership and mode precedence, including during playback and after pattern switch. Verify whether the report concerns sequencer mode versus performance Step FX.
2. Add deterministic host fixture for sequencer step 0 on/off and LED output, comparing steps 1, 2 and 16; cover stopped/playing, pattern switching and Performance/CPU page transitions.
3. Execute GCC and Clang host suites on exact commit and record output; hardware illumination and timing remain unknown until physical testing.

No firmware or UI logic changed; no ARM compiler or image packaging invoked.
