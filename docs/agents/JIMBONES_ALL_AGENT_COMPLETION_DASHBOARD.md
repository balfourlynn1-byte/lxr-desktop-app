# JIMBONES — All 26 workstreams completion and next steps

**Audit date: 2026-11-08.** Percentages are *engineering planning estimates*, not automated GitHub metrics or claims of validated feature readiness. They weigh code, integration, tests, review and device proof. A shared integration feature can have progress even where its individual work-order branch has no implementation.

**Evidence:** Compared JB-01–12 and JB-14–20 individual branches with `agents/jb13-native-submenu-ui`: each is 1 documentation-only commit ahead and 14 behind; JB-21–26 branches each 1 documentation-only commit ahead and 2 behind. JB-06 intentionally deferred. JB-13 is shared integration branch with real menu and DSP/test changes. PR #7 open draft, zero submitted reviews, no PR-triggered workflow runs returned for latest reviewed commit. Therefore **no individual agent has completed independently reviewed feature code**.

**Overall estimated release readiness: 25% (subjective, not the mean of agent percentages).** This estimate represents preparation and partial shared code; actual ARM and hardware gates remain unsatisfied. Do not interpret as 25% of bugs fixed or audio quality.

| Agent | Workstream | Estimated complete | Concrete next step |
|---|---|---:|---|
| JB-01 | Global/About | 10% | Implement version/build details and verify original global settings |
| JB-02 | CPU monitor | 30% | Connect existing telemetry to real peak render cycles and hardware deadlines |
| JB-03 | No Clip | 15% | Reproduce overlapping modulation crunch, isolate DSP/mixer clipping and fix |
| JB-04 | Morph A/B/C | 10% | Implement protected A and editable B/C with voice exclusions and tests |
| JB-05 | Tape saturation | 5% | Implement opt-in tape DSP, gain staging, bypass parity and CPU tests |
| JB-06 | USB multitrack | 0% | Remain deferred until hardware USB feasibility and No Clip baseline |
| JB-07 | Step performance FX | 25% | Verify existing step FX control against audio timing and multi-voice load |
| JB-08 | Metallic FM | 15% | Integrate metal layer with native OSC, measure aliasing and CPU |
| JB-09 | Wavetable/waveshaping | 20% | Wire waveshaping UI and test interpolation and original sound preservation |
| JB-10 | Hi-hat articulation | 5% | Implement FD-8 choke/open-closed transitions and test MIDI routing |
| JB-11 | Snare probability | 5% | Implement deterministic bounded randomization without losing backbeat |
| JB-12 | Bass/comb filter | 10% | Implement real filter DSP and verify frequency/feedback stability |
| JB-13 | Native menu router | 55% | Compile full firmware menu, fix navigation edge cases, kit load and physical 16x2 UI |
| JB-14 | Transient designer | 45% | Verify existing DSP + CLICK wiring, transition peaks, persistence decision |
| JB-15 | FM drum | 5% | Implement isolated FM drum engine with CPU and audio tests |
| JB-16 | Velocity matrix | 5% | Implement calibrated mapping, boundaries and protected kick |
| JB-17 | Comb routing | 5% | Implement feedback-safe routing and per-voice CPU limits |
| JB-18 | LOFI/SRR | 45% | Run host regression on recent 64-sample transition, quantify click/pop and bypass parity |
| JB-19 | Cluster oscillator | 5% | Implement bounded layer count and voice load tests |
| JB-20 | Release integration | 10% | Collect independent reviews, exact SHA CI, image checksum, hardware gate |
| JB-21 | Mixer crunch root cause | 5% | Instrument and isolate 2/3/4 simultaneous modulated voices |
| JB-22 | CPU deadline specialist | 5% | Add peak-cycle and underrun telemetry, stress four long-tail voices |
| JB-23 | Independent audio QA | 5% | Create PCM clipping, transient, bypass and multi-voice regressions |
| JB-24 | ARM build specialist | 5% | Fix exact-commit host/ARM CI, linker/memory and artifact verification |
| JB-25 | Morph specialist | 5% | Implement A/B/C protected voice DSP and versioned kit handling |
| JB-26 | Layered DSP optimisation | 5% | Profile metal/wave/cluster, reduce CPU while preserving sound |

