# JIMBONES — CURRENT STATUS (single-screen recovery snapshot)

**This file is the canonical CURRENT STATE, not a history log.** Codex must **replace** this file at task start, after each meaningful step, on any blocker and before timeout. Keep under ~100 lines. Historical evidence goes into [CODEX_LIVE_CHECKPOINT.md](CODEX_LIVE_CHECKPOINT.md) as append-only records. Never delete history. Read [CODEX_HANDOVER_START_HERE_2026-11-10.md](CODEX_HANDOVER_START_HERE_2026-11-10.md) first for full safety and test protocol.

## State at creation (2026-11-10; before Codex execution)

- **Task:** P0 — execute source guards and real native host-only GCC/Clang `DrumVoice.c` syntax compilation.
- **Status:** BLOCKED / NOT RUN (no usable host checkout or runner available to current assistant).
- **Working branch:** `agents/jb-wavetable-host-experiment` (write only here).
- **Integration reference:** `agents/jb-28-full-integration-v186`, SHA `f797ec786fc57d008afeaabad964906f35a43177` (read only).
- **Pre-status isolated HEAD:** `ee90e1347fbcd6f83ff7999909f8a157a7ea3fe8`; **this status-file creation advances HEAD**. Read live SHA before executing.
- **Last verified code change:** `1c9690d302f54c0b3e178538dc518c5197668fed` — C06 source guard; GitHub source inspection PASS; Python execution NOT RUN.
- **Last known native CI:** run `42720355331`, FAIL on old SHA `6d3ae26f86b868c88bc6d9794a6431e5ed272f47`. Not evidence of latest source.
- **Actual test commands on current HEAD:** NONE executed here; all below **NOT RUN**.
- **Next command:** `python3 tests/test_c06_drum_scan_source.py` on checked-out isolated branch; then C12 guard, preflight, GCC, Clang, stopping on first failure.
- **Blocker:** no fresh Linux host runner / current GitHub Actions workflow execution.
- **Safety:** NO ARM compilation, .img, workflow re-enable, merge or flash. User has not approved exact frozen candidate. Preserve original six voices/kits, 132-sample No Clip, bouncing skull; Metallic FM shelved.

## Codex MUST update these fields each time (replace current values, do not append)

| Field | Value |
| --- | --- |
| UTC timestamp | NOT STARTED |
| Task ID / status | P0 / BLOCKED |
| Current branch HEAD (after last push) | RE-READ FROM GITHUB |
| Last source commit SHA | `1c9690d302f54c0b3e178538dc518c5197668fed` |
| Tested SHA | NONE |
| Environment / toolchain | NONE |
| Exact command / exit code | NOT RUN |
| Evidence (source / syntax / link / PCM) | Source inspection only; runtime NOT RUN |
| First failure / blocker | Fresh native host runner unavailable |
| Next single command | `python3 tests/test_c06_drum_scan_source.py` |
| Last checkpoint commit SHA | `ee90e1347fbcd6f83ff7999909f8a157a7ea3fe8` (historical handover, not a test) |
| Frozen candidate approval | NOT GRANTED |

## Write and recovery rules

1. At start: read this status file, guide and latest history entry; replace status with `STARTED`, task ID, SHA, intended command, and push **before editing**. Append the same event to history and push.
2. After each meaningful change/test: commit and push source/test files, replace this short status with actual command, exit code, tested SHA, first blocker and next command, push; append one concise history entry and push. A checkpoint's own resulting commit SHA is unknowable inside that same commit; record it in the *next* update or final user report.
3. On timeout or failure: push `BLOCKED` or `IN PROGRESS` with exact recovery command; do not claim PASS. New session starts here, checks `git status`, then reads latest history. Do not overwrite concurrent changes without fetching fresh blob SHA.
4. On task completion: update all four project tracking documents with exact source/test evidence, but **keep this file short** and point to history. Never replace the history with the snapshot.
