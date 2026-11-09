# JIMBONES autonomous atomic task tree — 2026-11-08

**Purpose:** one small verifiable task per run. Always read this file and `docs/JIMBONES_WORK_QUEUE.md` first. Work in the existing feature branch or a dedicated isolated branch; never merge to main. User is testing v1.8.06 and says it is not nearly ready; bouncing skull is approved and must be preserved. Proposed v1.8.08 is a development target, not a release promise.

## Rules of execution
- Pick the first unblocked TODO; do **one task only** before reporting. Inspect actual source; never infer implementation from design docs.
- A task is DONE only with a pushed commit and evidence (exact paths, relevant code/CI result). A research task needs a precise file/line or symbol map and an explicit finding; a code task needs a passing focused host test and no unreviewed upstream code.
- On success, change TODO to DONE with commit SHA, date and evidence; update `docs/JIMBONES_WORK_QUEUE.md` and checkpoint, push, and notify user with task ID, result, commit and next task. If blocked, mark BLOCKED with specific unblock action; do not silently skip critical dependencies.
- Never copy LXR02Open's non-commercial code into the public GPL-only JIMBONES repo before resolving licensing; source reading and independently written code are allowed.
- **ABSOLUTE ARM HOLD:** no ARM compilation, firmware .img packaging, workflow re-enablement, main merge, or hardware-ready claim until the user explicitly approves the chosen feature-frozen candidate.
- Keep experimental Metallic FM disabled. Preserve original voices, 3 ms No Clip fade and user-approved bouncing skull.

## November 10 Codex handover deadline
- **By end of November 9 (UK time):** prioritise A01–A05, B01–B07 and evidence-backed D01–D02; finish compatibility mapping, licence decision brief, No Clip and skull preservation specifications, and a single proposed implementation architecture. Where possible prepare host-only fixtures/patches; do not pretend all tasks are done.
- **November 10:** provide Codex a handover document with exact branch and source SHAs, upstream references, accepted/excluded features, implementation order, test commands, known failures, unresolved decisions, and strict ARM approval gate. Codex may implement and test but must not ARM-build before explicit user approval.
- **Do not let deadline override correctness:** tasks C/D/E not genuinely completed must remain TODO/BLOCKED with precise instructions for Codex. User expects a more substantial improvement over v1.8.06, particularly wavetable and velocity control.
- **Handover deliverable:** `docs/CODEX_HANDOVER_2026-11-10.md` kept current as discoveries are made.

## Atomic task checklist (ordered; dependencies explicit)

### A — Baseline, fade, screensaver
- [ ] **A01** PARTIAL 2026-11-08: exact original source symbols, blob hashes and 132-sample/≈3ms call chain pinned in `docs/CODEX_HANDOVER_2026-11-10.md` from `jimbones-noclip-baseline`. **BLOCKER for DONE:** earliest introducing commit not established by available commit-message search; inspect Git file history/blame for `Core/Src/Soundengine/DSPAudio/RetriggerSmoother.c` and compare its parent revisions. No audio test run.
- [x] **A02** DONE 2026-11-08: source comparison of historical No Clip baseline against JB-26 v1.8.06-source lineage and current JB-28 shows byte-identical smoother `.c`/`.h`, enabled macro and unchanged three-drum trigger/process call sites; additional `jb_trigger` and `jb_preFilterNoise` calls in JB-26 may affect audio around smoother. No note-off handler exists in smoother; retrigger only on active envelope. Evidence: `docs/CODEX_HANDOVER_2026-11-10.md` A02 section; source comparison only, not audio verification.
- [ ] **A03** TEST ADDED / NOT RUN (2026-11-08): deterministic 132-sample fade host test in `tests/test_noclip_132.c`, wired to `tests/run_host.sh` (commits `3cb98a2`, `eb2f00a`). Covers fresh note, zero count, exact 132-sample ramp across 1-sample blocks, mid-ramp retrigger, saturation and independent instances. **DONE requires actual passing host GCC/Clang runs**; mixed 64/0/64/4/16 block boundary fixture added in commit `1ffba1a5` (NOT RUN: checkout unavailable due DNS). Further saturation edges and full execution pending; no hardware build.
- [x] **A04** DONE 2026-11-09 (source-only): pinned approved JB-26 skull glyph, 170ms animation, menu LCD latch/drain, nonblocking wake/repaint and existing C/Python host tests in `docs/JB_A04_SKULL_PRESERVATION_MAP.md` (commit `098ff972`). Host tests NOT RUN here; hardware image identity remains A05.
- [ ] **A05** Pin v1.8.06 candidate image/source identity if available in repo artifacts; otherwise mark hardware image SHA awaiting user-provided file, without guessing.

### B — Upstream source analysis
- [x] **B01** Pin LXR02Open `prime` HEAD commit, licence, oscillator and interpolation audit blob SHAs. DONE 2026-11-08; upstream commit `c896bfd0efe86a4037418bf9f46f1fcc39255958`; verified six blob SHAs and licence in `docs/JB_WAVETABLE_UPSTREAM_PORT_PLAN.md` (research evidence commit `a5c51dc2a60b25108ca8689593e81932c5629711`).
- [x] **B02** Trace upstream `modNode_updateValue` fractional waveform-ID write, global interpolation toggle and reset paths. DONE 2026-11-08; source evidence in `docs/JB_WAVETABLE_UPSTREAM_PORT_PLAN.md` (modulationNode.c blob e3a811b, lines 61–109, 173–194, 253–305).
- [x] **B03** Trace upstream `Oscillator.c` two-wave block rendering, sample/FM/noise/crash dispatch and fallback. DONE 2026-11-08; pinned oscillator blob 5c1bf056; evidence in `docs/JB_WAVETABLE_UPSTREAM_PORT_PLAN.md` (lines 98–155, 186–211, 229–253, 552–590).
- [x] **B04** DONE 2026-11-08 (source-only): mapped OscInfo ABI/layout, six primary waveform destinations, native ID 5 PWM vs upstream CRASH, native modulation dispatch and API mismatches. Evidence `docs/JB_B04_OSCILLATOR_COMPATIBILITY_MAP.md`, source commit `ed00f8c97fa952935115d6f14a38df94edd4bf27`. No tests run; exact numeric destination registry mapping remains for B05/B06.
- [x] **B05** DONE 2026-11-09 (source-only): upstream/native six velocity nodes, reset/CC dispatch, and JIMBONES existing velocity/blend semantics mapped. Proposed wave position first, shape second, drive deferred, default OFF/zero and versioned extension. Evidence `docs/JB_B05_VELOCITY_DESTINATION_MAP.md`. No host test run.
- [ ] **B06** PARTIAL 2026-11-09: native OscInfo ABI, 64-frame DMA, six velocity-node DTCM state, 44140 Hz timing, fixed JB kit byte contract and Metallic OFF verified in `docs/JB_B06_NATIVE_ABI_GATE.md` (source-only). Upstream sample memory, kit/global serialization, upstream MIDI/clock comparison and screen APIs still unverified; native MIDI/clock/NRPN mapped in `docs/JB_B06C_MIDI_CLOCK_COMPATIBILITY.md` (commit `59084b2f`); no host tests run. Do not mark DONE.
- [x] **B07** DONE 2026-11-09 (source-only): selected independently authored Erica-native extension for public GPL branch; rejected direct upstream copy because pinned noncommercial licence adds field-of-use restriction. Exact source/license evidence and gated alternatives in `docs/JB_B07_LICENCE_ARCHITECTURE_DECISION.md` (commit `8f6ea7de38a51006c037494cbdcd80b91a929938`). No tests or implementation run.

