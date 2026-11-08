# JB-27: Mixer-path instrumentation

**Priority:** P0 release blocker. **Status:** assigned in GitHub; no autonomous execution or human assignee implied.

## Implementation
Trace the exact native six-voice mixing, filter, FX, DAC and integer conversion chain; implement peak, saturation and overflow counters at each stage with negligible audio-thread overhead. Reproduce overlapping modulation crunch; coordinate JB-21 and JB-23.

## Deliverables
1. Actual source/test changes on this branch, with reproducer and baseline measurements.
2. Host regression and ARM build evidence on exact SHA, including commands and logs.
3. Independent review by the named cross-workstream, with code comments and PR.
4. Zero regression in original six voices, kit loading, native bypass, no overclocking, no bootloader modification.

## Release rule
Unwanted crunch, clicks, clipping, underruns, or missed deadlines are **FAIL**. Keep firmware image marked unverified until physical LXR-02 audio tests pass. No auto-merge or fabricated signoff.
