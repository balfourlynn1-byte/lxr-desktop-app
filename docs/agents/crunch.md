# JB-03 modulation crunch — independent agent workstream

**Owners:** DSP + Verification
**State:** Research proposal, not autonomous agent execution or tested implementation.

## Improvement investigation
Build 1/2/3/4 modulated-voice comparison; inspect kitWeight shared balance, full mixer headroom, sample casting, retrigger continuity, post-FX and CPU overruns. Prefer isolated tests before modifying gain law; preserve kick and v1.8.02 output reference.

## Deliverables
1. Locate concrete integration points and relevant upstream references, with license review.
2. Define a measurable before/after fixture and performance budget.
3. Implement minimal isolated code on this branch; do not alter release branch directly.
4. Request review and run host/ARM CI. Hardware verification required for release.

## Safety
Keep original six voices and the working v1.8.02 untouched. Do not merge unverified firmware.

## Solution research handoff — 2026-11-08
**Task:** JB-03. **Status:** proposed engineering approach; not implemented or benchmarked unless independently recorded.

**Recommended design:** Separate three hypotheses: summed PCM clipping, per-layer saturation, and missed audio deadlines. Capture pre/post FX peak counts and DWT peaks for identical deterministic hits; only then choose headroom or DSP optimisation. No speculative limiter.

**Acceptance evidence:** Final mixer/post-FX tests plus recorded 4-pad hardware audio; compare v1.8.02 and experimental at matched loudness.

**Integration:** feature flag / opt-in, preserve legacy kits and all six voices, host regression + ARM CI + hardware checks before release. Do not touch USB audio in this sprint.

**Research references:** https://github.com/erica-synths/lxr-desktop-app (official architecture, CMake, memory map); https://github.com/brendanclarke/LXR02Open (CPU/interpolation concepts, inspect license before reuse); https://altstrument.org/devices/erica-synths-lxr-02 (community feature comparisons). No external code copied.
