# JIMBONES Codex checkpoint — JB-28A

**Purpose:** durable progress log. Codex must update and PUSH this file during the job, not only at completion. A local-only edit is not a checkpoint. Record only facts actually observed; do not mark a step done before it is done.

## Current state
- Job: JB-28A Comb transplant into JB-26-derived branch
- Status: CODE PATCH PUSHED — CI PENDING, HARDWARE UNVERIFIED
- Last confirmed pushed checkpoint: initial template (prepared by assistant, not Codex)
- Working branch: `agents/jb-28-full-integration-v186`
- Current base SHA: prior confirmed docs commit `2cc2d1aa2a6595ec48b7e46cf47ee1eb684268b3`; live HEAD requires verification
- Comb donor SHA: `cb27f684a108a3fe257ac4e4dfe437a041fbe40b`
- Hardware status: NOT TESTED
- Latest verified firmware candidate: NONE; v1.8.07 configured, image NOT YET VALIDATED

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

### 2026-11-08 UTC — Comb integration source and CI gate pushed
- PR #15 donor mapped; nine source/test files transplanted or adapted. Core/CMakeLists.txt registers CombFilter.c while retaining latest source list; tests/run_host.sh invokes focused Comb and retains skull tests.
- CMake project version set to 1.8.07; workflow `.github/workflows/jimbones-jb28-comb-integration.yml` added with focused, GCC/Clang, ARM, >=512 B ITCM, updater validation, manifest and hardware-unverified artifact gates.
- Source and workflow pushed through commit `6bf88b1f4812d29dcfdb3840eb0fbf36c149521f`.
- Draft PR #18: https://github.com/balfourlynn1-byte/lxr-desktop-app/pull/18 against JB-26 branch.
- GitHub workflow lookup for that SHA returned zero PR-triggered runs; combined commit statuses returned none. **Tests NOT OBSERVED / NOT PASSED; no firmware image or ITCM evidence yet.** No local clone/compiler was available.
- Next: inspect https://github.com/balfourlynn1-byte/lxr-desktop-app/actions/workflows/jimbones-jb28-comb-integration.yml for push-triggered run, fix any failure; then verify image and complete device acceptance separately.

### 2026-11-08 UTC — CI execution visibility blocker
- Checked GitHub PR-triggered runs and commit statuses for `a82d630432b11365fecb65843c8c096e4b9ed9f4`: both empty. This does **not** prove no push-triggered run exists; connector query filters to PR runs.
- Added `pull_request` trigger for JB-26 target branch to dedicated JB-28 workflow; pushed `4e3b162b731bafdffa08e2eed8cdfc2bdbc8ec47`. Immediately queried PR-triggered runs: none returned yet.
- Container Git clone blocked by DNS (`Could not resolve host: github.com`); no ARM compiler found. Therefore no local compile, CI results, ITCM, image SHA or test pack verified. No device flash.
- STATUS: CI EVIDENCE BLOCKED / AWAITING ACTIONS. Next: inspect Actions workflow link and PR #18 checks; obtain actual run ID, host and ARM results; fix only concrete failing gate. Do not claim CODE_READY until all gates green.

### 2026-11-08 UTC — first real CI failure diagnosed and fixed
- GitHub Actions PR #18 run `42511490948` completed FAILURE. Host job `128825326193`: focused Comb regression PASS (zero-reso, six voices, 384k samples, source checks). Existing send/dub/Metallic-off tests passed before failure.
- First failing gate: `tests/test_jb22_integration.py:17` hard-coded `VERSION 1.8.06`; target intentionally now `VERSION 1.8.07`. ARM job skipped; no memory/image evidence.
- Updated that exact assertion to 1.8.07, pushed `442d8be8dfcc22195be2c49b1bededd401c628aa`. No DSP implementation changed in this fix.
- Next: inspect new PR run for `442d8be8` or later, follow next concrete failure; no image approved.

### 2026-11-08 — USER-ORDERED ARM BUILD HOLD
- User testing v1.8.06 reports firmware not nearly ready; prefers a substantially more developed v1.8.08 candidate rather than incremental ARM builds.
- ARM and test-artifact job in `.github/workflows/jimbones-jb28-comb-integration.yml` disabled with unconditional `if: ${{ false }}` at commit `5b0dd2e3f23885dedb4a69c13ebb6a16c0dad4d7`. Host-only tests remain enabled.
- Do NOT re-enable ARM, produce a firmware image or label a candidate hardware-ready until user approves the chosen feature-frozen build and all host checks pass.
- Version 1.8.07 is a source-integration intermediate, NOT the next hardware test target; v1.8.08 is a proposed future target, pending scope and user acceptance. Existing older job logs may still exist.
- Next: gather specific v1.8.06 hardware shortcomings, prioritize meaningful improvements, finish and validate host-side integration before requesting ARM build approval.
