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

### 2026-11-08 — user-directed wavetable source investigation (research only)
- User confirms personal home-only use; prioritizes upstream Brendan Clarke waveform behavior, original 3 ms No Clip fade, and expanded velocity modulation over previous experimental oscillator.
- Read upstream `Oscillator.c`, `Oscillator.h`, `OSC_INTERP_AUDIT.md` and `LICENSE` via GitHub; compared against current native oscillator and JB-27 experimental branch.
- Pushed pinned audit and phased compatibility/test plan at `docs/JB_WAVETABLE_UPSTREAM_PORT_PLAN.md`, commit `504a936125b75e81640d795ddc1435ce0f3d50e8`.
- No upstream code copied, no new DSP implementation, no ARM build. Existing user ARM hold remains in force.
- Next: locate original No Clip 3ms fade exact source and tests; inspect upstream modulation target write path, then map implementation on isolated development branch.

### 2026-11-08 UTC — atomic B01 upstream commit and licence pinned
- Upstream `brendanclarke/LXR02Open` `prime` resolved to commit `c896bfd0efe86a4037418bf9f46f1fcc39255958`; fetched six exact-commit blobs including LICENSE, oscillator, modulation and interpolation audit. Blob hashes recorded in `docs/JB_WAVETABLE_UPSTREAM_PORT_PLAN.md` (pushed evidence commit `a5c51dc2a60b25108ca8689593e81932c5629711`).
- Licence expressly forbids commercial use and requires modified-source redistribution; no upstream code imported. No host or ARM tests run.
- Next: A01 locate original 3 ms No Clip source and compare to v1.8.06; continue B04 architecture mapping if A01 history inaccessible.

### 2026-11-08 UTC — A01 original No Clip source found, history pending
- Historical branch `jimbones-noclip-baseline` contains the exact 132-sample (~2.99 ms) retrigger smoother. Header blob `2818d420`, C blob `b8b480d6`, DrumVoice blob `6d276b54`. The 3 primary drums capture active envelope before note-on and apply correction after voice render; snare/hat are not covered by this code.
- Full paths, lines, and regression requirements are recorded in `docs/CODEX_HANDOVER_2026-11-10.md`. GitHub commit-message search found no introducing SHA, so A01 remains PARTIAL.
- Next command in a checkout: `git log --follow --format='%H %ad %s' --date=iso -- Core/Src/Soundengine/DSPAudio/RetriggerSmoother.c` on `jimbones-noclip-baseline`; inspect oldest introducing diff and compare with v1.8.06. No tests, ARM, image or hardware verification.

### 2026-11-08 UTC — atomic A02 source comparison
- Compared historical baseline with JB-26 skull lineage and JB-28 current branch: identical smoother C/header blobs, enabled config, three-drum call sites; JIMBONES `jb_trigger` and `jb_preFilterNoise` added around the original path.
- Full SHA/line references in `docs/CODEX_HANDOVER_2026-11-10.md`. Source-only DONE, audio tests NOT RUN; no ARM/image activity. Next: A03 132-sample focused host regression; A01 earliest introducing commit remains unresolved.

### 2026-11-08 — B04 oscillator compatibility research checkpoint
- Pushed source-only compatibility map `docs/JB_B04_OSCILLATOR_COMPATIBILITY_MAP.md` at `ed00f8c97fa952935115d6f14a38df94edd4bf27`. Erica PWM=5 conflicts with upstream CRASH=5; `OscInfo` layouts, sample caches, and native modulation paths are not binary-compatible.
- Test ledger: source audit VERIFIED via exact GitHub file blobs; native GCC/Clang, WAV, ARM and hardware NOT RUN. Next B05 velocity destination routing, bounds and default-OFF tests. ARM hold unchanged.


### 2026-11-09 B05
Velocity destination research completed, see `docs/JB_B05_VELOCITY_DESTINATION_MAP.md`: six upstream/native velocity nodes; wave-position and shape recommended default OFF; drive deferred. Source-only, tests NOT RUN. ARM hold remains.

