# Debug agent DBG-01 — CPU telemetry reset regression
**Status:** assigned to virtual Debug Implementer and Independent Debug Reviewer. These are task roles, not background-running AI processes.
**Branch:** agents/debug-telemetry-reset
**Base:** jimbones-v1.8.03-hardware-test
**Bug:** prior jb_audioDiagReset cleared public jb_audioOverruns but not the private jb_renderTelemetry.over_budget; next audio block restored old counter.
**Initial fix already committed on integration:** d0bfc25e85bb069426d09d02d50475334d37c148. This branch starts from that fix; debug agent must validate, not duplicate it.

## Implementer tasks
1. Inspect Soundengine.c private telemetry reset, AudioDiagnostics.c critical section, and reset API.
2. Build a deterministic regression that records an over-budget block, invokes reset, records an under-budget block and asserts public/private deadline count remains zero.
3. Verify reset can be called only from safe non-audio context and that IRQ masking does not alter audio deadlines.
4. Identify whether a reset concurrent with render or initialization can produce a race; document constraints.
5. Check 32-bit counter saturation/wrap and monitor UI semantics.

## Reviewer acceptance
- Host regression explicitly fails before fix and passes after fix, where feasible.
- ARM firmware build succeeds and memory map is reviewed.
- All existing host tests pass including 1–4 voice fixtures.
- No change to six-voice sound, DSP gains, kit format, USB audio or bootloader.
- User hardware test confirms reset, current/peak load and overruns remain coherent after 3–4 simultaneous hits.
- Publish exact commits, CI links and unresolved limitations. Never claim hardware verified from CI alone.

## Release policy
No new flashable image is declared ready solely from this assignment. v1.8.02 is preserved. If this regression cannot be covered by host tests, report that gap explicitly.