### C — Isolated host implementation
- [ ] **C01** Create a dedicated wavetable experiment branch from pinned integration source, preserving rollback.
- [ ] **C02** Write focused host fixture for native oscillator output with interpolation OFF and compare exact legacy audio.
- [ ] **C03** Implement bounded fractional waveform-ID modulation state in native oscillator (without source copying); test invalid IDs.
- [ ] **C04** Implement periodic waveform pair interpolation with stable phase and bounded output; compare stepped/smooth host renders.
- [ ] **C05** Decide and implement noise/crash/sample boundary policy from B03/B06; no unsupported claim of upstream parity.
- [ ] **C06** Wire LFO-to-waveform destination and state reset; test no stale interpolation on target changes.
- [ ] **C07** Wire velocity-to-wave-position with adjustable depth; test zero-depth exact bypass and velocity 0/64/127.
- [ ] **C08** Wire second approved velocity destination (e.g. shape or drive), bounded and default OFF; regression test.
- [ ] **C09** Add host audio samples/metrics for 3–4 concurrent overlapping hits, fast retriggers and No Clip fade.
- [ ] **C10** Measure worst-case CPU proxy / per-block render work and allocations; reject costly designs before ARM.
- [ ] **C11** Run focused plus full GCC/Clang host suites; fix real regressions with one commit per defect.

### D — Performance integration
- [x] **D01** DONE 2026-11-09: verified historical Comb PR #18 host CI on SHA `05e1722`, run `42512010361`: focused Comb, GCC/Clang full host, Step FX all SUCCESS. Artifact job failed because JB-23 manifest rejected version 1.8.07, **not** comb DSP. Exact log evidence `docs/JB_D01_COMB_CI_EVIDENCE.md` (commit `6351b76c`). Latest HEAD/hardware NOT VERIFIED; no new ARM run.
- [ ] **D02** Verify Step 1 LED and five dub timing toggles in source and tests; record outstanding hardware-only uncertainty.
- [ ] **D03** Implement per-slot Step FX voice masks in isolated host-tested patch; keep kick protected by default.
- [ ] **D04** Implement four-knob VOICE/RATE/DEPTH/CHAR UI after D03; verify persistence and backwards compatibility.
- [ ] **D05** Integrate approved skull code without LCD contention; run display host regressions.
- [ ] **D06** Integrate approved oscillator, Comb and Step FX patches into one candidate branch without merging unrelated donor history.
- [ ] **D07** Run full host GCC/Clang, focused audio and persistence tests on exact integrated SHA; record known gaps.

### E — Freeze and user-gated build
- [ ] **E01** Prepare candidate feature matrix, excluded features, performance risks and hardware test sheet for user review.
- [ ] **E02** Obtain explicit user approval for **feature freeze and ARM build**; until then STOP.
- [ ] **E03** Re-enable ARM job only after E02; compile exact frozen SHA and inspect flash/RAM/ITCM (>=512 bytes free).
- [ ] **E04** Validate .img updater header, binary consistency, checksums, manifest and rollback image; no automatic flash.
- [ ] **E05** Deliver labelled hardware-test candidate; user tests pad/KT-10/FD-8, overlaps, DSP/PK/LATE/RAIL, No Clip, wave velocity, Step FX and skull.
- [ ] **E06** Resolve user-reported hardware defects one at a time; re-freeze/re-approve before any subsequent ARM build.
- [ ] **E07** Publish final candidate only after explicit hardware acceptance.

## Completion notification template
`JIMBONES [ID] DONE — [one concrete result]. Evidence: [commit SHA, tests or exact source]. Next: [ID]. ARM remains held.`

## First next task
**A01** — source-trace the original 3 ms No Clip fade. Preliminary repository commit-message and default-branch code searches did not locate it; inspect older branch/file history and audio trigger/reset symbols. B02 source analysis is complete. Do not start C or E before A and B prerequisites.


## Feature-level implementation and sound-quality acceptance tree (added 2026-11-08)

**Scope and status:** These are additional acceptance criteria and future implementation tasks, **not completed claims**. Preserve ordered atomic A–E dependencies above. The Nov 10 18:30 UK handover is a documentation/host-validation target, not permission to ARM-build. Prioritize A/B/D blockers and existing deliverables; any new feature work not verified by then remains TODO for Codex. A feature is DONE only with source evidence, focused host tests, full integrated regression and measured sonic result; hardware-only checks remain pending until approved.

### F01 — Comb filter (linked to D01/D06/D07)
- [ ] F01.1 Inspect PR #18 comb topology, coefficients, buffer layout and current CI evidence; record exact source SHA.
- [ ] F01.2 Define tuning/delay, feedback, damping, polarity and wet/dry parameter bounds, UI placement and OFF defaults.
- [ ] F01.3 Host-test delay-buffer wrap, interpolation, parameter smoothing, silence, zero feedback and extreme resonance; reject runaway feedback, NaNs and denormals.
- [ ] F01.4 Render tuned ringing, resonant snare, pitched tom, clap and hi-hat reference audio with settings and hashes.
- [ ] F01.5 Stress 3–4 overlapping hits, fast sweeps and worst-case CPU/memory proxy; verify disabled path matches approved original.
- [ ] F01.6 Validate presets and full GCC/Clang integration; keep hardware audition pending explicit approval.

### F02 — Dub Step FX / send throws (linked to D02–D04/D06/D07)
- [ ] F02.1 Map existing five timing toggles, Step 1 LED, Step 16 morph shortcut and exact sequencer control semantics.
- [ ] F02.2 Design per-slot voice mask and FX send matrix, with kick protected by default; specify dry, send, return, feedback and filtering.
- [ ] F02.3 Define tempo sync including dotted/triplet divisions, 2/4/8-bar repeats, throw latch/momentary behaviour and tempo-change policy.
- [ ] F02.4 Host-test independent snare/clap/tom/hat sends, step-boundary and bar-boundary echo tails, rapid toggles and simultaneous throws.
- [ ] F02.5 Test feedback bounds, zero/maximum sends, delay buffer wrap, silence, preset reload and legacy kit defaults.
- [ ] F02.6 Verify VOICE/RATE/DEPTH/CHAR UI, Step 1–16 LEDs and no unintended kick send; run integrated host regressions.

### F03 — Native wavetable and audible sound tests (linked to B04–B07/C01–C11)
- [ ] F03.1 Finish licence-compliant native architecture mapping; do not transplant incompatible upstream code.
- [ ] F03.2 Pin native wave IDs, interpolation mode, phase, wrap/boundary and reset policy; handle noise/crash/sample separately.
- [ ] F03.3 Specify per-voice enable, waveform position, LFO scanning, velocity blend/depth and default OFF.
- [ ] F03.4 Host-render clean, aggressive, noisy, evolving and percussive test patches with settings, reference WAVs and hashes.
- [ ] F03.5 Compare stepped versus interpolated scanning and velocity 0/64/127 at matched loudness; document whether sonic change justifies cost.
- [ ] F03.6 Measure discontinuities, clipping, aliasing proxy, 3–4 concurrent long-decay hits, rapid retriggers and No Clip interactions.
- [ ] F03.7 Confirm zero-depth/default OFF bit-identical legacy output, preset persistence, CPU proxy and integrated GCC/Clang suite.

