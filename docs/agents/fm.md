# JB-08 metallic FM — independent agent workstream

**Owners:** DSP + Research
**State:** Research proposal, not autonomous agent execution or tested implementation.

## Improvement investigation
Profile existing FM layer first, compare precomputed coefficient and phase optimisations, bound feedback, test 2–3 simultaneous layers and aliasing. Preserve six voices.

## Deliverables
1. Locate concrete integration points and relevant upstream references, with license review.
2. Define a measurable before/after fixture and performance budget.
3. Implement minimal isolated code on this branch; do not alter release branch directly.
4. Request review and run host/ARM CI. Hardware verification required for release.

## Safety
Keep original six voices and the working v1.8.02 untouched. Do not merge unverified firmware.

## Solution research handoff — 2026-11-08
**Task:** JB-08. **Status:** proposed engineering approach; not implemented or benchmarked unless independently recorded.

**Recommended design:** Profile metallic FM sample loop before changes; cache per-trigger coefficients and increments, bound feedback and expensive transcendental calls, retain original oscillator modes and 6-voice allocation.

**Acceptance evidence:** 1–3 concurrent FM layers, alias spectra, CPU cycle before/after, deterministic host output.

**Integration:** feature flag / opt-in, preserve legacy kits and all six voices, host regression + ARM CI + hardware checks before release. Do not touch USB audio in this sprint.

**Research references:** https://github.com/erica-synths/lxr-desktop-app (official architecture, CMake, memory map); https://github.com/brendanclarke/LXR02Open (CPU/interpolation concepts, inspect license before reuse); https://altstrument.org/devices/erica-synths-lxr-02 (community feature comparisons). No external code copied.
