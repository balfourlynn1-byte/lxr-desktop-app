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

## Idle workstream redeployment — build-completion sprint

**Scheduling constraint:** This is a persistent assignment queue, not proof that autonomous agents are running. An owner may only be marked DONE after actual source/test changes, CI evidence and review.

### Fast-track tasks delegated to lower-risk/idle feature workstreams
| Owner | Microtask | Files / boundary | Exit evidence |
|---|---|---|---|
| JB-01 Global/About | Add version, git SHA and candidate/experimental warning to About/splash where memory allows | UI/version only; no DSP | ARM build and readable 16x2 text |
| JB-07 Step FX | Audit press/repress OFF and manual roll release, including SHIFT and performance page exit | buttonHandler, StepFX UI only | production selector host regression and LED review |
| JB-09 Wavetable | Add wavetable interpolation and bypass parity test cases | host DSP tests only; no oscillator refactor | deterministic six-voice test |
| JB-10 Hi-hat | Create FD-8 open/closed/choke MIDI event test plan and host fixture | MIDI tests only; no DSP changes | reproducible fixture |
| JB-11 Snare | Add deterministic probability/no-backbeat-loss test fixture, gated OFF by default | tests only | stable seed and zero missed backbeats |
| JB-13 Native UI | Check all 16 physical step-key indexes, step 1 OFF, page leave/re-enter, native kit UI | menu/button files only | host menu + ARM |
| JB-14 Transient | Add transient amount change / silence / pop-energy regression | host DSP tests only | threshold assertions |
| JB-16 Velocity | Add min/max/zero velocity boundary tests for six voices | tests only | no overflows or unintentional triggers |
| JB-18 LOFI | Add SRR factor-change and bypass click/discontinuity regression | host DSP tests only | threshold assertions |
| JB-19 Cluster | Estimate existing CPU/voice memory cost and identify unsafe defaults; no new engine yet | profiling report | concrete measurements |
| JB-20 Release | Write exact-commit hardware install and rollback checklist, label image UNVERIFIED | release checklist only | complete recovery instructions |
| JB-23 Audio QA | Create 2-voice modulated overlap reproducer, then 3/4 voice stress | tests only | red/green evidence |
| JB-24 ARM CI | Capture linked memory regions, image SHA and workflow artifact expiry | CI/scripts only | exact SHA, hash and size |
| JB-26 Optimisation | Inspect wavetable/metal loops for expensive operations, no DSP edits before measurement | profiler/tests | cycle delta |
| JB-29 PCM QA | Audit dub throw send-off tail with existing FX_DELAY, saturation rails and feedback | tests only | impulse response assertion |
| JB-30 CI gate | Add independent Step FX UI regression and candidate gate summary | workflow/tests only | all jobs success |
| JB-40 ARM packaging | Produce manifest with firmware SHA256, size and build commit | release scripts only | reproducible manifest |
| JB-41 Hardware QA | Prepare concise Alesis 2/3/4-voice test matrix and stop conditions | checklist only | human test ready |

### Reserved high-risk code ownership
- **JB-03/JB-21/JB-31/JB-32/JB-34:** No Clip native mixer root cause and telemetry. Only these workstreams modify mixer.c or final FX audio output until cause measured.
- **JB-33/JB-02/JB-22:** CPU/deadline measurement; do not share files with mixer team without coordination.
- **JB-45/JB-46:** Dub delay send and FX bus integration. No silent global FX takeover; preserve delay feedback tails.
- **JB-44:** Step FX physical toggle + LEDs. Integrate after host selector test.
- **JB-39/JB-42:** final candidate gate and release signoff, cannot sign off own unreviewed changes.

### Priority and work-in-progress limits
1. **P0 release gate:** JB-23/29 audio reproduction; JB-31/32 mixer; JB-33 CPU; JB-24/30 ARM; JB-20/41 rollback.
2. **P1 low-risk parallel:** JB-07/13/44 UI; JB-09/14/16/18 tests; JB-45/46/47 dub-tail.
3. **P2 deferred until stable:** morph, tape, extra oscillators, filter engines, USB.
4. Maximum **one active editor per production source file**; maximum two unreviewed audio-path changes; one integration owner controls merges.
5. Done means tested code, not work-order creation. An idle assignment without a running worker remains QUEUED.