### F04 — Morph, modulation and velocity (linked to B05/C06–C08)
- [ ] F04.1 Map velocity to wavetable position plus one approved second destination, with bounded depths and OFF defaults.
- [ ] F04.2 Design per-voice morph inclusion/exclusion, protected snapshot A and B/C editing at 50% morph.
- [ ] F04.3 Test LFO target switching/reset, rapid knob changes, zero/mid/max velocity, MIDI ramps and stale-state prevention.
- [ ] F04.4 Verify UI edit indicators, preset round trips and CPU budget; keep unimplemented controls labelled future work.

### F05 — Filters, drive and routing (future unless already approved)
- [ ] F05.1 Audit original filter, comb, per-voice drive and FX send order with exact paths.
- [ ] F05.2 Specify optional movable bass HPF/LPF and tape/tube-style saturation with low-end preservation.
- [ ] F05.3 Test cutoff/resonance extremes, velocity drive, headroom and bypass exactness; do not silently replace legacy sound.
- [ ] F05.4 Defer implementation unless dependencies and verified core features are complete.

### F06 — UI, skull, save and regression gates (linked to A03–A05/D05/D07/E01)
- [ ] F06.1 Pin v1.8.06 bouncing skull assets, LCD update/wake timing and host display tests; preserve unchanged.
- [ ] F06.2 Verify OSC/FILTER double-press navigation, Step 1 LED, Step 16 shortcut, page contention and live playback.
- [ ] F06.3 Test new parameter bounds, defaults, legacy kit migration, save/reload and interrupted-save simulation.
- [ ] F06.4 Run deterministic 132-sample No Clip tests, block splits, int16 saturation, repeated retriggers and three-drum independence.
- [ ] F06.5 Run exact integrated SHA through host GCC/Clang, full-kit stress, NaN/full-scale/discontinuity checks and CPU work proxy.
- [ ] F06.6 Publish feature matrix: implemented, host-verified, hardware-unverified, blocked and deferred; record every known defect.
- [ ] F06.7 Prepare Codex integration order, build commands **for later approved execution only**, acceptance sheet and rollback.

**Feature completion scoring:** Track design, implementation, focused audio evidence, integrated regression and handover separately. Never equate checked design tasks with functional or hardware-accepted implementation. No Metallic FM, ARM compilation, .img packaging, ARM workflow re-enablement or merge to main before explicit approval.


## G — Engineering success gates (user-approved 2026-11-08; all new work TODO)

**Execution policy:** These gates support, not displace, the A–F dependency order and one-verifiable-task-per-run rule. A new checklist is not an implemented safeguard until its own acceptance evidence is committed. No release, ARM build, image packaging or main merge is authorized.

### G01 — Source integration manifest and dependency graph (CRITICAL)
- [ ] G01.1 Pin exact branch head, v1.8.06 baseline source/image provenance (mark unknown image SHA), PR #18 comb commit, JB-27 oscillator donor, Step FX donor and skull source refs.
- [ ] G01.2 Record per-feature donor SHA, destination files, dependencies, conflicts, licence status, integration order and rollback SHA in a versioned manifest.
- [ ] G01.3 Validate every referenced ref/path against repository; fail closed for missing refs, incompatible licences or unreviewed code.
- [ ] G01.4 Integrate only isolated, host-tested changes; never merge unrelated donor histories.

### G02 — DSP, memory and worst-case performance budget (CRITICAL)
- [ ] G02.1 Capture baseline render cost and RAM/buffer use; record platform sample rate, block size and available budget.
- [ ] G02.2 Estimate/measure incremental wavetable, comb and dub-send work independently and combined; include 4+ overlapping long-tail hits.
- [ ] G02.3 Use <=75% sustained DSP as a **provisional hardware target**, not a proven limit; require zero LATE, no audible crunch and zero unexpected clipping in acceptance.
- [ ] G02.4 Investigate RAIL growth under sequencer playback with source-defined counter semantics; do not equate count with audible clipping.
- [ ] G02.5 Record worst-case memory/stack and ARM ITCM >=512 B free only after explicitly approved ARM build; mark hardware-only checks pending.

### G03 — Golden audio corpus and objective/subjective sound gates (CRITICAL)
- [ ] G03.1 Capture immutable legacy OFF-path WAVs and SHA256 hashes for six voices, No Clip overlap, simultaneous hits and approved v1.8.06 sound.
- [ ] G03.2 Build deterministic reference patch/settings manifests for comb, dub throws, wavetable stepped/smooth and velocity 0/64/127.
- [ ] G03.3 Compare OFF-path PCM exactly where promised; measure peaks, discontinuities, full-scale counts, silence, output level and render determinism.
- [ ] G03.4 Perform matched-level A/B listening for sonic usefulness; document reject/rework decisions, not just compilation.

### G04 — Automated host CI gates (CRITICAL)
- [ ] G04.1 Add host-only GCC and Clang jobs for focused Comb, Step FX, wavetable, No Clip, skull and preset tests on exact integration SHA.
- [ ] G04.2 Add regression fixture hashes, deterministic seeds, compiler warnings, sanitizers where supported and clear failure logs.
- [ ] G04.3 Require focused plus full suite green after each integration; record exact run URLs and SHAs; keep ARM job disabled.
- [ ] G04.4 Prevent stale version assertions and silent test skips; report test coverage gaps.

### G05 — Defect register and regression prevention (CRITICAL)
- [ ] G05.1 Create versioned defect register with severity, exact reproduction, expected/actual result, affected SHA, owner, fix SHA and regression evidence.
- [ ] G05.2 Seed known observations: overlapping modulated tom crunch, Step 1 LED, sequencer-only RAIL growth, preset save/recovery failures and any current CI failures; distinguish fixed from unverified.
- [ ] G05.3 Block handover-ready status for known unresolved critical/high defects; log hardware-only unknowns separately.
- [ ] G05.4 Add a focused regression for every fixed defect and rerun full host suite.

### G06 — Codex handover readiness audit (CRITICAL)
- [ ] G06.1 Produce a machine-readable and human-readable source/feature matrix with exact SHAs, licences, tests and statuses.
- [ ] G06.2 Verify integration order, host commands, acceptance criteria, rollback, open defects and explicit ARM hold are all documented.
- [ ] G06.3 Distinguish host-ready, Codex-build-ready (pending user approval), hardware-unverified and released; never claim bug-free.
- [ ] G06.4 At Nov 10 18:30 UK provide truthful completed/blocked/deferred list and handover SHA.

### G07 — Preset migration and recovery (HIGH)
- [ ] G07.1 Define backward-compatible defaults for all new controls; verify old kits reproduce original sound.
- [ ] G07.2 Host-test save/reload, invalid ranges, truncated files, full SD, interrupted write and rollback/recovery.
- [ ] G07.3 Keep hardware power-loss/card-disconnect verification pending approved image.

