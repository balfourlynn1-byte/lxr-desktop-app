# JB-05 tape/tube — independent agent workstream

**Owners:** DSP + Verification
**State:** Research proposal, not autonomous agent execution or tested implementation.

## Improvement investigation
Benchmark bounded asymmetric soft saturation and optional pre/de-emphasis, wet/dry loudness matching and aliasing; oversampling only if CPU margin permits. Per-voice not master; preserve low end and measure spectral changes.

## Deliverables
1. Locate concrete integration points and relevant upstream references, with license review.
2. Define a measurable before/after fixture and performance budget.
3. Implement minimal isolated code on this branch; do not alter release branch directly.
4. Request review and run host/ARM CI. Hardware verification required for release.

## Safety
Keep original six voices and the working v1.8.02 untouched. Do not merge unverified firmware.

## Solution research handoff — 2026-11-08
**Task:** JB-05. **Status:** proposed engineering approach; not implemented or benchmarked unless independently recorded.

**Recommended design:** Prototype optional per-voice asymmetric tanh approximation or bounded polynomial soft clip with DC blocking, input trim and equal-loudness wet/dry; high-frequency pre/de-emphasis only after aliasing tests; no oversampling by default.

**Acceptance evidence:** Bypass parity, DC offset, low-end response, 1–4 voices, alias spectra, CPU cycles.

**Integration:** feature flag / opt-in, preserve legacy kits and all six voices, host regression + ARM CI + hardware checks before release. Do not touch USB audio in this sprint.

**Research references:** https://github.com/erica-synths/lxr-desktop-app (official architecture, CMake, memory map); https://github.com/brendanclarke/LXR02Open (CPU/interpolation concepts, inspect license before reuse); https://altstrument.org/devices/erica-synths-lxr-02 (community feature comparisons). No external code copied.