## Critical path
1. **JB-21 + JB-03:** reproduce original multi-voice crunch and implement bounded root-cause fix. Do not hide defects behind clipping or gain attenuation without identifying cause.
2. **JB-22 + JB-02:** CPU peak cycles, interrupt deadline and voice stress evidence, coordinated with JB-26.
3. **JB-23:** independently quantify no-clip rails, bypass parity and transition pops on exact integration commit; review JB-21/22.
4. **JB-13 + JB-14 + JB-18:** compile full native menu and DSP integration, verify all six voices, kit reload and new SRR transition; clarify temporary vs persistent settings.
5. **JB-24 + JB-20:** exact-commit host tests PASS, ARM compile/link PASS, independent PR approvals, artifact checksum and memory budget; then make *hardware test candidate* only, not a verified production release.
6. **JB-25 and JB-04** then **JB-26, JB-08, JB-09, JB-19**, followed by remaining features, only after P0 audio gates are green.

## Definition of 100% per feature
Real code merged into reviewed integration branch; deterministic host tests pass; no CPU/deadline regressions; native bypass and original voices preserved; kit persistence or explicit TEMP design verified; ARM compile/link passes; physical LXR-02 UI and multi-voice audio test passes. Hardware-dependent features cannot be 100% before physical validation.

## Constraints
These are assignments and estimates, not running background agents. No peer reviews have been submitted; no new ARM image or hardware qualification was produced in this audit. Working firmware untouched.

## Progress update — 2026-11-08
- **JB-23 audio QA:** added `four_voice_mix_regression` to production DSP host tests: four voices, transient and SRR enabled, long decays, conservative 1/4 PCM summing, assert non-silent and no full-scale sample rails. This is a meaningful **new test implementation** in shared integration, not an independent JB-23 agent PR. Test has not been run in CI. Planning estimate JB-23 **5% → 12%** (test written, unverified).
- **JB-03/JB-21:** no root-cause fix for the original hardware crunch; 15%/5% unchanged. A conservative PCM mixer fixture cannot prove hardware mixer integrity.
- **JB-13/JB-20:** corrected a misleading kit-revision test label; UI reset still requires actual end-to-end test. Progress unchanged.
- **Next:** compile and run `tests/test_jimbones.c` and menu fixture for exact commit, then inspect audio peaks and compare with actual LXR mixer; no ARM image or hardware PASS yet.

## Progress update — host test reliability pass
- **JB-23:** fixed the four-voice PCM regression's C diagnostic newline escape. Source updated; no host run has been executed.
- **JB-13/JB-24:** `tests/test_menu.py` now creates `build/host` before writing its extracted C fixture. This removes a deterministic fresh-checkout failure mode; compilation and runtime still unverified.
- **Release gate:** no physical LXR-02 audio proof, no verified latest host/ARM CI, no independent approvals. Keep next ARM candidate blocked pending those gates.

## Verified CI improvement — run 42462298513
- Added `tests/host_include/stm32f7xx_hal.h` isolated host-only interrupt/HAL shim and `tests/run_jimbones_host.sh` to compile **actual production Jimbones.c, wavetable.c, JimbonesPreset.c** with `tests/test_jimbones.c`.
- Added `host-dsp` job to `.github/workflows/jimbones-candidate-gate.yml`.
- **Actual GitHub CI evidence:** host-dsp SUCCESS, host-menu SUCCESS on workflow run 42462298513; ARM compilation still IN PROGRESS when observed. This does not demonstrate hardware No Clip or DAC/mixer behavior.
- Planning estimates: JB-23 **12% → 25%** (audio regression now actually runs in CI); JB-24 **5% → 25%** (CI now verifies DSP and menu; ARM status pending); JB-30 **5% → 20%** (real 3-job gate added). These are estimates, not measured percent completion.
- Next: confirm exact-commit ARM result, collect link/artifact, investigate actual mixer/FX path and capture 2–4 voice CPU/rail measurements before hardware-test approval.