### G08 — Hardware test protocol and diagnostic capture (HIGH)
- [ ] G08.1 Publish reproducible Alesis/KT-10/FD-8 pad and sequencer test matrix: velocity, overlap, tom switches, comb, sends, wave, skull.
- [ ] G08.2 Capture DSP/PK/LATE/RAIL before/after each test with firmware SHA, preset, tempo, duration and audio recording.
- [ ] G08.3 Define pass/fail and rollback triggers; require user hardware acceptance after approved build.

### G09 — Future feature feasibility queue (DEFER until A–G core complete)
- [ ] G09.1 Rank optional filters, drive, morph and additional routing by audible benefit, CPU/memory cost, UI/persistence complexity and licensing.
- [ ] G09.2 Produce modular drop-in briefs with host-only tests and integration points; no unapproved hardware claims.

**Mandatory Nov 10 handover audit:** G01–G06 must be either evidence-backed DONE or explicitly BLOCKED/DEFERRED with reasons; no checkbox inflation. G07–G08 can remain hardware-pending; G09 must not displace core work.


## H — LXR02Open source and sound reference investigation (2026-11-08)

**Reference:** https://github.com/brendanclarke/LXR02Open (pin source commit `c896bfd0efe86a4037418bf9f46f1fcc39255958` before comparison); developer demonstration https://www.youtube.com/watch?v=pubgafd-DEk ; Reddit author screenshot confirms waveform interpolation, sample loading changes and MIDI morph. Upstream README documents two dynamically allocated oscillator interpolation slots, main oscillators only (not FM), CPU monitor, asynchronous SD and 0.37-format compatibility within upstream. These are **upstream behaviours**, not JIMBONES functionality. Do not assume compatibility with Erica kits or copy licence-incompatible source.

- [x] **H01 — Oscillator source map (P0; B04/B07 prerequisite).** SOURCE-AUDIT DONE 2026-11-08; pinned blob/line map, two-slot priority, render paths, sample cache and risk cases in `docs/JB_LXR02OPEN_H01_SOURCE_MAP.md` (commit `f217630`). This is NOT a tested JIMBONES wavetable implementation.  Pin exact upstream source blobs and licence; trace waveform selection, phase accumulation, adjacent-wave mixing, interpolation slot allocation/eviction, noise/sample/FM exclusions and parameter scaling. Output path/line map and native-vs-upstream differences; verify with focused source assertions.
- [x] **H02 — Sample loading/memory (P0).** UPSTREAM SOURCE AUDIT DONE (commit `d60c2fe`, `docs/JB_LXR02OPEN_H02_SAMPLE_MEMORY_AUDIT.md`); memory layout, cache generations, install/append, async SD and flash-stall hazards documented. JIMBONES implementation and fault-injection tests remain TODO in G07/H06.  Trace upstream flash sample table, 120-entry/1.5MB limits, one-shot/loop behaviour, SD async queue and audio-thread safety; compare with JIMBONES storage, RAM/flash budget and existing save failure cases. No wholesale SD replacement. Output memory budget and fault-injection test plan.
- [ ] **H03 — MIDI morph and velocity routing (P0; B05/F04).** Trace upstream MIDI CC/NRPN to morph, LFO modulation writes, value shaping, per-voice state and preset serialization. Produce exact destination mapping, bounds and zero-depth bypass test fixtures; do not silently alter legacy controls.
- [ ] **H04 — Reference audio fixtures (P0; F03/G03).** Define and host-render stepped and smooth wave sweeps, rapid retriggers, velocity 0/64/127, low/mid/high pitch, all original voice classes and four overlapping long-decay hits. Compare deterministic PCM, clicks, peaks, full-scale samples and level-matched listening. Record settings, compiler, SHA and hashes; video is a qualitative goal only until its audio is actually reviewed.
- [ ] **H05 — Native wavetable prototype (P0; C01–C11).** Implement a separate licence-compliant host-only prototype reproducing selected upstream *behaviour*, with two-slot cost measured against scalable alternatives; disabled output must remain bit-identical. Benchmark CPU and memory; reject inaudible or excessive-cost changes.
- [ ] **H06 — Integration gates (P0; D06/D07/G04).** Wire deterministic H01–H05 focused tests into host runner and GCC/Clang CI, sanitizers where possible; verify no regression in No Clip, Step FX, comb, skull, legacy kits or SD saves. Mark PASS only with actual CI run and source SHA.
- [ ] **H07 — Handover and hardware hold (P0; E01/G06).** Publish source mapping, legal decision, sonic A/B, DSP budget, outstanding risks and exact candidate SHA by Nov 10 18:30 UK; no ARM build, firmware image or hardware-ready claims without explicit approval.

**Start now:** H01 source mapping and H02/H03 investigation can proceed in parallel with A03 host-test validation, but no new implementation should bypass A/B gates. Completion is evidence-based; incomplete tasks remain TODO/BLOCKED at deadline.


## I — User-loaded custom transient samples (requested 2026-11-08)

**Goal:** Allow a user to import short custom one-shot attack/transient WAVs (for example, stick, click, snap, clap or acoustic drum attack) and layer them over any of the six **unchanged original LXR-02 voices**. Preserve the original voice when transient layer is OFF. **Priority: HIGH after A–H stability gates; prototype host-only before ARM approval.** Do not repurpose or erase upstream LXR02Open sample flash sectors without linker/storage review.

- [ ] **I01 — Import and format contract.** Define supported mono PCM WAV bit depth/sample rate, max duration/size, filename/index limits and deterministic resampling/conversion on Mac; reject malformed, oversized and unsupported files safely. Decide SD streaming vs preloaded memory using measured latency/budget; avoid SD access from audio ISR.
- [ ] **I02 — Sample library and safe storage.** Design SD-card folder/manifest, per-kit sample references, validation, missing-file fallback, duplicate names and versioning; use staged/atomic writes and interrupted-save recovery. Preserve existing presets and user data.
- [ ] **I03 — Transient playback engine.** Implement bounded one-shot PCM voice with configurable start offset, gain, attack/decay, pitch/tune, polarity and retrigger/choke behaviour; deterministic sample-accurate trigger aligned with original voice. Zero allocation and blocking I/O in audio callback.
- [ ] **I04 — Per-voice layering and modulation.** Independent ON/OFF, sample selection, mix and velocity-to-transient amount on kick, snare, toms, clap and hi-hat. Optional transient-only filtering and random sample selection with controlled probability; default OFF, no original-engine changes.
- [ ] **I05 — UI, MIDI and persistence.** Map a compact additional edit page (no loss of existing OSC/Step FX controls); save/load per-kit sample ID, gain, tuning, envelope, velocity depth and enable state. Gracefully handle missing samples and old kits.
- [ ] **I06 — Focused host regression tests.** Verify exact OFF-path PCM, onset alignment, clipping/headroom, click-free retriggers, high-velocity extremes, malformed/empty WAVs, missing SD samples, 3–4 overlapping long decays, sample switching, memory bounds and no-alloc audio path. Compare rendered WAVs and CPU/RAM against G02/G03 gates.
- [ ] **I07 — Integration and handover.** Integrate only after wavetable/comb/Step FX/No Clip host regressions pass; GCC/Clang/sanitizer CI evidence, documented flash/RAM and licensing audit, explicit rollback and hardware test matrix. Remains TODO/DEFERRED at Nov 10 handover if unverified; **no ARM build or flashable image without user approval**.

