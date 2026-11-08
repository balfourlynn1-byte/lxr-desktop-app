# JB-09 wavetable — independent agent workstream

**Owners:** DSP + Research
**State:** Research proposal, not autonomous agent execution or tested implementation.

## Improvement investigation
Measure interpolation on/off CPU, crossfade band-limited tables, avoid new large ROM banks; test aliasing and velocity blend. Review LXR02Open license and avoid direct copying without compatibility audit.

## Deliverables
1. Locate concrete integration points and relevant upstream references, with license review.
2. Define a measurable before/after fixture and performance budget.
3. Implement minimal isolated code on this branch; do not alter release branch directly.
4. Request review and run host/ARM CI. Hardware verification required for release.

## Safety
Keep original six voices and the working v1.8.02 untouched. Do not merge unverified firmware.

## Solution research handoff — 2026-11-08
**Task:** JB-09. **Status:** proposed engineering approach; not implemented or benchmarked unless independently recorded.

**Recommended design:** Measure current linear interpolation and wavetable cost; offer selectable interpolation with bandlimited table selection or crossfade where feasible. Examine LXR02Open only as reference because license is not automatically GPL-compatible.

**Acceptance evidence:** License check, frequency sweeps and alias spectra, memory map, ARM cycle budget, old-kit parity.

**Integration:** feature flag / opt-in, preserve legacy kits and all six voices, host regression + ARM CI + hardware checks before release. Do not touch USB audio in this sprint.

**Research references:** https://github.com/erica-synths/lxr-desktop-app (official architecture, CMake, memory map); https://github.com/brendanclarke/LXR02Open (CPU/interpolation concepts, inspect license before reuse); https://altstrument.org/devices/erica-synths-lxr-02 (community feature comparisons). No external code copied.
