# START_HERE — JIMBONES Codex single-job packet

**One task only: JB-28A.** Do not spend this run researching the whole JIMBONES roadmap. Read this page, then the donor Comb design and only the files below. If a detail conflicts, prefer current code and test evidence; ask rather than inventing a mapping.

## Desired outcome
On `agents/jb-28-full-integration-v186`, integrate **existing Comb implementation from PR #15** into the latest skull/LCD and dub branch. Produce a **v1.8.07 hardware-unverified TEST ONLY** image *only if* host, ARM, >=512 B ITCM free, checksum/integrity all pass. Open draft PR against `agents/jb-26-skull-screensaver-v185`. Stop. **No JB-29, wavetable rewrite, or unrelated feature.**

## Known source of truth
- Comb donor: PR #15, commit `cb27f684a108a3fe257ac4e4dfe437a041fbe40b`, branch `agents/jb-12-comb-filter-v184`; donor test `tests/run_jb12_comb.sh`; donor design `docs/agents/jb12_comb_filter.md` **on donor branch**.
- Target: current branch `agents/jb-28-full-integration-v186`, descended from JB-26 skull branch; do not merge the donor branch wholesale.
- Existing working **on physical hardware**: Step FX audio core and dub delay/effect; keep untouched. Metallic FM remains disabled. v1.8.04 partial physical check: DSP ~62%, no reported crunch. v1.8.05/06, skull, combined Comb, full wavetable **not accepted on hardware**.
- Existing CI evidence: JB-26 560 B free ITCM; donor Comb 528 B free ITCM **on a different older branch**. Do not add these values or infer combined headroom.

## Minimum source map
| Concern | Inspect first |
|---|---|
| Comb DSP | donor `Core/Src/Soundengine/DSPAudio/CombFilter.c`, `Core/Inc/Soundengine/DSPAudio/CombFilter.h` |
| Existing filter dispatcher | `Core/Src/Soundengine/DSPAudio/ResonantFilter.c`, `Core/Inc/Soundengine/DSPAudio/ResonantFilter.h` |
| FILTER type menu | `Core/Inc/menu/MenuText.h` |
| ARM inclusion | `Core/CMakeLists.txt` (`lxr_dsp` source list) |
| Host test entry | `tests/run_host.sh` (currently calls StepFX/dub/wave/skull suites) |
| Comb focused tests | donor `tests/run_jb12_comb.sh`, `tests/test_jb12_comb.c`, `tests/test_jb12_comb_source.py` |
| Regression guard | current `tests/run_stepfx_ui.sh`, `tests/run_dub_delay_sync.sh`, `tests/run_jb26_skull.sh` |
| Image workflow/version | inspect current workflow and top-level CMake version only when packaging |

**Search discipline:** use `git diff --name-only`, `git diff -- <path>`, `rg -n 'specific_symbol' Core tests`. Do not dump all code, entire history or all PRs. Do not reread long plans.

## Execute in this order
1. Record exact base/donor SHAs and changed-file list. Inspect donor patch **by path**; transfer Comb-specific code only.
2. Keep existing filter IDs 0–7; Comb becomes ID 8. Reuse existing Freq/Reso/Drive; Reso=0 exact bypass; independent six-voice state; no audio-thread allocations. Verify no modifications to StepFX/dub/LCD beyond necessary integration.
3. Run **focused** `sh tests/run_jb12_comb.sh`. Fix a targeted failure, no broad refactor.
4. Run `sh tests/run_host.sh` with supported GCC and Clang setups; run ARM build and image integrity workflow. Fail if >=512 B ITCM free gate is not met. Do not weaken checks.
5. Package v1.8.07 only if green; include exact source SHA, image SHA256, bytes, ITCM used/free, test results, rollback to known-good v1.8.04.
6. Open one draft PR, report concise results using template below, and **STOP**. Hardware testing is a separate user action.

## Fail-fast
At most two attempts on the same concrete error. If still failing, stop with exact command, 10–20 relevant error lines, smallest proposed fix, and no image. Don't hide failures, invent test results, silently switch to another branch, overwrite known-good image or merge to main.

## Response template
```
JOB: JB-28A
STATE: BLOCKED | CODE_READY_HARDWARE_UNVERIFIED
BRANCH/COMMIT:
DONOR COMMIT:
CHANGED FILES:
FOCUSED HOST:
FULL HOST GCC/CLANG:
ARM:
ITCM USED/FREE:
IMAGE FILENAME / SIZE / SHA256: (only if validated)
DRAFT PR:
HARDWARE ACCEPTANCE: NOT TESTED
RISKS / NEXT ACTION: (max 3 bullets)
```

## Queued, not this run
JB-28B hardware regression and JB-29A Step FX three-group voice mask, then JB-29B four-knob RATE/DEPTH/CHAR. The user-approved UI spec lives on `agents/wavetable-research` in `docs/JIMBONES_JOB_TREE_V184.md`. Keep the scope isolated to avoid another compile-only feature iteration.