### 2026-11-09 UTC — B07 source/licence architecture decision
- Reviewed pinned upstream `LICENSE` at `c896bfd0efe86a4037418bf9f46f1fcc39255958`, blob `635ea7ee4e54922c03fad5795ba7c734048857b9`, plus previously pinned native/upstream oscillator ABI maps.
- Selected independently authored Erica-native bounded interpolation for public GPL-only branch; no upstream noncommercial source transplant. Decision evidence `docs/JB_B07_LICENCE_ARCHITECTURE_DECISION.md`, commit `8f6ea7de38a51006c037494cbdcd80b91a929938`.
- Tests: NOT RUN; code: documentation only; ARM: HELD. Next unblocked work: A03 GCC/Clang host execution if checkout available, or B06 remaining source mapping.


### A04 source trace — 2026-11-09
Pinned approved skull and wake paths in `docs/JB_A04_SKULL_PRESERVATION_MAP.md` (commit `098ff972`); source-only DONE, host tests NOT RUN. ARM HOLD remains.


### 2026-11-09 B06b checkpoint
Completed native sample-dispatch source audit: oscillator IDs 0–5 active, default sample dispatch commented, sample helpers `#if 0`; 32768-byte crash asset is not proof of playable sample support. Evidence `docs/JB_B06B_SAMPLE_DISPATCH_GATE.md` commit `3cbb1fb2`. B06 PARTIAL; tests NOT RUN; ARM held. Next: upstream sample memory and serialization mapping or A03 host execution.

### 2026-11-09 UTC — B06c native MIDI/clock audit
- Pushed `59084b2f78dc5ffd3d4313e68fa567839bbdb923`: `docs/JB_B06C_MIDI_CLOCK_COMPATIBILITY.md` records exact native MIDI parser blob/line map, clock filter gates, six velocity modulation destinations, NRPN bounds and deterministic host test specifications.
- Test status: NOT RUN (no full host checkout); no ARM/image/hardware work. B06 PARTIAL; next research: upstream MIDI/clock and native preset serialization.


