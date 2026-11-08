# JIMBONES — agent finish-line execution contracts
Date: 2026-11-08. Target: experimental v1.8.03+ combined candidate; preserve working v1.8.02.

## Execution policy
The "agents" below are assigned implementer/reviewer roles, **not** live autonomous AI processes. Each pair must deliver source code or a justified infeasibility decision, tests, measurements and a focused PR. Do not mark done on research alone. Work on separate branches; integration reviews and merges sequentially. No automatic flash. Report actual evidence, not assumed performance.

| ID | Pair / mission | Smallest shippable deliverable | Verification to finish | Dependency / release decision |
|---|---|---|---|---|
| JB-01 | Verification + Integration: baseline and recovery | Exact v1.8.02 vs experimental source/image comparison; kit compatibility matrix, version/rollback procedure, hash manifest | Reproducible ARM image, boot/recovery test on device, six voices and saved kit compatibility | Gate A; mandatory before release |
| JB-02 | Embedded + DSP: CPU diagnostics | DWT render-cycle load/peak/deadline counters integrated with existing AudioDiagnostics; optional actual queue pressure only when queue signal verified | Host boundary/overflow tests, host + ARM CI, bounded ISR overhead, hardware deadline counters under four hits | Gate A; PR #3 CI green individually, not yet integrated |
| JB-03 | DSP + Verification: modulation crunch | Full-path stress fixture plus measured root-cause fix for 2+ modulated voices; preserve sound at single voice | 1–4 voice repeatable tests, final mixer/post-FX rail counts, render overruns, before/after audio and pad hardware confirmation | Gate B; test fixtures staged into integration branch |
| JB-04 | DSP + Embedded: morph A/B/C | A-protected snapshot, editable B/C, per-voice mask/kick exclusion, LFO + velocity-to-morph with bounded smoothing | Save/load compatibility, fast-switch/no-click tests, protected A unchanged, six-voice CPU benchmark | Gate C after stable baseline |
| JB-05 | DSP + Verification: tape/tube | One opt-in per-voice soft-saturation mode with level matching, no global kick coloration | Peak/spectrum/aliasing comparison, CPU worst-case, bypass null test and low-frequency integrity | Gate D after JB-02/03 |
| JB-06 | Research + Embedded: 4-channel USB audio | STM32 peripheral/descriptor/pin/clock/RAM audit and USB bandwidth estimate; prototype only if viable | Class-compliant Mac detection, four distinct routed channels, sync/dropout/latency measurements; otherwise documented no-go | Gate F optional; cannot block main release |
| JB-07 | DSP + Embedded: step performance FX | Short tempo-synced repeat first; step 1 off / step 16 morph LFO mapping; kick-exclusion routing | Clock sync, repeat interval tests, 2/4/8 bar scheduler, buffer RAM, bounded feedback, CPU stress | Gate E after JB-02/03 |
| JB-08 | DSP + Research: metallic FM | Profile current FM; one opt-in optimisation or parameter improvement preserving original modes | 1–3 metallic layers, deterministic output, aliasing comparison and ARM cycle delta | Gate D; no unmeasured rewrite |
| JB-09 | DSP + Research: wavetable/waveshaping | Compare interpolation and waveshaping with referenced upstream; implement one compatible opt-in improvement | License audit, wavetable sweep spectral checks, host+ARM CI, CPU and ROM impact | Gate D; retain current table sound |
| JB-10 | Embedded + Verification: hi-hat | FD-8 open/closed/choke state machine through existing Ableton MIDI route, adjustable velocity response | Recorded MIDI fixture for open/closed transitions, repeated strikes and choke, hardware pedal test | Gate C; legacy MIDI unchanged by default |
| JB-11 | DSP + Embedded: snare variation | Seeded per-hit snare engine choice with configurable probability and backbeat guard | Repeatable seeded sequence, 0/100% edge tests, no mid-tail state corruption, saved kit compatibility | Gate C after JB-04 |
| JB-12 | DSP + Embedded: bass/comb filters | Opt-in stable bass HPF/LPF first, then bounded comb with preallocated memory | Frequency sweep, resonance stability, coefficient interpolation, CPU/RAM limit and bypass parity | Gate D; comb optional if budget fails |
| JB-13 | Embedded + Verification: UI | Render-load peak and overrun page; nonblocking update and double-press navigation, distinguish CPU render load vs queue | Menu tests, debounce, no display work in audio ISR, device legibility | Gate C/E after JB-02 |

## Track assignments
- **Track A — Reliability (start immediately):** JB-01, JB-02, JB-03. Block risky DSP changes until measurements are available.
- **Track B — Feature prototypes (independent branches):** JB-04, JB-05, JB-08, JB-09, JB-10, JB-11, JB-12, JB-13. Prototype in host fixtures, then integrate only when Gate A/B pass.
- **Track C — Architecture / performance:** JB-06 and JB-07. Feasibility and RAM/CPU designs first; USB may finish with a documented no-go.
- **Integration lead:** review one focused PR at a time, reconcile shared test runner, rerun host and ARM, record exact revision and SHA256; final user hardware sign-off.

## Definition of done for every pair
1. Implementer provides precise changed files, commit SHA, user-visible behavior and rollback switch.
2. Reviewer provides a reproducible test with pass/fail output, edge-case review, CPU/RAM budget and compatibility assessment.
3. Host CI and ARM CI pass on **the integration commit**; isolated branch CI is not enough.
4. If audio behavior changed: reference WAV/PCM comparison and real LXR-02 pad test required for release.
5. Documentation records limitations, upstream license checks, firmware image checksum and device recovery.
6. Any feature that cannot meet CPU/RAM/USB constraints is deferred or rejected explicitly, not silently marked complete.

## Current evidence snapshot
- CPU branch PR #3 commit c57c519 passed host and ARM CI in run 42456940467; code not yet integrated.
- Crunch branch PR #2 commit 28ef6f7 passed host and ARM CI in run 42456867458; two test files and host runner changes copied into integration branch at fb66966.
- Integration branch run 42457225292 was in progress when this plan was authored; do not call it passed until complete.
- Ten feature branches still contain research notes only; no completed new feature PRs.
- The two active branches are behind integration; avoid blindly merging PR #2 due to merge conflicts.

## Immediate coordinator queue
1. Confirm run 42457225292, read failing job logs if any.
2. Integrate JB-02 telemetry carefully, deduplicate existing DWT counters and preserve the test runner.
3. Collect actual JB-03 summed-PCM metrics and correlate with CPU budget.
4. Start JB-04 and JB-10 host-only prototypes while reliability gates are being completed.
5. Produce one combined experimental ARM artifact only after integration host+ARM checks succeed.
