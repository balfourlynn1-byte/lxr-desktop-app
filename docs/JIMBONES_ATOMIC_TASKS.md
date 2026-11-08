# JIMBONES autonomous atomic task tree — 2026-11-08

**Purpose:** one small verifiable task per run. Always read this file and `docs/JIMBONES_WORK_QUEUE.md` first. Work in the existing feature branch or a dedicated isolated branch; never merge to main. User is testing v1.8.06 and says it is not nearly ready; bouncing skull is approved and must be preserved. Proposed v1.8.08 is a development target, not a release promise.

## Rules of execution
- Pick the first unblocked TODO; do **one task only** before reporting. Inspect actual source; never infer implementation from design docs.
- A task is DONE only with a pushed commit and evidence (exact paths, relevant code/CI result). A research task needs a precise file/line or symbol map and an explicit finding; a code task needs a passing focused host test and no unreviewed upstream code.
- On success, change TODO to DONE with commit SHA, date and evidence; update `docs/JIMBONES_WORK_QUEUE.md` and checkpoint, push, and notify user with task ID, result, commit and next task. If blocked, mark BLOCKED with specific unblock action; do not silently skip critical dependencies.
- Never copy LXR02Open's non-commercial code into the public GPL-only JIMBONES repo before resolving licensing; source reading and independently written code are allowed.
- **ABSOLUTE ARM HOLD:** no ARM compilation, firmware .img packaging, workflow re-enablement, main merge, or hardware-ready claim until the user explicitly approves the chosen feature-frozen candidate.
- Keep experimental Metallic FM disabled. Preserve original voices, 3 ms No Clip fade and user-approved bouncing skull.

## Atomic task checklist (ordered; dependencies explicit)

### A — Baseline, fade, screensaver
- [ ] **A01** Locate earliest pushed commit and exact source symbols for the original working 3 ms No Clip fade. Evidence: paths, SHA, call chain.
- [ ] **A02** Compare A01 against v1.8.06 code; document any change to fade length, retrigger, note-off or overlapping tails.
- [ ] **A03** Add focused host regression for No Clip fade, with exact expected sample/ramp counts at current sample rate; no hardware build.
- [ ] **A04** Identify v1.8.06 skull code, LCD update and wake paths; pin source SHA and host test so port preserves behaviour.
- [ ] **A05** Pin v1.8.06 candidate image/source identity if available in repo artifacts; otherwise mark hardware image SHA awaiting user-provided file, without guessing.

### B — Upstream source analysis
- [ ] **B01** Pin LXR02Open `prime` HEAD commit, licence, oscillator and interpolation audit blob SHAs.
- [ ] **B02** Trace upstream `modNode_updateValue` fractional waveform-ID write, global interpolation toggle and reset paths.
- [ ] **B03** Trace upstream `Oscillator.c` two-wave block rendering, sample/FM/noise/crash dispatch and fallback.
- [ ] **B04** Compare upstream `OscInfo` with Erica `OscInfo`; map native waveform IDs, LFO target enums and voice count.
- [ ] **B05** Map upstream velocity routing to proposed destinations; select initial safe 2–3 controls and backward-compatible default OFF.
- [ ] **B06** Compare sample memory, kit/global format, MIDI/clock and screen APIs; identify blockers to a full upstream firmware base.
- [ ] **B07** Record licence-compliant implementation route: independently authored Erica adaptation versus separately licensed non-commercial fork; do not transplant incompatible files.

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
- [ ] **D01** Confirm Comb PR #18 full host CI after v1.8.07 assertion fix; record exact run.
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
**A01** — source-trace the original 3 ms No Clip fade. Do not start C or E before A and B prerequisites.
