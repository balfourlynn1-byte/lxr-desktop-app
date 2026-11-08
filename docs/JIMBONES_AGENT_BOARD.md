# JIMBONES agent assignment board

**Status:** Assignments defined; agents are virtual roles, not active autonomous processes. GitHub Issues is disabled in this repository (API 410), so this tracked board replaces issue creation. No task is implemented merely by appearing here.

| Priority | Assigned agent roles | Task | Status |
|---|---|---|---|
| P0 | Verification + Integration | Baseline parity and release gate | Backlog |
| P0 | Embedded + DSP | Real-time CPU monitor | Research complete; implementation queued |
| P0 | DSP + Verification | Multi-voice modulation crunch | Initial code audit complete; reproduction queued |
| P1 | DSP + Embedded | Morph A/B/C, LFO and velocity | Backlog |
| P1 | DSP + Verification | Per-voice tape/tube saturation | Backlog |
| P1 | Research + Embedded | USB multitrack feasibility | Backlog |
| P1 | DSP + Embedded | Step performance FX | Backlog |
| P2 | DSP + Research | Metallic FM layered engine | Backlog |
| P2 | DSP + Research | Wavetable oscillators and waveshaping | Backlog |
| P2 | Embedded + Verification | Hi-hat articulation and velocity curves | Backlog |
| P2 | DSP + Embedded | Per-hit snare randomisation | Backlog |
| P2 | DSP + Embedded | Bass and comb filters | Backlog |
| P2 | Embedded + Verification | UI navigation and performance diagnostics | Backlog |

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
- **State:** Initial code audit complete; see `docs/JIMBONES_P0_FINDINGS.md`. Reproduction pending.

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
- **Priority:** P1
- **Owners:** DSP + Embedded
- **Scope:** Tempo-synced 2/4/8-bar effects; kick exclusion, trigger delay, randomisation and step UI.
- **Deliverable:** Research findings with source links and license check, implementation proposal or focused PR, host tests, ARM build, image verification and hardware-test notes.
- **State:** Backlog; no agent process launched.

### JB-08 — Metallic FM layered engine
- **Priority:** P2
- **Owners:** DSP + Research
- **Scope:** 2–3 concurrent layers, six-voice compatibility and CPU benchmark.
- **Deliverable:** Research findings with source links and license check, implementation proposal or focused PR, host tests, ARM build, image verification and hardware-test notes.
- **State:** Backlog; no agent process launched.

### JB-09 — Wavetable oscillators and waveshaping
- **Priority:** P2
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
- **Priority:** P2
- **Owners:** DSP + Embedded
- **Scope:** Independent bass HPF/LPF and comb resonance stability/CPU profiling.
- **Deliverable:** Research findings with source links and license check, implementation proposal or focused PR, host tests, ARM build, image verification and hardware-test notes.
- **State:** Backlog; no agent process launched.

### JB-13 — UI navigation and performance diagnostics
- **Priority:** P2
- **Owners:** Embedded + Verification
- **Scope:** Double-press pages, edit indicators, CPU diagnostics; waveform display deferred.
- **Deliverable:** Research findings with source links and license check, implementation proposal or focused PR, host tests, ARM build, image verification and hardware-test notes.
- **State:** Backlog; no agent process launched.

## Findings and constraints

- The v1.8.03 ARM CI has passed; the firmware remains hardware-untested.
- CPU instrumentation and multi-voice crunch are first engineering priorities, before adding more DSP load.
- USB multitrack is a feasibility investigation, not a committed deliverable.
- Keep v1.8.02 untouched; experimental changes stay on dedicated branches.
- Reference implementations require licensing and architectural review before code reuse.

## Updating this board

Change each task's state only with evidence: Backlog → Research → Implementation → CI verified → Hardware verified. Include PR link, exact commit, measurements and blockers in the relevant work order.
