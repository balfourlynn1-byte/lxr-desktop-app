# JIMBONES Codex checkpoint — JB-28A

**Purpose:** durable progress log. Codex must update and PUSH this file during the job, not only at completion. A local-only edit is not a checkpoint. Record only facts actually observed; do not mark a step done before it is done.

## Current state
- Job: JB-28A Comb transplant into JB-26-derived branch
- Status: PATCHING — PARTIAL, NOT TESTED
- Last confirmed pushed checkpoint: initial template (prepared by assistant, not Codex)
- Working branch: `agents/jb-28-full-integration-v186`
- Current base SHA: prior confirmed docs commit `2cc2d1aa2a6595ec48b7e46cf47ee1eb684268b3`; live HEAD requires verification
- Comb donor SHA: `cb27f684a108a3fe257ac4e4dfe437a041fbe40b`
- Hardware status: NOT TESTED
- Latest verified firmware candidate: NONE for combined v1.8.07

## Append-only progress log
Each update must include UTC time, pushed commit SHA, stage, files inspected/changed, finding or blocker, tests and exact next command. Keep newest entries at bottom. Never erase previous checkpoints.

### Initial checkpoint — preparation
- The one-page task is in `docs/CODEX_START_HERE.md`.
- Expected target: Comb appended as filter ID 8 without changing IDs 0–7 or existing dub/skull behaviour.
- No Codex implementation or new image has been verified.

## Discoveries and decisions
- Record symbol paths, actual API signatures, memory bottlenecks, compatibility constraints and rejected approaches here with source locations. Keep concise and actionable.
- **Do not claim a code change is implemented unless its commit SHA is pushed.**

## Test ledger
| Stage | Command / CI run | Result | Commit |
|---|---|---|---|
| Donor diff reviewed | | NOT RUN | |
| Focused Comb host test | `sh tests/run_jb12_comb.sh` | NOT RUN | |
| Full host GCC/Clang | `sh tests/run_host.sh` | NOT RUN | |
| ARM build | | NOT RUN | |
| ITCM >=512 B free | | NOT RUN | |
| Updater checksum | | NOT RUN | |
| Hardware | user physical test | NOT RUN | |

## Resume instructions for next Codex session
1. Read `docs/CODEX_CHECKPOINT_JB28A.md` and `docs/CODEX_START_HERE.md`, not the full conversation.
2. Inspect `git status` and `git log -3 --oneline`, verify the last **pushed** checkpoint SHA.
3. Resume from the exact pending step below, do not redo completed research without a concrete inconsistency.
4. If a code change is incomplete, mark it PARTIAL and give the next specific edit/test. Never mark hardware accepted from CI.

**Next action:** inspect PR #15 Comb-specific diff and target file content; map smallest transplant. No local git checkout or compilation available yet.

### 2026-11-08 UTC — assistant checkpoint (source mapping start)
- GitHub branch exists: `agents/jb-28-full-integration-v186`; checkpoint was previously initial template.
- PR #15 lists 12 changed paths: CombFilter.{c,h}, ResonantFilter.{c,h}, MenuText.h, Core/CMakeLists.txt, 3 Comb test files, run_host.sh, CI workflow and donor design.
- Connector access verified; local clean worktree **NOT VERIFIED** (no local clone). No tests run, no firmware generated.
- Next: inspect donor patch and current target, then checkpoint exact compatibility plan.

### 2026-11-08 UTC — source transplant partial checkpoint
- Inspected PR #15 complete patch and current target: ResonantFilter.h/.c and MenuText.h target blob SHAs matched PR donor's base blobs, so donor versions transplanted without overwriting later edits to those paths.
- Pushed CombFilter.h/.c, three focused tests, ResonantFilter.h/.c, MenuText.h (latest source commit `23abbb7d3e32b9bb11db91a48ca2494cd3445aaf`).
- Remaining integration: Core/CMakeLists.txt add CombFilter.c; tests/run_host.sh add focused suite. Then focused/full/ARM CI. No compilation or hardware test yet; firmware NOT READY.
- Exact next action: modify the two remaining existing files preserving newer skull test and engine additions; inspect resulting diff, run tests via CI or checked-out build environment.
