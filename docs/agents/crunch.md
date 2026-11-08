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