### 2026-11-09 D01 verified historical comb CI
GitHub run [42512010361](https://github.com/balfourlynn1-byte/lxr-desktop-app/actions/runs/42512010361), SHA `05e1722`: focused Comb, full host GCC and Clang, standalone Step FX all SUCCESS (host job `128826896630`). Separate artifact job `128826975121` failed at `tests/jb23_candidate_manifest.py:38`: `ValueError: Unexpected version for JB-23 candidate: 1.8.07`; artifact skipped. Root cause is manifest version validation, not proven comb DSP failure. Exact evidence `docs/JB_D01_COMB_CI_EVIDENCE.md`, commit `6351b76c`; D01 checklist update `137efcce`. Historical CI evidence only; latest HEAD and hardware NOT VERIFIED; no ARM work performed. Next D02 Step 1 LED/Step FX source test or A03 host execution when checkout available.

**H03 2026-11-09 PARTIAL source audit:** upstream morph MIDI 0..127 maps to 0..254 via `value<<1`, with six LFO and velocity amount CC routes, original-value reset and per-block interpolation mapping. Pinned blob evidence and host-only test matrix: `docs/JB_H03_MORPH_VELOCITY_ROUTE.md`, commit `30dc4e77`. Native morph precedence and kit/global serialization still UNVERIFIED; tests NOT RUN; H03 OPEN. ARM HOLD.

**H03 2026-11-09 native MIDI contract check:** exact GitHub source blob `04f3d03ffcee0b20f6d58e26d7a91f34e4b89bca` inspected: native `CC_MOD_WHEEL` branch is **no-op**, unlike upstream `value<<1` morph; six velocity amount CC cases dispatch to `velocityModulators[]` using normalized value. Source assertions executed during inspection (both TRUE). Committed executable Python source guard `tests/test_h03_native_midi_contract.py` (`26ff895b`) and host runner wiring (`32fce496`). Python test itself/full GCC-Clang suite **NOT RUN**; no local checkout. This is an important upstream/native compatibility difference; no morph behavior change implemented. H03 remains PARTIAL, ARM HOLD.

**H03 native morph safety guard 2026-11-09:** Added executable source contract `tests/test_h03_native_morph_contract.py` (`356a0a21`) and wired into `tests/run_host.sh` (`04275031`). It checks alternate morph kit buffer load, morph destination bookkeeping, source-kit audio-routing exclusion and existing velocity nodes, based on `Core/Src/menu/menu.c` blob `6282d73b`. Test NOT RUN (no local checkout); no audio/preset round-trip proof. H03 PARTIAL; next inspect `preset_morph` implementation/serialization and execute GCC/Clang host tests. ARM HOLD.

**H03 2026-11-09 morph slot contract:** Inspected `Core/Src/menu/menu.c` blob `6282d73b`: morph save uses `preset_saveDrumsetRam(...,1)`; morph load uses `preset_loadDrumsetRam(...,1,0)`, updates `PAR_MORPH_KIT_DEST` and `CURRENT_MORPH`, and morph rendering excludes audio-routing parameters. Added executable source guard `tests/test_h03_morph_slot_contract.py` (`21b4be75`, anchor correction `999d4e0f`) and wired into host runner (`eb916590`). Equivalent source predicates checked TRUE during connector inspection; committed Python test/GCC/Clang NOT RUN. Actual serializer body, round-trip, corruption and hardware remain unverified. H03 PARTIAL; ARM HOLD.

**H03 2026-11-09 morph RAM/SD route guard:** `Core/Src/menu/menu.c` morph save case calls `preset_saveDrumsetRam(...,1)` and contains no direct SD save; morph load calls `preset_loadDrumsetRam(...,1,0)` and updates destination/current morph. Added executable source guard `tests/test_h03_morph_ram_route.py` (`76df5649`, corrected case selection `c7a2b00b`) and wired `tests/run_host.sh` (`f2d32455`). Important: **this does NOT establish power-cycle persistence or prove that the serializer cannot write indirectly**. Python test/GCC/Clang NOT RUN; H03 OPEN; next inspect actual preset implementation and add round-trip fixture. ARM HOLD.

**H03 2026-11-09 native kit morph-save isolation (candidate FIX, NOT RUN):** `Core/Src/Preset/presetManager.c` blob `1b91b342` unconditionally copied `jb_parameters` into `jb_kits[presetNr]` at entry to `preset_saveDrumsetRam`, even when `isMorph=1`. That could overwrite the saved native extension of the morph target. Commit `8f37e730` moves the copy into the non-morph branch, preserving normal kit saves and morph parameter interpolation. Added focused source guard `tests/test_h03_morph_kit_extension_guard.py` (`83ffbc64`, correction `a3ac1ce2`) and wired into `tests/run_host.sh` (`3c7473c8`). Post-change connector source inspection confirms exactly one extension memcpy in the non-morph branch; **Python test and full GCC/Clang suite NOT RUN**, no full checkout. Need behavioral two-slot RAM round-trip, SD serialization audit and CI before marking H03 DONE. Six voices, 3ms No Clip, skull untouched; ARM HOLD.

**H03 2026-11-09 SD extension contract (PARTIAL, NOT RUN):** inspected `Core/Src/Preset/presetManager.c` SD save/load paths: normal project loop calls `preset_saveDrumsetSD(i,0)` and load `preset_loadDrumsetSD(i,0)`; normal save packs `jb_kits[presetNr]` and normal load unpacks it. Morph SD save, if invoked, writes a packed *live* `jb_parameters` extension but morph SD load explicitly skips extension; asymmetry requires policy review, not proof of corruption in normal project flow. Added executable source guard `tests/test_h03_sd_extension_contract.py` (commits `76defea5`, `832038f8`), wired to host runner (`9bec28a9`). Checkout failed DNS resolution (`Could not resolve host: github.com`), so Python/GCC/Clang **NOT RUN**. Need actual two-slot RAM/SD round-trip, legacy record truncation and failure injection; H03 OPEN; ARM HOLD.

**A03 2026-11-09 additional executable edge fixture (NOT RUN):** `tests/test_noclip_132.c` now exercises both full-scale opposite-polarity retriggers (-32768 to +32767 and reverse), first-sample continuity, final ramp endpoint within one LSB, fade completion and post-fade exact output. Commit `fdbcd18620f46cda4d43586d3ca33d662384ab4f`. Existing `tests/run_host.sh` already invokes this test; no runner change needed. Native checkout via git failed DNS (`Could not resolve host: github.com`), so GCC/Clang execution NOT RUN and A03 remains OPEN. Source is host-only; no ARM build/image/merge. Next run GCC/Clang host suite on exact SHA and record output.


**2026-11-09 H03 CI guard repair (PENDING rerun):** PR #18 run [42663528291](https://github.com/balfourlynn1-byte/lxr-desktop-app/actions/runs/42663528291) checked out merge SHA `bebb4a6c`; Comb focused test passed, GCC host runner stopped at `tests/test_h03_native_morph_contract.py` because its regex required `{` immediately followed by `morph = 0`, but native `Core/Src/menu/menu.c` has a legitimate explanatory comment between them. Updated guard to permit comments without crossing braces in commit `3cd0f13845f4c04f366122cbacb061ddb1d2d928`. This is a source-guard correction, **not a DSP fix**. CI has **not been rerun**; H03 and A03 remain OPEN; Clang skipped, no hardware verification. ARM HOLD unchanged.

### 2026-11-09 UTC — wavetable C02 local audio fixture and CI setup
- Isolated experiment branch: `agents/jb-wavetable-host-experiment`; C01 `20b34317525e789927cf2b98582b8016c5e6d0e6`; C02 fixture `3823de5ff81a804eb69a5952e8dda32b63a7c1ec`; CI workflow `614f2ead51c0ba554605e5039917417900e987a0`.
- Local run: `python3 test_c02_wavetable_audio.py` PASS, 22070 frames 44140 Hz mono 16-bit; SHA-256 `20396bc4f995544882b27bb49b0b68f001a3093b2822012818062bd93b41a94c`. No Actions run observed; no browser test.
- **C02 PARTIAL:** generated standalone audio only. Native oscillator OFF/bypass exact sample identity, GCC/Clang native host integration and emulator playback NOT TESTED. Next implement native bypass fixture; no ARM build.

### 2026-11-09 — C03 native wavetable kernel, isolated host PASS
- Independently authored `Core/Inc/Soundengine/DSPAudio/JimbonesNativeWaveScan.h` committed `4d1023994207c3237079b5d1a619437845eff61e`; test `tests/test_c03_native_wave_scan.c` committed `e21c5d99816f802ed8c5f6d6f01883fb144df9db` and NAN fix `c2ca319ccece723efed393c96034c614af81175b`; runner `tests/run_c03_native_wave_scan.sh` committed `b6dbe664bf98f6de66db7602ac7306af5cb9e5a0`.
- Local focused GCC and Clang C11 `-O2 -Wall -Wextra -Werror -fsanitize=undefined,float-cast-overflow` checks PASS for scan position, interpolation, clamping and NaN. Executed an equivalent locally reconstructed test source/header; exact GitHub runner not independently run. No native oscillator dispatch connection or legacy OFF comparison yet; no emulator test. Next task is guarded integration with OFF bit-identity tests, preserving all original voices/No Clip/skull. No ARM build.