**Acceptance:** A user can add a custom transient to a drum, play expressively with velocity, save/reload a kit and disable the layer to recover original audio bit-for-bit, without audible crunch or unsafe DSP/SD behaviour.


### J — LXR-02 native/browser emulator integration (HIGH priority, host-only)
Reference: https://github.com/bg1eym/lxr-desktop-app/tree/emu — unofficial firmware-v1.7 emulator. Read `emu/README.md` and pin an exact upstream commit before any port. Its native CLI renders WAV, `ctest` runs host tests, and its browser build runs firmware C through WebAssembly/AudioWorklet with virtual sequencer, controls and FatFs SD. This is an integration candidate, **not** yet a validated JIMBONES emulator. Prioritise after preserving baseline and before extensive new DSP feature integration; do not displace the November 10 handover gates.
- [ ] **J01 — Pin and audit emulator upstream:** exact commit, GPL/provenance, native build graph, firmware source assumptions, shim boundaries, timing and float differences; compare with JIMBONES source. Deliver compatibility/diff map and licence decision, without copying unreviewed code.
- [ ] **J02 — Host-only baseline:** establish isolated native emulator build for pinned stock v1.7; run documented `cmake -S emu -B build/emu -DCMAKE_C_COMPILER=clang`, `cmake --build build/emu`, `build/emu/lxr_cli render --demo --out demo.wav`, `ctest --test-dir build/emu`; record environment, results, WAV checksum and failures. Do not mark PASS until actually executed.
- [ ] **J03 — JIMBONES compatibility port:** isolate emulator adapter from firmware changes, integrate existing JIMBONES audio engine and sequencer while retaining six original voices, optional features OFF identity, 132-sample No Clip and skull behaviour; do not accidentally re-enable Metallic FM. Host-only; review every shim and memory assumption.
- [ ] **J04 — Reproducible tom-overlap stress corpus:** simulate FD-8/Ableton alternating Tom 1/2, 3–4 concurrent voices, long overlapping tails, repeated fast retriggers and modulation combinations; export deterministic WAV and metadata. Assert no unexpected full-scale runs/discontinuities and preserve known-good reference renders.
- [ ] **J05 — Automated feature/regression suite:** waveform scan and velocity sweeps, comb and dub Step FX timing, Step 1 LED/Step 16 morph, per-voice FX exclusion, transient-sample import/retrigger, preset/SD corruption recovery, OFF-path bit identity. Run GCC/Clang where supported, sanitizers and golden-audio comparisons; distinguish missing features from failures.
- [ ] **J06 — CI and evidence artifacts:** add host-only GitHub Actions workflow (never ARM), run native emulator and test matrix on pushes/PRs, upload WAV, logs, checksums, crash traces and concise pass/fail report. Gate feature integration on verified runs; no fake success claims.
- [ ] **J07 — Browser front end (optional after CLI stability):** build WebAssembly/AudioWorklet and virtual controls from validated JIMBONES host sources, test browser sequencer/SD workflows and screenshots; ensure users can audition new features without firmware flashing. Not a prerequisite for CLI regression speed.
- [ ] **J08 — Hardware correlation and DSP limits:** compare selected emulator renders against recorded LXR-02 output, log real DSP/LATE/RAIL under sequencer playback and reconcile differences. Browser/desktop CPU usage is **not** STM32 DSP headroom. Only user-approved firmware candidates may proceed past the absolute ARM hold.
**Acceptance:** deterministic host-only JIMBONES render and automated overlap regression with published execution evidence, without altering firmware semantics. **Current state: QUEUED / NOT IMPLEMENTED / NOT RUN.**


### K — Reproduce historical JIMBONES firmware builds in emulator (HIGH priority; depends on J01–J03)
Use **GitHub source history**, not user-uploaded `.img` files: source branches already present include `agents/jb-03-dsp-stress-v184`, `agents/jb-23-hardware-test-pack-v184`, `agents/jb-28-full-integration-v186`, and `agents/jb-27-native-wave-scan-v187`. A hardware `.img` is not directly executable by the host emulator. Pin the exact source commit that corresponds to each tested image; do not assume branch names establish image identity.
- [ ] **K01 — Historical candidate inventory:** inspect commits, tags, releases, CI artifacts and source diffs for the crunchy Metallic-enabled candidate, v1.8.06 baseline and the later Metallic-removed candidate. Record source SHA, matching image SHA if provable, enabled feature flags, and confidence; mark unknown matches UNKNOWN.
- [ ] **K02 — Isolated host integration:** adapt each selected historical source tree to the pinned `bg1eym/lxr-desktop-app` `emu` native platform (see J01–J03), without rewriting historical DSP behavior or lifting the ARM hold. Keep baseline, experimental and Metallic-OFF candidates isolated.
- [ ] **K03 — Deterministic comparison scenario:** alternate Tom 1/Tom 2 with modulated layers as in FD-8/Ableton Chain Selector, include long decays, rapid retriggers, 3–4 simultaneous hits, sequencer running vs stopped, 40–100% layer settings; render WAV plus event trace for each candidate.
- [ ] **K04 — Evidence-based findings:** measure peak dBFS, full-scale sample counts, discontinuity/click metrics, timing, reproducibility and golden-render differences. Separately report emulator-only limitations; never equate desktop CPU to STM32 DSP, or claim the simulator reproduces hardware RAIL without validation.
- [ ] **K05 — Regression gate:** turn any confirmed historic crunch/retrigger reproduction into a repeatable host test, run native `ctest` plus relevant JIMBONES GCC/Clang tests and publish logs/WAV/checksums in CI; mark PASS only with actual execution evidence.
**Acceptance:** at least one verified historical source candidate rendered under the emulator, an independently repeatable overlapping-tom test, and a concise comparison with measured results. **Current status: QUEUED / NOT RUN.** No source upload required from user; source history is in GitHub.


- [ ] **B06** PARTIAL 2026-11-09: B06b native sample-dispatch audit committed `3cbb1fb2`: live non-FM oscillator handles IDs 0–5; default sample path commented out; sample playback helpers are `#if 0`; crash asset existence is not active playback. Evidence `docs/JB_B06B_SAMPLE_DISPATCH_GATE.md`. Upstream memory, serialization, MIDI/clock and display still OPEN; tests NOT RUN. This note supplements existing B06 status.


### L — Sequencer Step 1 LED and tempo/autosave reliability (user-reported, HIGH priority)
**Reported 2026-11-09:** When sequencer Step 1 is armed, its LED does not illuminate; steps 2–16 behave normally. Current tempo returns to 120 BPM after restart/kit change. Investigate autosave and whether tempo should belong to kit, pattern or project; do not assume current storage semantics. Keep Step FX Step 1 on/off toggle separate from sequencer Step 1 LED.
- [ ] **L01 — Sequencer Step 1 LED defect:** trace LED bitmask, step-index conversion (0-based vs 1-based), UI mode priority and refresh code; reproduce with Step 1 ON/OFF and steps 2/16 controls, playing/stopped, across pattern changes and Step FX mode. Add deterministic host test and minimal fix, preserve approved skull/wake repaint and all other step LEDs.
- [ ] **L02 — Autosave behaviour audit:** map kit, pattern, project, globals and temporary working-state save paths; debounce/delayed write, power-loss/SD-full handling, recovery/rollback and unsaved edit semantics. Produce source-backed recommendation for safe opt-in autosave and deterministic fault-injection tests; no blocking SD writes from audio callback.
- [ ] **L03 — Tempo persistence decision and implementation:** trace 120 BPM reset, BPM owner (project/pattern/global/kit), load/save ordering and external MIDI sync. User requests saving tempo with kit; first check format/compatibility and decide whether kit-associated tempo metadata or project-level persistence is safer. Ensure a saved kit recalls intended BPM without surprising global tempo overrides, unless user explicitly opts into kit tempo recall. Test reboot, kit/project switching, 60/120/180 BPM, missing/corrupt files, backward compatibility and external sync; commit implementation only after source audit and passing host tests.
**Acceptance:** armed sequencer Step 1 LED reflects state; documented safe autosave semantics; tempo is recalled predictably after reboot and kit loading, with backwards-compatible storage and explicit sync precedence. **Status: QUEUED / NOT IMPLEMENTED / NOT TESTED.** ARM hold unchanged.

