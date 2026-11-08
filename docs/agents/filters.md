# JB-12 filters

**State:** Research proposal, no autonomous agent executing.

## Improvement investigation
Prototype a stable bass HPF/LPF and bounded comb delay; calculate coefficients outside sample loop, measure worst-case resonance, memory and CPU; protect existing six-voice signal path.

## Required work
Identify upstream references and license constraints; define baseline and test fixtures; implement on isolated branch; verify host tests, ARM build and image; require hardware testing before release.

Do not overwrite known-working v1.8.02.

## Solution research handoff — 2026-11-08
**Task:** JB-12. **Status:** proposed engineering approach; not implemented or benchmarked unless independently recorded.

**Recommended design:** Prototype optional stable biquad bass HPF/LPF with smoothed coefficients and bounded resonance; add short comb only if SRAM and CPU budget allow; no dynamic allocation in callback.

**Acceptance evidence:** Frequency response, coefficient sweep, denormals, resonance and feedback stability, worst-case cycles.

**Integration:** feature flag / opt-in, preserve legacy kits and all six voices, host regression + ARM CI + hardware checks before release. Do not touch USB audio in this sprint.

**Research references:** https://github.com/erica-synths/lxr-desktop-app (official architecture, CMake, memory map); https://github.com/brendanclarke/LXR02Open (CPU/interpolation concepts, inspect license before reuse); https://altstrument.org/devices/erica-synths-lxr-02 (community feature comparisons). No external code copied.
