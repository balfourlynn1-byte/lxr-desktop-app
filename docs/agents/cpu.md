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
