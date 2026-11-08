# JB-11 snare randomisation — independent agent workstream

**Owners:** DSP + Embedded
**State:** Research proposal, not autonomous agent execution or tested implementation.

## Improvement investigation
Use seeded per-voice PRNG and per-hit engine selection with probability; avoid random changes mid-envelope, provide stable backbeat guard and deterministic regression tests.

## Deliverables
1. Locate concrete integration points and relevant upstream references, with license review.
2. Define a measurable before/after fixture and performance budget.
3. Implement minimal isolated code on this branch; do not alter release branch directly.
4. Request review and run host/ARM CI. Hardware verification required for release.

## Safety
Keep original six voices and the working v1.8.02 untouched. Do not merge unverified firmware.

## Solution research handoff — 2026-11-08
**Task:** JB-11. **Status:** proposed engineering approach; not implemented or benchmarked unless independently recorded.

**Recommended design:** Use seeded xorshift PRNG evaluated only on snare trigger, configurable 0–100% probability; pick B/C layer or engine before envelope starts, never mid-tail; optional backbeat lock.

**Acceptance evidence:** Seed repeatability, 0/100% edges, backbeat lock, kit persistence, six voices.

**Integration:** feature flag / opt-in, preserve legacy kits and all six voices, host regression + ARM CI + hardware checks before release. Do not touch USB audio in this sprint.

**Research references:** https://github.com/erica-synths/lxr-desktop-app (official architecture, CMake, memory map); https://github.com/brendanclarke/LXR02Open (CPU/interpolation concepts, inspect license before reuse); https://altstrument.org/devices/erica-synths-lxr-02 (community feature comparisons). No external code copied.
