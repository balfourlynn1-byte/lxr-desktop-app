# JIMBONES — next sprint assignments (2026-11-08)

**Execution model:** Named agent roles are work orders, NOT autonomous processes. Each owner works on its own `agents/*-research` branch; coordinator reviews PRs into the experimental branch. Do not merge or flash automatically.

| Order | Owner | Immediate task | Evidence required | Dependency |
|---|---|---|---|---|
| JB-01 | Verification + Integration | Retrieve working v1.8.02 source/image reference and compare with v1.8.03 build inputs, version, image layout, checksum and rollback | Compatibility matrix and exact revisions | User baseline availability |
| JB-02 | Embedded + DSP | Find complete audio callback and STM32 clock configuration; implement DWT CYCCNT callback timing behind feature flag | Last/peak cycles, budget, overruns, host math tests, ARM compile | None |
| JB-03 | DSP + Verification | Run and extend PR #2 one-to-four voice regression; capture separate final mixer clips and layer saturation | Reproducible failing or passing fixtures, CI link and before/after | JB-02 for CPU correlation |
| JB-04 | DSP + Embedded | Design A-protected B/C morph storage and per-voice mask with kick exclusion | State diagram, kit compatibility, smoothing specification | JB-01 |
| JB-05 | DSP + Verification | Compare bounded soft saturation alternatives at equal loudness | Spectral and peak measurements, CPU estimate, test audio | JB-02 |
| JB-06 | Research + Embedded | Inspect USB peripheral/descriptors and 4-channel 48 kHz throughput and RAM | Feasibility memo with blockers and fallback | None |
| JB-07 | DSP + Embedded | Calculate delay-buffer RAM for tempo-synced 2/4/8 bar step FX and define kick-safe routing | Memory table, timing/state diagram, bounded feedback | JB-02 |
| JB-08 | DSP + Research | Profile current metallic FM cost and determine cheaper coefficient/phase strategy | Before/after cost model and aliasing cases | JB-02 |
| JB-09 | DSP + Research | Compare interpolation/waveshaping cost and external reference licensing | Test matrix and licensing notes | JB-02 |
| JB-10 | Embedded + Verification | Define FD-8/Alesis MIDI open/closed choke and velocity test events | Reproducible MIDI event fixture | None |
| JB-11 | DSP + Embedded | Specify seeded per-hit snare random engine selector and backbeat protection | Deterministic algorithm and edge-case tests | JB-04 |
| JB-12 | DSP + Embedded | Evaluate filter topologies and comb memory/resonance limits | Stable parameter ranges, RAM and CPU estimates | JB-02 |
| JB-13 | Embedded + Verification | Identify existing menu hooks for CPU meter and double-press navigation | UI page map and non-blocking update plan | JB-02 |

## Acceptance checklist
- Research notes must distinguish observed code from hypotheses.
- Each code change is isolated to its own branch with tests and a draft PR.
- Verification records exact revision, host test output, ARM result, memory and image SHA256.
- A passing host test cannot establish no crunch on hardware.
- No changes to known-working v1.8.02; no automated merge or flash.
- Combined release gate: six-voice compatibility, 3–4 pad simultaneous hits, CPU budget, clipping, kits, boot/rollback, and user hardware confirmation.

## Next coordinator review
1. Review JB-03 PR #2 host CI and address failures.
2. Review JB-02 instrumented callback implementation when available.
3. Approve only measured features for staged merge; defer USB multitrack if infeasible.
