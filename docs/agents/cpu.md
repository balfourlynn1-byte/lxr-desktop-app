# JB-02 CPU meter — independent agent workstream

**Owners:** Embedded + DSP
**State:** Research proposal, not autonomous agent execution or tested implementation.

## Improvement investigation
Measure whole audio callback using Cortex-M7 DWT CYCCNT; verify SystemCoreClock and buffer deadline, 32-bit wrap, ISR context; calculate peak and overruns with no heap or logging. Research alternative: GPIO pin timing on oscilloscope. Recommendation: instrument before adding DSP.

## Deliverables
1. Locate concrete integration points and relevant upstream references, with license review.
2. Define a measurable before/after fixture and performance budget.
3. Implement minimal isolated code on this branch; do not alter release branch directly.
4. Request review and run host/ARM CI. Hardware verification required for release.

## Safety
Keep original six voices and the working v1.8.02 untouched. Do not merge unverified firmware.

## Solution research handoff — 2026-11-08
**Task:** JB-02. **Status:** proposed engineering approach; not implemented or benchmarked unless independently recorded.

**Recommended design:** Use existing DWT CYCCNT render window and AudioDiagnostics counters, avoid duplicate ISR arithmetic. Record worst-case render cycles and deadline misses; expose UI snapshot only from non-audio thread. Queue pressure only if an actual queue occupancy signal exists.

**Acceptance evidence:** Check 1–4 voices, long decays, 0/100% load boundaries, cycle wrap; compare cycles with baseline; ARM CI + hardware.

**Integration:** feature flag / opt-in, preserve legacy kits and all six voices, host regression + ARM CI + hardware checks before release. Do not touch USB audio in this sprint.

**Research references:** https://github.com/erica-synths/lxr-desktop-app (official architecture, CMake, memory map); https://github.com/brendanclarke/LXR02Open (CPU/interpolation concepts, inspect license before reuse); https://altstrument.org/devices/erica-synths-lxr-02 (community feature comparisons). No external code copied.
