# JB-10 hi-hat dynamics — independent agent workstream

**Owners:** Embedded + Verification
**State:** Research proposal, not autonomous agent execution or tested implementation.

## Improvement investigation
Document FD-8 CC/note mapping through Ableton, define choke and open/closed state machine; test repeated strikes and quiet/loud velocity response with deterministic MIDI sequences.

## Deliverables
1. Locate concrete integration points and relevant upstream references, with license review.
2. Define a measurable before/after fixture and performance budget.
3. Implement minimal isolated code on this branch; do not alter release branch directly.
4. Request review and run host/ARM CI. Hardware verification required for release.

## Safety
Keep original six voices and the working v1.8.02 untouched. Do not merge unverified firmware.
