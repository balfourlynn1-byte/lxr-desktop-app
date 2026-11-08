# JIMBONES agent assignment board

**Status (2026-11-08):** JIMBONES **v1.8.04 EXPERIMENTAL** research handover. Agents are virtual responsibilities, **not running autonomous processes**; this tracked board replaces GitHub Issues (API 410). No job is implemented merely by being listed. Codex should take the discrete, researched jobs below, one PR per change.

**Start here:** [Dub-send technical audit](agents/dub_step_send_v184.md) · [Codex execution handover](CODEX_V184_HANDOVER.md). Official user manual: https://www.ericasynths.lv/media/LXR_manual.pdf

| Priority | Assigned agent roles | Task | Status |
|---|---|---|---|
| P0 | Verification + Integration | Baseline parity and release gate | Backlog |
| P0 | Embedded + DSP | Real-time CPU monitor | Research complete; implementation queued |
| P0 | DSP + Verification | Multi-voice modulation crunch | Reproduced by user on hardware; fix and quantify |
| P1 | DSP + Embedded | Morph A/B/C, LFO and velocity | Backlog |
| P1 | DSP + Verification | Per-voice tape/tube saturation | Backlog |
| P2 | Research + Embedded | USB multitrack feasibility | Deferred until stable audio budget |
| P0 | DSP + Embedded | Step FX on/off toggle and step 1 LED fix (JB-07/JB-21) | Source mapped; needs implementation |
| HOLD | DSP + Research | Metallic FM layered engine | Shelved by user: insufficient audible benefit for DSP load |
| P1 | DSP + Research | Wavetable oscillators and waveshaping | Core tables/interpolation found; optimize, audit and regression-gate |
| P2 | Embedded + Verification | Hi-hat articulation and velocity curves | Backlog |
| P2 | DSP + Embedded | Per-hit snare randomisation | Backlog |
| P1 | DSP + Embedded | Bass and comb filters | Comb: research/source audit required before implementation |
| P1 | Embedded + Verification | UI navigation and performance diagnostics | Step 1 and modal controls urgent; CPU monitor kept |
 
| P0 | DSP + Embedded | JB-19A — six-lane send gate | **Draft PR #8 implemented; host tests passed locally; ARM/hardware not verified** |
| P1 | DSP + Embedded | JB-20 — tempo-synced dub delay using current FX buffer | **Research complete, buffer limits known; implementation needed** |
| P0 | Embedded + Verification | JB-21 — all 16 buttons toggle and LEDs, per-voice send UI | **Source locations mapped; implementation needed** |
| P0 | Verification + Integration | JB-22 — baseline/CPU/host/ARM/hardware release gates | **Hardware failure evidence recorded; test implementation needed** |

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
- **Scope:** Reproduce simultaneous modulation crunch with overlapping 3–4 hits; isolate clipping, CPU overruns and shared state.
- **Deliverable:** Research findings with source links and license check, implementation proposal or focused PR, host tests, ARM build, image verification and hardware-test notes.
- **State:** Initial code audit complete; hardware reproduction CONFIRMED (DSP 94–101%, PK 105%, LATE 999, RAIL 240 on overloaded tom tests); see `docs/JIMBONES_P0_FINDINGS.md` and `docs/agents/dub_step_send_v184.md`. Root cause not yet established.

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
- **Scope:** Independent bass HPF/LPF and comb resonance stability/CPU profiling.
- **Deliverable:** Research findings with source links and license check, implementation proposal or focused PR, host tests, ARM build, image verification and hardware-test notes.
- **State:** Backlog; no agent process launched.

### JB-13 — UI navigation and performance diagnostics
- **Priority:** P1
- **Owners:** Embedded + Verification
- **Scope:** Double-press pages, edit indicators, CPU diagnostics; waveform display deferred.
- **Deliverable:** Research findings with source links and license check, implementation proposal or focused PR, host tests, ARM build, image verification and hardware-test notes.
- **State:** Backlog; no agent process launched.

### JB-19 — Parallel gated send from existing rendered voices
- **Priority:** P0. **Owners:** DSP + Embedded.
- **Scope:** Preserve physical dry routing; tap rendered voice to FX buffer on bounded per-voice Step FX gate; no voice retrigger; no additional delay engine.
- **State:** JB-19A implemented on isolated draft [PR #8](https://github.com/balfourlynn1-byte/lxr-desktop-app/pull/8). A mirrored copy of the source and regression fixture passed strict GCC and Clang host tests with sanitizers (2026-11-08). **Not merged, no GitHub CI/ARM or device test yet; no audible send exists.** JB-19B mixer tap remains unimplemented. [Source audit](agents/dub_step_send_v184.md).

### JB-20 — Dub tempo-synced delay and feedback tails
- **Priority:** P1. **Owners:** DSP + Embedded.
- **Scope:** Use current global Delay engine; BPM subdivisions only if within existing mono/ping-pong buffer capacity; ensure trails continue after send gate closes.
- **State:** Research complete, code changes unimplemented; [buffer math and source locations](agents/dub_step_send_v184.md).

### JB-21 — Step FX buttons, LED parity and send controls
- **Priority:** P0. **Owners:** Embedded + Verification.
- **Scope:** Press to enable, press again disable, for all sixteen steps. Fix step 1 LED; preserve separate CPU diagnostic reset; provide send amount, gate length, voice mask.
- **State:** UI code mapped, not implemented; [Codex handoff](agents/dub_step_send_v184.md).

### JB-22 — Experimental integration and regression gate
- **Priority:** P0. **Owners:** Verification + Integration.
- **Scope:** Dry bit-parity bypass, four-pad overlapping hit, echo feedback tail, FX routing, memory, host + ARM CI, v1.8.02 rollback, hardware check.
- **State:** Test checklist complete, tests and firmware build not yet done; [Codex handoff](CODEX_V184_HANDOVER.md).

## Findings and constraints

- **Next implementation job:** JB-19B, read PR #8 and add a parallel, gated copy of already-rendered voice audio to the existing FX bus, without rerouting or retriggering the dry voice. Review and merge the JB-19A primitive into a verified integration branch before building on it; no firmware has been flashed for the new send.

- Earlier v1.8.03 ARM CI passed, AND v1.8.03 subsequently **booted on tester's actual LXR-02**. It is **NOT safe/validated**: DSP crunch/overrun reproduced; idle readings after restart DSP 62%, PK 66%, LATE 0, RAIL 0; under load DSP 101%, PK 105%, LATE 999 (UI cap), RAIL 240. These meter fields have distinct semantics. No v1.8.04 firmware build exists yet.
- CPU instrumentation and multi-voice crunch are first engineering priorities; any new send must reuse existing FX processing and default OFF. User explicitly shelved Metallic FM and moved wavetable, dub send, step toggle/LED fix and comb-filter feasibility higher. The upstream LXR02Open license is **non-commercial**, not automatically compatible with GPL-3.0-only: study ideas, do not copy source.
- USB multitrack is a feasibility investigation, not a committed deliverable.
- Keep v1.8.02 untouched; experimental changes stay on dedicated branches.
- Reference implementations require licensing and architectural review before code reuse.

## Updating this board

Change each task's state only with evidence: Backlog → Research → Implementation → CI verified → Hardware verified. Include PR link, exact commit, measurements and blockers in the relevant work order.