- **D02 partial source audit 2026-11-09:** Step 1 index maps correctly to LED_STEP1, but Performance/CPU branches can override sequencer LEDs; five dub timings and existing host fixtures mapped. Cause of ordinary sequencer Step 1 fault NOT CONFIRMED; tests NOT RUN. See `docs/JB_D02_STEP1_SOURCE_AUDIT.md` commit `532ddf29`. D02 remains OPEN.

2026-11-09 D02/L01 candidate fix: `Core/Src/buttonHandler.c` now mirrors both ON and OFF sequencer step LED state after toggle, including step index zero (`c9317850`). Added `tests/test_seq_step_led_source.py` (`a604f735`) and wired it into host runner (`5772dc84`). Source guard is NOT RUN; does not prove hardware Step 1 fault resolved. Need executable behavioral LED test, GCC/Clang CI and hardware check; keep D02/L01 OPEN and ARM HOLD.

L03 partial (2026-11-09): source audit identified two 120 BPM initial defaults in `sequencer.c:57` and `menu.c:767`, with `SET_BPM` runtime routing in `menu.c:3728`. Persistence owner and startup load order still unknown. Evidence `docs/JB_L03_TEMPO_SOURCE_AUDIT.md` commit `17d7143c`; NOT RUN; L03 OPEN; ARM HOLD.

**H03 2026-11-09 PARTIAL source audit:** upstream morph MIDI 0..127 maps to 0..254 via `value<<1`, with six LFO and velocity amount CC routes, original-value reset and per-block interpolation mapping. Pinned blob evidence and host-only test matrix: `docs/JB_H03_MORPH_VELOCITY_ROUTE.md`, commit `30dc4e77`. Native morph precedence and kit/global serialization still UNVERIFIED; tests NOT RUN; H03 OPEN. ARM HOLD.

**H03 2026-11-09 native MIDI contract check:** exact GitHub source blob `04f3d03ffcee0b20f6d58e26d7a91f34e4b89bca` inspected: native `CC_MOD_WHEEL` branch is **no-op**, unlike upstream `value<<1` morph; six velocity amount CC cases dispatch to `velocityModulators[]` using normalized value. Source assertions executed during inspection (both TRUE). Committed executable Python source guard `tests/test_h03_native_midi_contract.py` (`26ff895b`) and host runner wiring (`32fce496`). Python test itself/full GCC-Clang suite **NOT RUN**; no local checkout. This is an important upstream/native compatibility difference; no morph behavior change implemented. H03 remains PARTIAL, ARM HOLD.

**H03 native morph safety guard 2026-11-09:** Added executable source contract `tests/test_h03_native_morph_contract.py` (`356a0a21`) and wired into `tests/run_host.sh` (`04275031`). It checks alternate morph kit buffer load, morph destination bookkeeping, source-kit audio-routing exclusion and existing velocity nodes, based on `Core/Src/menu/menu.c` blob `6282d73b`. Test NOT RUN (no local checkout); no audio/preset round-trip proof. H03 PARTIAL; next inspect `preset_morph` implementation/serialization and execute GCC/Clang host tests. ARM HOLD.

**H03 2026-11-09 morph slot contract:** Inspected `Core/Src/menu/menu.c` blob `6282d73b`: morph save uses `preset_saveDrumsetRam(...,1)`; morph load uses `preset_loadDrumsetRam(...,1,0)`, updates `PAR_MORPH_KIT_DEST` and `CURRENT_MORPH`, and morph rendering excludes audio-routing parameters. Added executable source guard `tests/test_h03_morph_slot_contract.py` (`21b4be75`, anchor correction `999d4e0f`) and wired into host runner (`eb916590`). Equivalent source predicates checked TRUE during connector inspection; committed Python test/GCC/Clang NOT RUN. Actual serializer body, round-trip, corruption and hardware remain unverified. H03 PARTIAL; ARM HOLD.

**H03 2026-11-09 morph RAM/SD route guard:** `Core/Src/menu/menu.c` morph save case calls `preset_saveDrumsetRam(...,1)` and contains no direct SD save; morph load calls `preset_loadDrumsetRam(...,1,0)` and updates destination/current morph. Added executable source guard `tests/test_h03_morph_ram_route.py` (`76df5649`, corrected case selection `c7a2b00b`) and wired `tests/run_host.sh` (`f2d32455`). Important: **this does NOT establish power-cycle persistence or prove that the serializer cannot write indirectly**. Python test/GCC/Clang NOT RUN; H03 OPEN; next inspect actual preset implementation and add round-trip fixture. ARM HOLD.

**H03 2026-11-09 native kit morph-save isolation (candidate FIX, NOT RUN):** `Core/Src/Preset/presetManager.c` blob `1b91b342` unconditionally copied `jb_parameters` into `jb_kits[presetNr]` at entry to `preset_saveDrumsetRam`, even when `isMorph=1`. That could overwrite the saved native extension of the morph target. Commit `8f37e730` moves the copy into the non-morph branch, preserving normal kit saves and morph parameter interpolation. Added focused source guard `tests/test_h03_morph_kit_extension_guard.py` (`83ffbc64`, correction `a3ac1ce2`) and wired into `tests/run_host.sh` (`3c7473c8`). Post-change connector source inspection confirms exactly one extension memcpy in the non-morph branch; **Python test and full GCC/Clang suite NOT RUN**, no full checkout. Need behavioral two-slot RAM round-trip, SD serialization audit and CI before marking H03 DONE. Six voices, 3ms No Clip, skull untouched; ARM HOLD.

**H03 2026-11-09 SD extension contract (PARTIAL, NOT RUN):** inspected `Core/Src/Preset/presetManager.c` SD save/load paths: normal project loop calls `preset_saveDrumsetSD(i,0)` and load `preset_loadDrumsetSD(i,0)`; normal save packs `jb_kits[presetNr]` and normal load unpacks it. Morph SD save, if invoked, writes a packed *live* `jb_parameters` extension but morph SD load explicitly skips extension; asymmetry requires policy review, not proof of corruption in normal project flow. Added executable source guard `tests/test_h03_sd_extension_contract.py` (commits `76defea5`, `832038f8`), wired to host runner (`9bec28a9`). Checkout failed DNS resolution (`Could not resolve host: github.com`), so Python/GCC/Clang **NOT RUN**. Need actual two-slot RAM/SD round-trip, legacy record truncation and failure injection; H03 OPEN; ARM HOLD.

