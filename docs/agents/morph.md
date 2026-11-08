# JB-04 morph A/B/C — independent agent workstream

**Owners:** DSP + Embedded
**State:** Research proposal, not autonomous agent execution or tested implementation.

## Improvement investigation
Keep A immutable during B/C edits; double-buffer snapshot state and interpolate at audio-block boundaries; parameter-specific smoothing for frequencies vs enums, per-voice mask and kick exclusion; add velocity/LFO tests.

## Deliverables
1. Locate concrete integration points and relevant upstream references, with license review.
2. Define a measurable before/after fixture and performance budget.
3. Implement minimal isolated code on this branch; do not alter release branch directly.
4. Request review and run host/ARM CI. Hardware verification required for release.

## Safety
Keep original six voices and the working v1.8.02 untouched. Do not merge unverified firmware.

## Solution research handoff — 2026-11-08
**Task:** JB-04. **Status:** proposed engineering approach; not implemented or benchmarked unless independently recorded.

**Recommended design:** Represent A as immutable stored snapshot; B/C separate snapshots, per-voice enable bitmask; continuous params interpolate with slew, discrete engine/enums switch at zero crossing or trigger boundary; LFO/velocity produce bounded position.

**Acceptance evidence:** Round-trip kits, A unchanged, kick mask, 0/50/100% positions, rapid switching and clicks, CPU.

**Integration:** feature flag / opt-in, preserve legacy kits and all six voices, host regression + ARM CI + hardware checks before release. Do not touch USB audio in this sprint.

**Research references:** https://github.com/erica-synths/lxr-desktop-app (official architecture, CMake, memory map); https://github.com/brendanclarke/LXR02Open (CPU/interpolation concepts, inspect license before reuse); https://altstrument.org/devices/erica-synths-lxr-02 (community feature comparisons). No external code copied.
