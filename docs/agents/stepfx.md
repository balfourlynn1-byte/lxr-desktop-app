# JB-07 step performance FX — independent agent workstream

**Owners:** DSP + Embedded
**State:** Research proposal, not autonomous agent execution or tested implementation.

## Improvement investigation
Use fixed-size ring buffers, transport-clock tick scheduling, bounded feedback and explicit kick exclusion. Start with short trigger-repeat before 2/4/8 bar delay storage; calculate RAM requirements and test discontinuities.

## Deliverables
1. Locate concrete integration points and relevant upstream references, with license review.
2. Define a measurable before/after fixture and performance budget.
3. Implement minimal isolated code on this branch; do not alter release branch directly.
4. Request review and run host/ARM CI. Hardware verification required for release.

## Safety
Keep original six voices and the working v1.8.02 untouched. Do not merge unverified firmware.

## Solution research handoff — 2026-11-08
**Task:** JB-07. **Status:** proposed engineering approach; not implemented or benchmarked unless independently recorded.

**Recommended design:** Prefer retrigger/event scheduling for 2/4/8-bar rhythmic patterns before allocating multi-bar audio delay RAM. Use sequencer tick and fixed small buffer; explicit kick exclusion and bounded feedback; step 1 bypass, step 16 LFO toggle.

**Acceptance evidence:** Transport start/stop, BPM changes, off-grid hits, kick protection, SRAM and timing.

**Integration:** feature flag / opt-in, preserve legacy kits and all six voices, host regression + ARM CI + hardware checks before release. Do not touch USB audio in this sprint.

**Research references:** https://github.com/erica-synths/lxr-desktop-app (official architecture, CMake, memory map); https://github.com/brendanclarke/LXR02Open (CPU/interpolation concepts, inspect license before reuse); https://altstrument.org/devices/erica-synths-lxr-02 (community feature comparisons). No external code copied.