**A03 2026-11-09 additional executable edge fixture (NOT RUN):** `tests/test_noclip_132.c` now exercises both full-scale opposite-polarity retriggers (-32768 to +32767 and reverse), first-sample continuity, final ramp endpoint within one LSB, fade completion and post-fade exact output. Commit `fdbcd18620f46cda4d43586d3ca33d662384ab4f`. Existing `tests/run_host.sh` already invokes this test; no runner change needed. Native checkout via git failed DNS (`Could not resolve host: github.com`), so GCC/Clang execution NOT RUN and A03 remains OPEN. Source is host-only; no ARM build/image/merge. Next run GCC/Clang host suite on exact SHA and record output.

**A03 2026-11-09 host CI execution gate (NOT RUN / pending CI):** Standalone `tests/run_noclip_132.sh` (`3d94a75f`) compiles only `tests/test_noclip_132.c` + `RetriggerSmoother.c` with ASan/UBSan. Scoped GCC/Clang workflow `.github/workflows/jb-a03-noclip-host.yml` (`6ad6f947`) triggers on A03 file changes and supports manual dispatch. No ARM jobs or image packaging. GitHub combined status query at runner commit returned no statuses; do not infer PASS. A03 remains OPEN until both CI jobs are observed green with exact commit SHA and logs, then check full host suite and hardware-only behavior.

**A03 2026-11-09 zero-frame pending edge (NOT RUN):** Added `test_pending_survives_zero_frame()` to `tests/test_noclip_132.c` (commit `4131df513c1ae381d6f400ecd470222fe354ce2b`), asserting zero-frame processing preserves pending retrigger, remaining countdown and buffer, followed by exact first-sample continuity. Already included by standalone and full host runners; no ARM changes. Local GitHub checkout attempted again but DNS failed (`Could not resolve host: github.com`); no GCC/Clang execution or CI PASS verified. A03 OPEN; execute exact-HEAD isolated and full host suites, then inspect logs.


**2026-11-09 H03 CI guard repair (PENDING rerun):** PR #18 run [42663528291](https://github.com/balfourlynn1-byte/lxr-desktop-app/actions/runs/42663528291) checked out merge SHA `bebb4a6c`; Comb focused test passed, GCC host runner stopped at `tests/test_h03_native_morph_contract.py` because its regex required `{` immediately followed by `morph = 0`, but native `Core/Src/menu/menu.c` has a legitimate explanatory comment between them. Updated guard to permit comments without crossing braces in commit `3cd0f13845f4c04f366122cbacb061ddb1d2d928`. This is a source-guard correction, **not a DSP fix**. CI has **not been rerun**; H03 and A03 remain OPEN; Clang skipped, no hardware verification. ARM HOLD unchanged.

### Wavetable experiment checkpoint — 2026-11-09 (C02 PARTIAL)
- C01 isolated branch `agents/jb-wavetable-host-experiment` created from `bb7d5bd8dd5cd3dea57fc776241770f19c83c0ab`, with C01 notes committed at `20b34317525e789927cf2b98582b8016c5e6d0e6`.
- C02 standalone four-frame sweep fixture committed `3823de5ff81a804eb69a5952e8dda32b63a7c1ec`, **locally executed and PASS**: 22070 mono PCM16 frames at 44140 Hz; SHA-256 `20396bc4f995544882b27bb49b0b68f001a3093b2822012818062bd93b41a94c`. Workflow committed `614f2ead51c0ba554605e5039917417900e987a0` but no Actions run verified.
- **Do not check C02 as DONE:** fixture is independent Python, not original native oscillator legacy OFF bit-identical regression. Native DSP, LFO/velocity, emulator and CPU remain NOT TESTED. Next: exact native OFF fixture with GCC/Clang before any integration claim.

### 2026-11-09 — C03 native wavetable kernel, isolated host PASS
- Independently authored `Core/Inc/Soundengine/DSPAudio/JimbonesNativeWaveScan.h` committed `4d1023994207c3237079b5d1a619437845eff61e`; test `tests/test_c03_native_wave_scan.c` committed `e21c5d99816f802ed8c5f6d6f01883fb144df9db` and NAN fix `c2ca319ccece723efed393c96034c614af81175b`; runner `tests/run_c03_native_wave_scan.sh` committed `b6dbe664bf98f6de66db7602ac7306af5cb9e5a0`.
- Local focused GCC and Clang C11 `-O2 -Wall -Wextra -Werror -fsanitize=undefined,float-cast-overflow` checks PASS for scan position, interpolation, clamping and NaN. Executed an equivalent locally reconstructed test source/header; exact GitHub runner not independently run. No native oscillator dispatch connection or legacy OFF comparison yet; no emulator test. Next task is guarded integration with OFF bit-identity tests, preserving all original voices/No Clip/skull. No ARM build.

### 2026-11-09 — C04 velocity-to-wave-scan DSP connection
- Added opt-in `jb_wave_velocity_position` and `jb_wave_scan_velocity_sample` to `Core/Inc/Soundengine/DSPAudio/JimbonesNativeWaveScan.h`, commit `924bf73facc7028df7fd9149bda5e4dfea696ae5`; zero velocity depth leaves base scan position unchanged, positive/negative depth and clamping supported. No OscInfo ABI, waveform IDs, native dispatch or kit serialization changed.
- `tests/test_c04_wave_velocity.c` commit `f6ebf6f2a3c0558bc7e9d633e8cf4d2aeee1c874`; local GCC and Clang sanitizer host tests PASS using reconstructed equivalent C source: velocities 20,50,80,110,127 monotonic, zero-depth sample bypass, bounded extremes. CI workflow `.github/workflows/jb-c04-wave-velocity.yml` committed `dea5fbff6f3a5565f0ef7044705e3d5057fddcaa`; remote CI not yet verified.
- **C04 PARTIAL, not hardware-connected:** captured MIDI note-on velocity has NOT been threaded from parser/voice trigger to the scanner, scanner is NOT invoked by native oscillator dispatch, and actual audio/CPU/browser/hardware tests remain OPEN. Next task: trace native MIDI velocity through voice trigger and add opt-in voice-local routing, with OFF-path bit-identical regression first. No ARM build.

### 2026-11-09 — C05 native oscillator scan entry point (SOURCE PATCHED, ENGINE UNTESTED)
- Added opt-in `jb_calcNativeWaveScanBlock` declaration to `Core/Inc/Soundengine/DSPAudio/Oscillator.h` commit `8d8a1a2e3220babd444d2dda90d00bdf1b37dcbc`, and implementation to `Core/Src/Soundengine/DSPAudio/Oscillator.c` commit `8bbad59c62ec776a508f3207b3d44cf372fca01d`. Function uses existing `OscInfo.phase/phaseInc`, four-frame scanner, note-on velocity and signed LFO block modulation; factory `calcNextOscSampleBlock` dispatch deliberately unchanged. No new waveform IDs, struct fields or saved-kit bytes.
- Source-only guard added at `tests/test_c05_native_scan_source.py` commit `6d9fe69befa0831befadede4e3c46da9c3839bf9`; **not yet executed**, and native `Oscillator.c` has not been compiled/linked or audio-tested. **C05 PARTIAL.** Next: execute source guard, native GCC/Clang build, add call-site/voice velocity capture and LFO routing behind OFF-by-default controls, then bit-identical factory bypass and overlap CPU/audio regression. No ARM build, firmware image, main merge or browser success claim.

