# JIMBONES P0 execution queue — next ARM candidate

**2026-11-08 — assigned work orders, not autonomously executing agents.** GitHub Issues are disabled in this repository (API 410), so this file and the dedicated branches serve as the authoritative queue.

| Order | Workstream | Branch | Definition of done | Review owner |
|---|---|---|---|---|
| 1 | JB-21 — crunch root cause | `agents/jb-21-specialist` | 2/3/4 modulated voices reproducible, pre/post mixer measurements, minimal patch, PCM regression | JB-23 |
| 2 | JB-22 — CPU deadline instrumentation | `agents/jb-22-specialist` | max cycles/deadline misses recorded with 4 long-tail voices, low-overhead telemetry, overload regression | JB-23 |
| 3 | JB-23 — independent No Clip tests | `agents/jb-23-specialist` | bypass parity, PCM peaks/rails, click/pop tests, cross-review of JB-21/22 evidence | JB-20 |
| 4 | JB-24 — host/ARM build integration | `agents/jb-24-specialist` | exact-commit host tests PASS, ARM compile/link PASS, flash artifact checksum, memory budget and full CI evidence | JB-20 |
| 5 (after P0) | JB-25 — Morph A/B/C | `agents/jb-25-specialist` | immutable A, editable B/C, kick exclusion, save/load and modulation tests | JB-13 / JB-23 |
| 6 (after P0) | JB-26 — DSP optimisation | `agents/jb-26-specialist` | measurable CPU savings without native sound drift | JB-22 / JB-23 |

## Immediate handoff
JB-21 must submit actual reproducer and fix first; JB-22 can instrument in parallel. JB-23 establishes baseline and independently reviews both. JB-24 begins host build diagnosis immediately, but does not promote to an ARM hardware-test candidate until audio and source gates are satisfied. JB-25 and JB-26 are queued, not authorized to increase feature risk in the P0 release branch.

## Required proof for promotion
- Named independent GitHub code review, not just work-order documentation.
- Host tests and ARM CI for the **same exact commit**, with logs and hashes.
- All six original voices, old kit load, bypass identity and multi-voice long-tail audio verified.
- No Clip remains UNVERIFIED until physical LXR-02 listening and rail/deadline measurement. Any unintended crunch/clipping/pops/dropout is FAIL.

**Operational limitation:** these branches and assignments do not start autonomous background workers. Code and test work must be performed in a live development session or by connected collaborators. No GitHub issue or assignee was created because Issues are disabled in this repository.
