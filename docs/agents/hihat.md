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

## Solution research handoff — 2026-11-08
**Task:** JB-10. **Status:** proposed engineering approach; not implemented or benchmarked unless independently recorded.

**Recommended design:** Parse FD-8/Ableton CC4 and note stream in MIDI path; implement open/closed pedal state and choke transitions with short release ramp. Keep mapping configurable and preserve default note behavior.

**Acceptance evidence:** MIDI event replay: CC4 threshold, repeated notes, open-to-closed choke, velocity extremes, device pedal.

**Integration:** feature flag / opt-in, preserve legacy kits and all six voices, host regression + ARM CI + hardware checks before release. Do not touch USB audio in this sprint.

**Research references:** https://github.com/erica-synths/lxr-desktop-app (official architecture, CMake, memory map); https://github.com/brendanclarke/LXR02Open (CPU/interpolation concepts, inspect license before reuse); https://altstrument.org/devices/erica-synths-lxr-02 (community feature comparisons). No external code copied.
