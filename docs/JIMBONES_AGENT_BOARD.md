# JIMBONES agent assignment board

**Status (2026-11-08):** JIMBONES **v1.8.04 EXPERIMENTAL** integration and hardware-test handover. Agents are virtual responsibilities, **not running autonomous processes**; this tracked board replaces GitHub Issues (API 410). No job is implemented merely by being listed. Codex should take the discrete, researched jobs below, one PR per change.

**Start here:** [Full dependency job tree](JIMBONES_JOB_TREE_V184.md) · [Codex handover](CODEX_V184_HANDOVER.md) · [Dub-send audit](agents/dub_step_send_v184.md). Official manual: https://www.ericasynths.lv/media/LXR_manual.pdf

| Priority | Assigned agent roles | Task | Status |
|---|---|---|---|
| P0 | Verification + Integration | Baseline parity and release gate | Backlog |
| P0 | Embedded + DSP | Real-time CPU monitor | Research complete; implementation queued |
| P0 | Verification (DSP conditional) | JB-03 — post-Metallic DSP load check | **No confirmed v1.8.04 crunch; FM disabled.** Await 2-tom/4-pad readings before any code fix |
| P1 | DSP + Embedded | Morph A/B/C, LFO and velocity | Backlog |
| P1 | DSP + Verification | Per-voice tape/tube saturation | Backlog |
| P2 | Research + Embedded | USB multitrack feasibility | Deferred until stable audio budget |
| P0 | DSP + Embedded | Step FX on/off toggle and step 1 LED fix (JB-07/JB-21) | **PR #10 implemented; physical LEDs not yet verified** |
| HOLD | DSP + Research | Metallic FM layered engine | Compiled OUT in JB-22 draft PR #12; legacy kit format preserved |
| P1 | DSP + Research | Wavetable oscillators and waveshaping | JB-24 [PR #13](https://github.com/balfourlynn1-byte/lxr-desktop-app/pull/13) wave-first OSC UI CI green; hardware unverified |
| P2 | Embedded + Verification | Hi-hat articulation and velocity curves | Backlog |
| P2 | DSP + Embedded | Per-hit snare randomisation | Backlog |
| P1 | DSP + Embedded | JB-12 — Bass and comb filters | **Comb implemented in [draft PR #15](https://github.com/balfourlynn1-byte/lxr-desktop-app/pull/15), [host/ARM CI passed](https://github.com/balfourlynn1-byte/lxr-desktop-app/actions/runs/42495034632); hardware unverified.** Independent bass filter remains backlog |
| P1 | Embedded + Verification | UI navigation and performance diagnostics | Source/UI regression green; real LED/display test pending |
 
| P0 | DSP + Embedded | JB-19A — six-lane send gate | **Draft PR #8 implemented; host tests passed locally; ARM/hardware not verified** |
| P0 | DSP + Embedded | JB-19B — parallel FX mixer tap | **Draft PR #9; GitHub host + ARM CI passed, hardware pending** |
| P1 | DSP + Embedded | JB-20 — tempo-synced dub delay using current FX buffer | **Draft PR #11 implemented; host+ARM CI green; hardware pending** |
| P0 | Embedded + Verification | JB-21 — all 16 buttons toggle and LEDs, per-voice send UI | **Draft PR #10 coded; host+ARM CI green, actual LED/device verification pending** |
| P0 | Verification + Integration | JB-22 — integrated v1.8.04 release gates | PR #12 host/ARM/image green; **hardware pending** |
| P1 | Embedded + DSP | JB-24 — wave-first OSC menu; metallic hidden | **[PR #13](https://github.com/balfourlynn1-byte/lxr-desktop-app/pull/13), host+ARM passed**; device unverified |
| P0 | Verification + Integration | JB-23 — SHA-pinned experimental test pack | **[PR #14](https://github.com/balfourlynn1-byte/lxr-desktop-app/pull/14), [CI passed](https://github.com/balfourlynn1-byte/lxr-desktop-app/actions/runs/42487592194), [artifact available](https://github.com/balfourlynn1-byte/lxr-desktop-app/actions/runs/42487592194/artifacts/13620400602)**; device unverified |

## Individual work orders

### JB-01 — Baseline parity and release gate
- **Priority:** P0
- **Owners:** Verification + Integration
- **Scope:** Compare v1.8.03 with working v1.8.02, image SHA, ELF sizes and rollback.
- **Deliverable:** Research findings with source links and license check, implementation proposal or focused PR, host tests, ARM build, image verification and hardware-test notes.
- **State:** Backlog; no agent process launched.

### JB-02 — Real-time CPU monitor
- **Priority:** P0
- **Owners:** Embedded + DSP
- **Scope:** DWT cycle counter, audio budget, peak/average load and overrun count; no audio-thread allocation.
- **Deliverable:** Research findings with source links and license check, implementation proposal or focused PR, host tests, ARM build, image verification and hardware-test notes.
- **State:** Initial research complete; see `docs/JIMBONES_P0_FINDINGS.md`. Implementation pending.

### JB-03 — Multi-voice modulation crunch
- **Priority:** P0
- **Owners:** DSP + Verification
- **Scope:** Compare the *FM-free v1.8.04* candidate against the same older 2-tom and 4-pad load scenarios, logging DSP/PK/LATE/RAIL. Only investigate clipping, deadlines or code optimisation if the issue remains on new hardware.
- **Deliverable:** Research findings with source links and license check, implementation proposal or focused PR, host tests, ARM build, image verification and hardware-test notes.
- **State:** Previous metallic-enabled **v1.8.03** hardware reproduction confirmed (DSP 94–101%, PK 105%, LATE 999, RAIL 240). Metallic FM render logic is now compiled out in v1.8.04 and hidden from the UI, so its previous CPU cost is not expected to recur. **Do not mark v1.8.04 faulty or claim a precise improvement before device measurements.** Next action is a like-for-like test with the backed-up experimental candidate.

### JB-04 — Morph A/B/C, LFO and velocity
- **Priority:** P1
- **Owners:** DSP + Embedded
- **Scope:** Protect A; edit B/C; per-voice exclusion and kick protection; smooth interpolation.
- **Deliverable:** Research findings with source links and license check, implementation proposal or focused PR, host tests, ARM build, image verification and hardware-test notes.
- **State:** Backlog; no agent process launched.

### JB-05 — Per-voice tape/tube saturation
- **Priority:** P1
- **Owners:** DSP + Verification
- **Scope:** Low-end-preserving saturation with velocity drive; aliasing, gain and CPU tests.
- **Deliverable:** Research findings with source links and license check, implementation proposal or focused PR, host tests, ARM build, image verification and hardware-test notes.
- **State:** Backlog; no agent process launched.

### JB-06 — USB multitrack feasibility
- **Priority:** P1
- **Owners:** Research + Embedded
- **Scope:** Check STM32 USB/DMA/memory/descriptor constraints and 4-channel routing; feasibility before code.
- **Deliverable:** Research findings with source links and license check, implementation proposal or focused PR, host tests, ARM build, image verification and hardware-test notes.
- **State:** Backlog; no agent process launched.

### JB-07 — Step performance FX
- **Priority:** P0
- **Owners:** DSP + Embedded
- **Scope:** Tempo-synced 2/4/8-bar effects; kick exclusion, trigger delay, randomisation and step UI.
- **Deliverable:** Research findings with source links and license check, implementation proposal or focused PR, host tests, ARM build, image verification and hardware-test notes.
- **State:** Backlog; no agent process launched.

### JB-08 — Metallic FM layered engine
- **Priority:** HOLD
- **Owners:** DSP + Research
- **Scope:** 2–3 concurrent layers, six-voice compatibility and CPU benchmark.
- **Deliverable:** Research findings with source links and license check, implementation proposal or focused PR, host tests, ARM build, image verification and hardware-test notes.
- **State:** Shelved following hardware overload and modest sound change; no agent process launched.

### JB-09 — Wavetable oscillators and waveshaping
- **Priority:** P1
- **Owners:** DSP + Research
- **Scope:** Interpolation, velocity layers, licensing audit, aliasing tests and baseline-preserving off mode.
- **Deliverable:** Research findings with source links and license check, implementation proposal or focused PR, host tests, ARM build, image verification and hardware-test notes.
- **State:** Backlog; no agent process launched.

### JB-10 — Hi-hat articulation and velocity curves
- **Priority:** P2
- **Owners:** Embedded + Verification
- **Scope:** FD-8 choking and expressive quiet/loud hits with existing Alesis mapping.
- **Deliverable:** Research findings with source links and license check, implementation proposal or focused PR, host tests, ARM build, image verification and hardware-test notes.
- **State:** Backlog; no agent process launched.

### JB-11 — Per-hit snare randomisation
- **Priority:** P2
- **Owners:** DSP + Embedded
- **Scope:** Percentage blend, rhythm constraints, kit-level toggle and kick protection.
- **Deliverable:** Research findings with source links and license check, implementation proposal or focused PR, host tests, ARM build, image verification and hardware-test notes.
- **State:** Backlog; no agent process launched.

### JB-12 — Bass and comb filters
- **Priority:** P1
- **Owners:** DSP + Embedded
- **Scope:** Separate bass HPF/LPF is still backlog. A **per-voice recursive Comb** is now a real ninth native FILTER Type, reusing existing Freq/Reso without modifying kit IDs. Benchmarks and physical CPU measurements remain necessary.
- **Deliverable:** Research findings with source links and license check, implementation proposal or focused PR, host tests, ARM build, image verification and hardware-test notes.
- **State:** **Comb source implemented** in stacked draft [PR #15](https://github.com/balfourlynn1-byte/lxr-desktop-app/pull/15); six-voice 384k-sample host stress, GCC/Clang regressions, ARM cross-build, image integrity and >=512 B ITCM memory guard all passed [run #42495034632](https://github.com/balfourlynn1-byte/lxr-desktop-app/actions/runs/42495034632). Only **528 B free ITCM** and no physical audio/CPU tests; not approved for flashing. The original factory FM submenus are separate from Metallic FM; preserve them. [Controls and risk handover](https://github.com/balfourlynn1-byte/lxr-desktop-app/blob/agents/jb-12-comb-filter-v184/docs/agents/jb12_comb_filter.md).

### JB-13 — UI navigation and performance diagnostics
- **Priority:** P1
- **Owners:** Embedded + Verification
- **Scope:** Double-press pages, edit indicators, CPU diagnostics; waveform display deferred.
- **Deliverable:** Research findings with source links and license check, implementation proposal or focused PR, host tests, ARM build, image verification and hardware-test notes.
- **State:** Backlog; no agent process launched.

### JB-19 — Parallel gated send from existing rendered voices
- **Priority:** P0. **Owners:** DSP + Embedded.
- **Scope:** Preserve physical dry routing; tap rendered voice to FX buffer on bounded per-voice Step FX gate; no voice retrigger; no additional delay engine.
- **State:** JB-19A draft [PR #8](https://github.com/balfourlynn1-byte/lxr-desktop-app/pull/8); JB-19B stacked draft [PR #9](https://github.com/balfourlynn1-byte/lxr-desktop-app/pull/9) now wires an optional additive tap from six rendered voices to the existing global FX bus. GitHub Actions JB-19B **host and ARM build checks passed** on 2026-11-08 ([run #42480313654](https://github.com/balfourlynn1-byte/lxr-desktop-app/actions/runs/42480313654), commit `cc8cd0ff20fafb52e05269b487246889ca0fa806`). **Neither PR merged or device-tested.** UI and tempo-delayed throw implementation now exists in stacked PRs #10 and #11, still **unmerged and device-unverified**. [Source audit](agents/dub_step_send_v184.md).

### JB-20 — Dub tempo-synced delay and feedback tails
- **Priority:** P1. **Owners:** DSP + Embedded.
- **Scope:** Use current global Delay engine; BPM subdivisions only if within existing mono/ping-pong buffer capacity; ensure trails continue after send gate closes.
- **State:** Implemented as stacked draft [PR #11](https://github.com/balfourlynn1-byte/lxr-desktop-app/pull/11), with source-backed mono/pingpong mapping, visible LONG/SHORT/CLK/NOFX statuses, BPM-derived delay repeats, wet-only echo tail and conservative feedback cap. **Host and ARM compilation passed** in [run #42483624857](https://github.com/balfourlynn1-byte/lxr-desktop-app/actions/runs/42483624857), commit `943302d4e03d41b5a24caca04fe8242633243fd4`. ITCM overflow caught in first build, then fixed by moving once-per-block timing setup out of ITCM. Not hardware-verified and not approved for flashing; see [implementation handover](https://github.com/balfourlynn1-byte/lxr-desktop-app/blob/agents/jb-20-dub-delay-sync-v184/docs/agents/dub_delay_sync_jb20.md).

### JB-21 — Step FX buttons, LED parity and send controls
- **Priority:** P0. **Owners:** Embedded + Verification.
- **Scope:** Press to enable, press again disable, for all sixteen steps. Step 1 is DUB SEND; preserve separate CPU diagnostic reset, enable per-voice send amount, 1/4–1/32-note gate lengths, voice mask.
- **State:** Implemented in stacked draft [PR #10](https://github.com/balfourlynn1-byte/lxr-desktop-app/pull/10) on top of #9/#8, host+ARM CI passed [run #42482118609](https://github.com/balfourlynn1-byte/lxr-desktop-app/actions/runs/42482118609). Real physical LED and CPU/FX output test pending; not merged, not flashable. [Control mapping](https://github.com/balfourlynn1-byte/lxr-desktop-app/blob/agents/jb-21-stepfx-toggle-dub-ui-v184/docs/agents/stepfx_ui_jb21.md).

### JB-22 — Experimental integration and regression gate
- **Priority:** P0. **Owners:** Verification + Integration.
- **Scope:** Dry bit-parity bypass, four-pad overlapping hit, echo feedback tail, FX routing, memory, host + ARM CI, v1.8.02 rollback, hardware check.
- **State:** Draft [PR #12](https://github.com/balfourlynn1-byte/lxr-desktop-app/pull/12) integrates JB-19A/B/JB-21/JB-20, compiles Metallic FM fully out, preserves legacy kit payload mapping, and checks mono/pingpong delays, six-voice output bypass, 16-button UI, Step 1 LED source path, external-clock failures and host/ARM release gates. GCC + Clang host tests, ARM cross-link and image integrity all **PASSED** in [run #42485178857](https://github.com/balfourlynn1-byte/lxr-desktop-app/actions/runs/42485178857); ITCM has **560 bytes free** versus 8 bytes free in JB-20. The strict 512-byte margin gate passed. **Hardware DSP/PK/LATE/RAIL test and real LED/audio confirmation remain mandatory.** [Verification plan](https://github.com/balfourlynn1-byte/lxr-desktop-app/blob/agents/jb-22-integration-gate-v184/docs/agents/jb22_release_gate.md).

## Findings and constraints

- **Next engineering gate:** Independently review [stacked #8–#14](https://github.com/balfourlynn1-byte/lxr-desktop-app/pull/14), verify rollback and SD kit backup, and run **on-device** CPU/PK/LATE/RAIL and full dry/wet routing tests of the JB-23 v1.8.04 candidate against a backed-up bootable firmware. GitHub source verification is green, but device verification is outstanding. New FM-free candidate source is **not yet hardware-validated**. Then wavetable cost optimisation and comb-filter feasibility, after headroom is confirmed. Review JB-19A PR #8 before stacked JB-19B PR #9. Dry voice remains untouched. GitHub ARM checks and real-device CPU headroom/hardware acceptance are required; no firmware has been flashed for the new send.

- Earlier v1.8.03 ARM CI passed, AND v1.8.03 subsequently **booted on tester's actual LXR-02**. It is **NOT safe/validated**: DSP crunch/overrun reproduced; idle readings after restart DSP 62%, PK 66%, LATE 0, RAIL 0; under load DSP 101%, PK 105%, LATE 999 (UI cap), RAIL 240. These meter fields have distinct semantics. A v1.8.04 GitHub Actions-generated **HARDWARE-UNVERIFIED** firmware test artifact now exists: [run #42487592194](https://github.com/balfourlynn1-byte/lxr-desktop-app/actions/runs/42487592194), SHA256 `0be1f7553f0fd86adbd8c49891b5de0c2f44a46348642e369b6223a76c741aae`. No on-device tests have been performed for this image.
- CPU instrumentation and multi-voice crunch are first engineering priorities; any new send must reuse existing FX processing and default OFF. User explicitly shelved Metallic FM and moved wavetable, dub send, step toggle/LED fix and comb-filter feasibility higher. The upstream LXR02Open license is **non-commercial**, not automatically compatible with GPL-3.0-only: study ideas, do not copy source.
- USB multitrack is a feasibility investigation, not a committed deliverable.
- Keep v1.8.02 untouched; experimental changes stay on dedicated branches.
- Reference implementations require licensing and architectural review before code reuse.

## Updating this board

Change each task's state only with evidence: Backlog → Research → Implementation → CI verified → Hardware verified. Include PR link, exact commit, measurements and blockers in the relevant work order.
