# JIMBONES — active hardware-test work orders (no USB audio)
Updated 2026-11-08. Working v1.8.02 remains frozen. **Assignments below are engineering roles, not live autonomous agents.** GitHub Issues are disabled for this repository (HTTP 410), so this tracked file is the assignment queue.

## Release gates
- **G0:** Preserve v1.8.02 image/rollback and existing six voices.
- **G1:** Four-voice stress regression including final mixer/post-FX and DWT deadline counters. Eliminate confirmed crunch mechanism.
- **G2:** Host + ARM CI and image memory map for exact integrated commit.
- **G3:** Device SD update, kit restore, pads, FD-8 and audible A/B against v1.8.02. Not automatable remotely.

## Paired agent assignments
| Task | Implementer work order | Independent reviewer gate | Dependency |
|---|---|---|---|
| JB-01 Baseline | Hash original v1.8.02, preserve recovery image and kit compatibility; compare build maps | Verify hashes, safe rollback and six-voice kit roundtrip | Immediate / G0 |
| JB-02 CPU | Consolidate DWT counters, snapshot last/peak cycles and deadline misses; no fabricated queue load | 1–4 hits, long tails, cycle wrap and ISR budget | Immediate / G1 |
| JB-03 Crunch | Instrument final mixer/pre-post FX peak and deadline correlation; isolate clipping vs overload | Deterministic PCM tests, A/B captures, pad verification | Immediate / G1 |
| JB-04 Morph | Implement immutable A, editable B/C, per-voice mask, kick exclude, smooth continuous parameters | Kit serialization, morph clicks, A unchanged | After G1 |
| JB-05 Tape | Opt-in per-voice saturation, DC block and level compensation | Bass/spectrum/aliasing/CPU/bypass | After FM/wavetable budget |
| JB-07 Step FX | Tempo event retrigger, 2/4/8-bar timing, kick exclusion; bounded buffers | Clock drift, RAM, feedback and pad stress | After G1 |
| JB-08 FM | Profile existing metallic FM; cache per-trigger coefficients | 1–3 FM layers, CPU/aliasing and sound parity | After G1 |
| JB-09 Wavetable | Profile interpolation and optional bandlimited mode; license review before external reuse | Sweep, flash/RAM, CPU and kit parity | After G1 |
| JB-10 Hi-hat | Optional FD-8 CC4/note choke state with short release ramp | MIDI replay, pedal hardware, default compatibility | Parallel isolated |
| JB-11 Snare | Seeded trigger-only variation and backbeat lock | Deterministic seed, 0/100%, save/load | After morph storage |
| JB-12 Filters | Opt-in stable bass HPF/LPF; bounded comb if memory permits | Frequency sweep, resonance and CPU | After G1 |
| JB-13 UI | Read-only render load/peak/deadline page, CLICK transient controls, Step FX LEDs, no ISR formatting | Menu navigation, debounce, real device | After JB-02 |
| JB-14 Transient CLICK | Finish attack/sustain DSP integration, actual CLICK submenu and compatible storage | Bit-exact bypass, host+ARM CI, real LXR menu and audio | After CLICK UI map |
| JB-15 FM Drum | Optional body/noise engine integrated with existing metallic FM | CPU and native bypass | After G1 |
| JB-16 Velocity Matrix | Optional per-voice mappings, protected A and kick exclude | MIDI velocity fixture and storage | After JB-04/JB-14 |
| JB-17 Comb Routing | Comb polarity and pre/post routing | Feedback stability, memory | After JB-12 |
| JB-18 Bit/SRR | Sample-rate reduction alongside existing bit quantisation | Aliasing, CPU, bypass | After G1 |
| JB-19 Osc Cluster | Optional CPU-bounded detuned oscillator | Four-voice render budget | After G1 |
| JB-20 Integration QA | Merge only reviewed feature PRs; exact SHA host+ARM CI, map, manifest and rollback | Independent combined device test and old-kit A/B | Final gate |

## Work protocol
1. Each implementer works in its `agents/*-research` branch or a new feature branch. Research alone is not a finished feature.
2. Each reviewer supplies runnable host fixtures and evidence. The integration branch receives one reviewed change at a time.
3. After every integration commit, require host regression and ARM image CI. Compare memory usage, bootloader boundaries and preserved sound/kit behavior.
4. Final firmware image is **hardware-test candidate only**, never called bug-free without physical LXR-02 confirmation.
5. **JB-06 USB audio is on hold**; no USB descriptors, routing or audio-streaming modifications in this release.
6. If a feature exceeds render budget or breaks compatibility, default-disable or defer it rather than compromise the combined image.

## Current verified baseline
Combined CPU telemetry and four-voice diagnostics commit `1790aec920905eaece2241dcd4fab24dfce9f7b9` passed host-regression and ARM firmware jobs in Actions run 42457405607. Later documentation-only commits are not automatically firmware validated. Multi-voice audible crunch remains unproven fixed. Feature branches primarily contain solution research, not completed feature code.

## Next execution queue
1. Implement final mixer/post-FX rail instrumentation (JB-03) and run host tests.
2. Reconcile CPU render counters and UI snapshot (JB-02/JB-13).
3. Add opt-in CC4 hi-hat state machine host fixture (JB-10).
4. Build A/B/C morph storage prototype with safe kit versioning (JB-04).
5. Profile FM/wavetable to set realistic effect CPU budgets.

## Live tracking
See [combined implementation tracker](JIMBONES_COMBINED_BUILD_IMPLEMENTATION_TRACKER.md) for feature-by-feature estimated readiness and explicit outstanding work. No autonomous agent processes are implied by these assignments.

## All-agent UI submission request (2026-11-08)
Every JB-01 through JB-20 workstream (JB-06 research-only/deferred) is assigned a feature-specific native submenu recommendation brief in [JIMBONES_UI_SUBMENU_RECOMMENDATION_REQUEST.md](agents/JIMBONES_UI_SUBMENU_RECOMMENDATION_REQUEST.md). Each must submit source-verified navigation, controls, defaults, kit storage, bypass, cost and tests in `docs/agents/ui-submissions/JB-XX-UI.md`. JB-13 consolidates a collision-free master navigation tree and JB-20 independently reviews. These are tracked assignments, **not autonomous agents actively submitting reports**.
