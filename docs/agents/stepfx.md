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
