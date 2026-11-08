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