### 2026-11-09 — native trigger and render call-site audit (implementation unblock)
- `Core/Src/Soundengine/MIDI/MidiVoiceControl.c`: MIDI velocity `vel` already passed to `Drum_trigger(voice, vel, note)`, `Snare_trigger`, `Cymbal_trigger`, and `HiHat_trigger`.
- `Core/Src/Soundengine/DSPAudio/DrumVoice.c`: `Drum_trigger` lines 125–140 receives raw `vol` 0..127, calls `lfo_retrigger(voiceNr)`, then routes normalized velocity to `modNode_updateValue(&velocityModulators[voiceNr],vol/127.f)`. Capture raw note-on velocity per voice here without modifying retrigger smoother ordering.
- `calcDrumVoiceSyncBlock` lines 252–275 calls factory `calcNextOscSampleBlock` for the mod oscillator, then main oscillator in mix mode; otherwise `calcNextOscSampleFmBlock` for FM. Guard new scan at this main-oscillator render decision, retaining mod oscillator, FM and original dispatch when OFF. Need dedicated per-voice OFF-by-default settings, 4x256 waveform data, note-on capture, actual LFO block signal and persistence strategy before enabling.
- **No new render call-site code in this audit.** Next atomic task: implement minimal guarded voice-local main-oscillator call with OFF-path sample-identity test and verify with GCC/Clang; do not bypass FM/No Clip or change stored kits. ARM hold unchanged.

### 2026-11-09 — C06 guarded main-drum wavetable call site (CODED, NOT VALIDATED)
- `Core/Src/Soundengine/DSPAudio/DrumVoice.c` commit `465c0eb61080fa3f7d11fe36eae81a014f7e0902`: added volatile-only per-drum opt-in scan configuration, captures raw note-on `vol` before original retrigger handling, and routes main oscillator in `mixOscs` mode to `jb_calcNativeWaveScanBlock` only when explicitly enabled and supplied four persistent tables. Disabled path calls original `calcNextOscSampleBlock` unchanged; FM-only path untouched. `Core/Inc/Soundengine/DSPAudio/DrumVoice.h` API declaration commit `aec77b8ce4045afb1e06f3d838e965cc67555cb0`.
- **C06 PARTIAL / UNVERIFIED**: no host compilation or audio regression, no source tables or caller that enables the path, no native LFO feed, no per-kit persistence or UI, no FM-only integration. Shared table pointer currently applies to all enabled drum voices and must be made voice-specific before supporting independent tables. Next atomic task: validate compile and OFF-path, add owned waveform tables and explicit test harness, then live LFO wiring. Keep ARM compile/image/merge HOLD.

### 2026-11-09 — C06 test execution request
- Confirmed GitHub Actions C04 velocity regression run `42682444356` **SUCCESS** (GCC and Clang matrix), https://github.com/balfourlynn1-byte/lxr-desktop-app/actions/runs/42682444356 . C02 fixture run `42680038099` also SUCCESS.
- Added `tests/test_c06_drum_scan_source.py` commit `3723bc5560db6aecb01756f8adc06e202066f8ce` and `.github/workflows/jb-c06-drum-scan-guards.yml` commit `36fc6aac62a74409f226f0432a504520a65e5757`. GitHub Actions runs API at inspection still listed only C02 and C04; **C05/C06 guards have NOT been observed running or passing**. Source guards do not substitute for native compilation or sound testing.
- C06 remains **PARTIAL/UNVALIDATED**; no ARM build, no hardware candidate. Next: get C05/C06 workflow executed, fix failures, then host-native compilation, waveform-table owner and live LFO call-site wiring, and OFF-path bit identity.

### 2026-11-09 — one-day Codex handover preparation
- Created `docs/CODEX_EXECUTION_BRIEF_2026-11-10.md` commit `a1df5a447788729777e5840ea02a7da75635bbc6`: prioritized executable handover, exact branch references, C02/C04 confirmed Actions, C05/C06 unverified status, native build/legacy OFF identity/wavetable ownership/LFO/UI/SD/CPU gates, candidate manifest, explicit ARM approval hold and first Codex instruction. No build or new validation claimed.

### 2026-11-09 20:19 UTC — C05/C06 source guards verified by GitHub Actions
- Previously unobserved workflow `JIMBONES C05-C06 source guards` run **42687294056** is now **COMPLETED SUCCESS** at commit `36fc6aac62a74409f226f0432a504520a65e5757`: https://github.com/balfourlynn1-byte/lxr-desktop-app/actions/runs/42687294056 . Both C05 and C06 Python **source guards passed in CI**. This is source-pattern verification only; native C compilation, actual OFF-path audio identity, LFO feed, waveform-table lifetime, CPU and hardware remain NOT RUN / OPEN.
- Committed `.github/workflows/jb-c04-wave-velocity.yml` update `84bf478e8e9428318543f0496287299768f3fcaa` to additionally execute both source guards in the existing GCC/Clang host regression matrix. **No run of this new matrix revision observed yet**. Next task: real native host compilation and OFF-path reference audio fixture; do not claim C05/C06 engine PASS. ARM hold unchanged.

### 2026-11-09 — C07 factory OFF-path source comparison prepared (NOT YET PASS)
- Added `tests/test_c07_factory_off_source_compare.py` commit `be82fd6c2493e12afa3fe82a555de94f4607feb6` comparing original oscillator/FM/mix/transient call expressions with integration branch `agents/jb-28-full-integration-v186`, plus guarded factory fallback checks.
- Added CI `.github/workflows/jb-c07-factory-off-compare.yml` commit `3e53281a3e7091d13be2a71454423f1f048c7542`, fetching the baseline in CI. At last Actions API check C07 had **no observed run**; status NOT RUN, not PASS. This is SOURCE equivalence only; real native compile and PCM sample-identity comparison remain BLOCKED/NOT RUN. No ARM build/image/merge.

### 2026-11-09 — C08 voice-local wavetable bank routing
- Replaced shared C06 `jbScanTables` pointer with `jbScanTables[NUM_DRUM_VOICES]` in `DrumVoice.c` commit `dc129b7004cf140281d69750783518bd13feb096`; configure sets bank for target voice only, disable clears target pointer, render reads that voice's bank. This prevents enabling a second voice from silently redirecting the first voice to the second's waveform bank.
- Added source guard `tests/test_c08_voice_local_scan_bank_source.py` commit `24327894ee930025d921e76686903965e96b0550`. **NOT RUN**; no native compilation/audio/CPU or persistent owned wave tables yet. Borrowed pointers must still reference durable data. Next priority: execute guard, native compile, then ROM/owned wave bank and audible ON-path fixture. ARM hold unchanged.

### 2026-11-09 — C08 automated guard wired to CI
- Added `tests/test_c08_voice_local_scan_bank_source.py` to existing C04 GCC/Clang workflow and push path filters, commit `dc3fae1b9f59c4832d5be8fd51e32c32a8b1d329`. The check now executes on relevant changes, rather than sitting uninvoked. **CI run result not yet verified**; do not count source guard as audio or native compile. Next: inspect Actions outcome, then add actual durable waveform tables and native ON-path PCM test. No ARM compile or image.
