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
