# CODEX START — JB-28A (single execution contract)

**Read this file + `docs/CODEX_CHECKPOINT_JB28A.md` only.** For exact external references use `docs/CODEX_SOURCE_REGISTER.md`; open `docs/CODEX_COMPREHENSIVE_HANDOVER.md` **only for a specific unresolved question**. Do not scan the whole repository, read all docs, or start queued jobs.

## Goal
Transplant **Comb only** from [PR #15](https://github.com/balfourlynn1-byte/lxr-desktop-app/pull/15), pinned donor `cb27f684a108a3fe257ac4e4dfe437a041fbe40b`, into `agents/jb-28-full-integration-v186` (JB-26/#17 lineage). Do **not** merge the donor branch wholesale. Output one draft PR targeting `agents/jb-26-skull-screensaver-v185`; produce **v1.8.07 HARDWARE_UNVERIFIED** image only if all gates pass. Never merge, flash, or overwrite known-good v1.8.04.

## First 3 commands / actions
1. `git status --short && git rev-parse HEAD && git log -3 --oneline` — verify correct branch, clean tree, actual SHA. If dirty, inspect before modifying.
2. Read last pushed checkpoint; determine whether source mapping or implementation is already complete. **Do not repeat finished work.**
3. Write stage, actual SHA and next action to checkpoint; `git add docs/CODEX_CHECKPOINT_JB28A.md docs/JIMBONES_WORK_QUEUE.md && git commit -m 'docs: checkpoint JB-28A start' && git push`. If no change, do not make an empty commit. Verify remote push; if push fails twice, report NOT PERSISTED.

## Scope — minimum file set
Donor: `Core/{Src,Inc}/Soundengine/DSPAudio/CombFilter.{c,h}` (check actual paths), `tests/run_jb12_comb.sh`, `tests/test_jb12_comb.c`, `tests/test_jb12_comb_source.py`, `docs/agents/jb12_comb_filter.md`. Target: `Core/{Src,Inc}/Soundengine/DSPAudio/ResonantFilter.{c,h}`, `Core/Inc/menu/MenuText.h`, `Core/CMakeLists.txt`, `tests/run_host.sh`. Open extra files only on a specific dependency.

**Contract:** existing filter IDs 0–7 unchanged; Comb ID 8; six independent bounded delay histories; existing Freq/Reso/Drive; Reso=0 exact PCM bypass; no dynamic allocation in render; no unrelated Step FX, dub, wavetable, skull, preset-format or original-FM edits. Metallic FM stays disabled. Verify actual source symbols and voice mapping, don't assume.

## Six checkpoints / gates
| Stage | Required output, then PUSH |
|---|---|
| 0 START | Branch SHA, donor SHA, next action |
| 1 MAP | Donor changed-file diff, exact target symbols, minimal transplant plan, risk |
| 2 PATCH | Small Comb-only source commit, WIP status if incomplete |
| 3 FOCUSED | `sh tests/run_jb12_comb.sh` PASS; record command/result |
| 4 FULL | `sh tests/run_host.sh` with GCC + Clang, ARM cross-link, >=512 bytes free ITCM, updater checksum/integrity; record actual commands and results |
| 5 DELIVER | Only if all gates green: SHA256/size/source SHA for v1.8.07, draft PR, hardware NOT TESTED, next physical checklist |

**Before every expensive full build**, commit/push source and checkpoint. **After any failed test**, push concise failure and next action. Maximum two attempts on the same error; then BLOCKED, no image. Do not weaken gates. Never label CI PASS as device PASS.

## Physical truth
User reports v1.8.04 boots, ~62% DSP after Metallic FM removal, existing Step FX effects and dub work. Pitch Step FX musically limited; targeting not implemented. New Comb, five dub timing pads, skull and native scan not physically accepted. Old Metallic branch crunched at 94–101% DSP. Preserve known-good audio and kick protection.

## Required end report (<=15 lines)
```text
JOB JB-28A | STATE BLOCKED / CODE_READY_HARDWARE_UNVERIFIED
BRANCH + PUSHED SHA:
DONOR SHA:
FILES CHANGED:
FOCUSED:
HOST GCC/CLANG:
ARM + ITCM FREE:
IMAGE SHA256 + BYTES: [only if fully validated]
DRAFT PR:
CHECKPOINT PUSHED SHA:
HARDWARE: NOT TESTED
BLOCKER / EXACT NEXT ACTION:
```

## Resume / other work
Resume from the last **pushed** checkpoint, not this chat. Detailed persistence rules: `docs/CODEX_PERSISTENCE_PROTOCOL.md`. Branch-local queue: `docs/JIMBONES_WORK_QUEUE.md`; historical master tree is on `agents/wavetable-research` and does not sync automatically. **Queued, not this run:** JB-28B hardware validation; JB-29A per-slot voice masks; JB-29B four-knob UI; separate existing JB-27 native wave branch.
