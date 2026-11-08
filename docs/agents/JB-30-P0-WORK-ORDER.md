# JB-30: CI and ARM release gate

**Priority:** P0 release blocker. **Status:** assigned in GitHub; no autonomous execution or human assignee implied.

## Implementation
Own GitHub Actions test orchestration: run native menu and production DSP host tests, ARM build/link, memory report, SHA256, artifacts, exact commit and review gates. Diagnose failures from run 42462017630; candidate not release until hardware tests. Coordinate JB-24 and JB-20.

## Deliverables
1. Actual source/test changes on this branch, with reproducer and baseline measurements.
2. Host regression and ARM build evidence on exact SHA, including commands and logs.
3. Independent review by the named cross-workstream, with code comments and PR.
4. Zero regression in original six voices, kit loading, native bypass, no overclocking, no bootloader modification.

## Release rule
Unwanted crunch, clicks, clipping, underruns, or missed deadlines are **FAIL**. Keep firmware image marked unverified until physical LXR-02 audio tests pass. No auto-merge or fabricated signoff.
