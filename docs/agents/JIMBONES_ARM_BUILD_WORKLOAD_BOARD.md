# JIMBONES ARM candidate — workload reallocation (2026-11-08)

## Operating rule
These are specialist workstream assignments and review responsibilities, **not autonomous agents that have been started**. Reuse completed workstreams for small tasks. One integration owner updates the active branch; parallel research/tests must not edit shared mixer/DSP files at the same time. Code and green CI count as progress; documents alone do not.

## Current verified baseline
- Run 42463459188: host-dsp, host-menu, arm-compile SUCCESS (previous dub send ramp candidate).
- Subsequent UI-toggle commit requires its own CI verification.
- No Clip on physical LXR-02 is NOT verified. No audible dub-delay-tail proof yet.

| Workstream | Status | Next small deliverable | Dependencies | Acceptance gate |
|---|---|---|---|---|
| JB-44 Step FX UI | Code + host regression committed | Validate LED behaviour and step 1 OFF on hardware | UI CI | host-menu/host-dsp/ARM |
| JB-47 Step FX QA | Reassigned from planning | Impulse test proving delay tail persists after send-off | JB-46 | DSP test with assertions |
| JB-45 Dub delay | Native delay research complete | Check FX_DELAY wet return and FX slot conflict; no silent takeover | JB-46 | source audit + test |
| JB-46 Dub routing | Send taps + ramps committed | Confirm per-voice send bypass, double routing and timing | JB-45 | native mixer test |
| JB-31 Mixer trace | Reassigned P0 | Map every output rail, native FX path and gain stage | none | source map |
| JB-32 Rail telemetry | Reassigned P0 | Add non-invasive native mixer/FX peak counters | JB-31 | deterministic regression |
| JB-33 CPU deadline | Reassigned P0 | Verify render deadline/peak counters under 2–4 voices | none | ARM timing proof |
| JB-34 Two-voice QA | Reassigned P0 | Reproduce reported two-voice mod crunch in a failing regression | none | red test before fix |
| JB-35 Four-voice QA | Reassigned P0 | Long-tail + modulation + retrigger stress | JB-34 | deterministic regression |
| JB-39/40/42 CI/release | Reassigned from completed CI work | SHA, memory map, artifact hash, stop/rollback criteria | all P0 | three CI jobs + review |
| JB-41 Hardware | Pending user test | SD backup and actual Alesis pad audio recording | release gate | real hardware evidence |

## Scheduling
**Lane A (fast UI/test fixes):** JB-44 then JB-47. One task per commit, host tests required.
**Lane B (No Clip root cause):** JB-31 → JB-32 and JB-34 → JB-35; integrate only after independent review.
**Lane C (release engineering):** JB-33 → JB-39/40/42; no new feature scope.
**Lane D (dub integration):** JB-45 → JB-46; do not activate or override global FX silently.

## Candidate gates
1. Six original voices and wavetable unaffected, dry bypass bit-exact.
2. No Clip: two-, three-, four-voice modulation tests and no missed audio deadlines.
3. Step FX: step 1 OFF, repeat press OFF, LEDs, native rolls unaffected outside Step FX page.
4. Dub FX: feedback tail after send-off, bounded send ramp, no kick send by default, no FX slot conflict.
5. Host DSP, host menu and ARM compilation success on exact candidate SHA, artifact hash recorded.
6. Hardware install only after backup/rollback instructions and explicit user decision.
