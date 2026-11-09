# JIMBONES project tree — active path to next full hardware candidate

Updated 2026-11-08. **User is testing v1.8.06 and finds it far from ready. v1.8.08 is a proposed feature-rich target, not a guaranteed release. ARM builds and image packaging are on HOLD until user explicitly approves a feature-frozen candidate.**

## Foundation decision
- **Primary research reference:** [Brendan Clarke's LXR02Open](https://github.com/brendanclarke/LXR02Open) (Sonic Potions 0.37 port); study its oscillator, waveform interpolation and modulation.
- **Current implementation branch:** `agents/jb-28-full-integration-v186`; **experimental native wave branch:** `agents/jb-27-native-wave-scan-v187` (not integrated or hardware verified).
- **Preserve:** original six voices; original 3 ms No Clip fade/retrigger continuity; reliable velocity response; current known-working v1.8.06 image and rollback; **v1.8.06 bouncing skull screensaver** (user reports it is very good), including its animation, wake behaviour and display interaction. Do not redesign it; if changing firmware foundation, port and re-test it.
- **Licence gate:** upstream non-commercial redistribution conditions vs public JIMBONES GPL. No unreviewed upstream source copied into public branch. Decide between independently authored compatible port or separate appropriately licensed noncommercial fork before implementation.
- **Scope rule:** do not reintroduce experimental Metallic FM or sacrifice CPU stability for effects. All optional DSP should bypass cleanly.

## Project tree / dependencies

| ID | Work package | State | Exit criterion / next action |
|---|---|---|---|
| JB-00 | Baseline and rollback | REQUIRED / OPEN | Pin exact v1.8.06 image SHA, source commit, kit/SD backup and reproducible hardware symptoms; retain known-good original firmware |
| JB-01 | No Clip 3 ms fade preservation | PRIORITY 1 / OPEN | Locate first working code, trace note-on/retrigger/overlap path, create audio regression and compare against v1.8.06 |
| JB-02 | Upstream oscillator audit | IN PROGRESS | Pin upstream commit; map waveform dispatch, two-wave block blend, FM/noise/sample transitions, oscillator state and CPU/memory exposure. See [audit plan](JB_WAVETABLE_UPSTREAM_PORT_PLAN.md) |
| JB-03 | Upstream modulation + velocity audit | OPEN | Map waveform-ID modulation destination, fractional LFO state, velocity routing and persistence; specify useful velocity destinations/curves |
| JB-04 | Licence/architecture decision | BLOCKING / OPEN | Confirm lawful reuse path and whether native Erica adaptation or private upstream-derived fork preserves essential native features, No Clip and kit compatibility |
| JB-05 | Host wavetable prototype | BLOCKED BY 01–04 | Implement native waveform scanning and velocity modulation in isolated branch, no unreviewed code copy; keep stock path bit-identical when OFF |
| JB-06 | Audio quality and CPU host validation | BLOCKED BY 05 | Compare stepped vs smooth sound, all waveform classes, 3–4 overlapping hits, retriggers, tail clicks, peak/rail, deterministic CPU/memory bounds |
| JB-07 | Step FX / performance essentials | QUEUED / SCOPE REVIEW | Confirm per-slot voice targeting and 4-knob controls; preserve functioning dub timing, Step 1 LED, original voice routing; avoid speculative extra features |
| JB-08 | Comb filter integration | PATCH PUSHED / PARTIALLY VERIFIED | PR #18 focused Comb test PASS in CI run 42511490948; full host regression stopped on stale version assertion, fixed commit 442d8be. Rerun GCC/Clang and review interactions; no ARM |
| JB-09 | Integrated feature freeze | BLOCKED BY 00–08 | User reviews exact included features; all focused and full host tests green; no known audible click/crunch regression; fix list and test sheet signed off |
| JB-10 | ARM build + memory + image validation | **ON HOLD — USER APPROVAL REQUIRED** | Only after JB-09 and explicit approval: cross-compile selected frozen SHA, verify ITCM >=512 B, flash/RAM, image header/CRC, manifest/SHA256 and rollback pack |
| JB-11 | Physical hardware acceptance | BLOCKED BY 10 | User tests LXR-02 with Alesis pads, KT-10, FD-8, overlapping toms; report DSP/PK/LATE/RAIL, wave sweep, velocity and No Clip; iterate only after observations |
| JB-12 | Candidate release / documentation | BLOCKED BY 11 | Release only accepted, checksum-pinned test image with honest version and hardware status; preserve earlier images |

## Existing jobs — mapping
- **JB-28A** Comb filter: maps to JB-08; current draft [PR #18](https://github.com/balfourlynn1-byte/lxr-desktop-app/pull/18).
- **JB-28B** integrated physical regression: deferred to JB-11 after feature freeze and ARM approval.
- **JB-29A** per-slot Step FX voice targeting: JB-07.
- **JB-29B** four-knob Step FX parameter UI: JB-07, after voice-target behavior.
- **JB-27** four-periodic-wave native interpolation: experimental donor/research, not a substitute for upstream parity.

## Immediate next three jobs (host-only)
1. **JB-01:** locate exact original No Clip 3 ms implementation, preserve tests and behaviour.
2. **JB-03 + JB-02:** inspect upstream `modNode_updateValue`, interpolation render and compare against Erica structures; document compatibility and licence decision.
3. **JB-05:** create isolated native wavetable/velocity prototype and audible host comparison, then full GCC/Clang gates.

**Explicit stop:** no ARM compilation, no .img packaging, no merge to main, no hardware-ready claim before the user approves the frozen candidate.

Historical planning tree: [JIMBONES_JOB_TREE_V184.md](https://github.com/balfourlynn1-byte/lxr-desktop-app/blob/agents/wavetable-research/docs/JIMBONES_JOB_TREE_V184.md).

## Autonomous execution board
The [atomic task checklist](JIMBONES_ATOMIC_TASKS.md) is the **canonical one-task-at-a-time execution queue**, with A01–E07, dependencies, evidence gates, per-task completion messages and explicit user-gated ARM hold. First actionable job: **A01 — locate the original 3 ms No Clip fade source and commit**. Future scheduled runs should complete one small task, push and report, not claim unverified progress.

### 2026-11-08 atomic B01 checkpoint
Upstream `brendanclarke/LXR02Open` `prime` HEAD pinned to `c896bfd0efe86a4037418bf9f46f1fcc39255958`; six file blob hashes and licence restrictions verified in `docs/JB_WAVETABLE_UPSTREAM_PORT_PLAN.md` (commit `a5c51dc`). Research-only; licence gate still blocking. Next priority A01 original No Clip source trace; then B04/B07.

### 2026-11-08 A01 No Clip source checkpoint (partial)
Found original `jimbones-noclip-baseline` implementation: `RetriggerSmoother.h` sets 132 samples (~2.99 ms at 44140 Hz), `RetriggerSmoother.c` corrects first sample then decays correction over 132 samples; `DrumVoice.c` hooks three main drums before envelope retrigger and after volume render. Exact source blob hashes and lines recorded in `docs/CODEX_HANDOVER_2026-11-10.md`. Earliest introducing commit remains unverified; use `git log --follow -- Core/Src/Soundengine/DSPAudio/RetriggerSmoother.c` on historical branch. A01 remains PARTIAL, not DONE. No test or build.

### 2026-11-08 A02 No Clip comparison (source-only DONE)
The 132-sample smoother C/header blobs and enabled config are identical across `jimbones-noclip-baseline`, JB-26 skull lineage and JB-28 integration. Three-drum trigger/processing call sites are unchanged. JB-26 adds `jb_trigger` and `jb_preFilterNoise`, which can change surrounding audio despite identical smoother code. Exact SHA/line evidence and caveats in Codex handover A02. A03 host regressions still required; physical v1.8.06 image identity not verified.

### 2026-11-08 B04 compatibility checkpoint
B04 source-only DONE: pinned Erica/upstream OscInfo and modulation header blobs, waveform ID collision (Erica PWM=5 versus upstream CRASH=5), and six main voice target scope. Full mapping `docs/JB_B04_OSCILLATOR_COMPATIBILITY_MAP.md`, commit `ed00f8c97fa952935115d6f14a38df94edd4bf27`. B05 velocity destination map next; no host/audio test executed, ARM held.

### 2026-11-09 B05 velocity research checkpoint
Source-only B05 DONE: map and proposed default-OFF wave-position/shape destinations in `docs/JB_B05_VELOCITY_DESTINATION_MAP.md`; drive deferred, existing velocity semantics preserved. Host test NOT RUN. Next B06/A03.


### 2026-11-09 B06a native compatibility checkpoint
Source-only partial progress: verified native oscillator ABI, 64-frame DMA, 44140 Hz, six velocity nodes, fixed JB kit layout and Metallic OFF. Evidence `docs/JB_B06_NATIVE_ABI_GATE.md`. B06 remains OPEN pending upstream sample/kit/MIDI/display comparisons; executable host tests NOT RUN. ARM hold remains absolute.

### 2026-11-09 B07 licence decision
B07 DONE (research/decision only): public JIMBONES remains native GPL-only; independently authored bounded waveform extension is the selected route, upstream noncommercial source must not be copied. Evidence `docs/JB_B07_LICENCE_ARCHITECTURE_DECISION.md`, commit `8f6ea7de38a51006c037494cbdcd80b91a929938`. B06 and A03 still open; no tests executed. ARM hold unchanged.


### 2026-11-09 A03 mixed-block boundary fixture (NOT RUN)
Added deterministic 64/0/64/4/16-sample segmentation to `tests/test_noclip_132.c` in commit `1ffba1a5bc47f9204e0b34d33e0a2747c605ae6c`. Asserts first sample equals previous output, sample 132 returns to unmodified input, zero-length call preserves state, remaining countdown and no correction after boundary. Existing `tests/run_host.sh` already compiles this file; no runner modification necessary. Local checkout unavailable (`Could not resolve host: github.com`); no GCC/Clang test run, CI PASS or hardware evidence. Next: run `CC=gcc sh tests/run_host.sh` and `CC=clang sh tests/run_host.sh` in GitHub-enabled checkout, then review sanitizer logs. ARM hold unchanged.


### A04 checkpoint — 2026-11-09
Source map completed in `docs/JB_A04_SKULL_PRESERVATION_MAP.md` (commit `098ff972`): 170ms skull frames, LCD drain/latching and wake repaint; existing host test entrypoint identified. Execution NOT RUN; no hardware image claim. Next A05 or A03 execution when checkout available.


### 2026-11-09 B06b native sample boundary
Pinned native oscillator dispatch and inactive sample playback in `docs/JB_B06B_SAMPLE_DISPATCH_GATE.md` (commit `3cbb1fb2`). Sample asset exists but playable sample path is not established; no upstream sample IDs may be assumed compatible. B06 remains PARTIAL; host tests NOT RUN; ARM held.

### 2026-11-09 B06c checkpoint
Native MIDI/clock, six velocity CCs, global/dedicated note routing and JIMBONES NRPN boundaries mapped from `MidiParser.c` blob `04f3d03f` in `docs/JB_B06C_MIDI_CLOCK_COMPATIBILITY.md` (commit `59084b2f`). Source-only, no host tests executed; B06 remains PARTIAL. Next: sample/preset serialization and upstream comparison. ARM hold unchanged.


### 2026-11-09 D01 verified historical comb CI
GitHub run [42512010361](https://github.com/balfourlynn1-byte/lxr-desktop-app/actions/runs/42512010361), SHA `05e1722`: focused Comb, full host GCC and Clang, standalone Step FX all SUCCESS (host job `128826896630`). Separate artifact job `128826975121` failed at `tests/jb23_candidate_manifest.py:38`: `ValueError: Unexpected version for JB-23 candidate: 1.8.07`; artifact skipped. Root cause is manifest version validation, not proven comb DSP failure. Exact evidence `docs/JB_D01_COMB_CI_EVIDENCE.md`, commit `6351b76c`; D01 checklist update `137efcce`. Historical CI evidence only; latest HEAD and hardware NOT VERIFIED; no ARM work performed. Next D02 Step 1 LED/Step FX source test or A03 host execution when checkout available.

L03 partial (2026-11-09): source audit identified two 120 BPM initial defaults in `sequencer.c:57` and `menu.c:767`, with `SET_BPM` runtime routing in `menu.c:3728`. Persistence owner and startup load order still unknown. Evidence `docs/JB_L03_TEMPO_SOURCE_AUDIT.md` commit `17d7143c`; NOT RUN; L03 OPEN; ARM HOLD.

**H03 2026-11-09 PARTIAL source audit:** upstream morph MIDI 0..127 maps to 0..254 via `value<<1`, with six LFO and velocity amount CC routes, original-value reset and per-block interpolation mapping. Pinned blob evidence and host-only test matrix: `docs/JB_H03_MORPH_VELOCITY_ROUTE.md`, commit `30dc4e77`. Native morph precedence and kit/global serialization still UNVERIFIED; tests NOT RUN; H03 OPEN. ARM HOLD.

**H03 2026-11-09 native MIDI contract check:** exact GitHub source blob `04f3d03ffcee0b20f6d58e26d7a91f34e4b89bca` inspected: native `CC_MOD_WHEEL` branch is **no-op**, unlike upstream `value<<1` morph; six velocity amount CC cases dispatch to `velocityModulators[]` using normalized value. Source assertions executed during inspection (both TRUE). Committed executable Python source guard `tests/test_h03_native_midi_contract.py` (`26ff895b`) and host runner wiring (`32fce496`). Python test itself/full GCC-Clang suite **NOT RUN**; no local checkout. This is an important upstream/native compatibility difference; no morph behavior change implemented. H03 remains PARTIAL, ARM HOLD.

**H03 native morph safety guard 2026-11-09:** Added executable source contract `tests/test_h03_native_morph_contract.py` (`356a0a21`) and wired into `tests/run_host.sh` (`04275031`). It checks alternate morph kit buffer load, morph destination bookkeeping, source-kit audio-routing exclusion and existing velocity nodes, based on `Core/Src/menu/menu.c` blob `6282d73b`. Test NOT RUN (no local checkout); no audio/preset round-trip proof. H03 PARTIAL; next inspect `preset_morph` implementation/serialization and execute GCC/Clang host tests. ARM HOLD.

**H03 2026-11-09 morph slot contract:** Inspected `Core/Src/menu/menu.c` blob `6282d73b`: morph save uses `preset_saveDrumsetRam(...,1)`; morph load uses `preset_loadDrumsetRam(...,1,0)`, updates `PAR_MORPH_KIT_DEST` and `CURRENT_MORPH`, and morph rendering excludes audio-routing parameters. Added executable source guard `tests/test_h03_morph_slot_contract.py` (`21b4be75`, anchor correction `999d4e0f`) and wired into host runner (`eb916590`). Equivalent source predicates checked TRUE during connector inspection; committed Python test/GCC/Clang NOT RUN. Actual serializer body, round-trip, corruption and hardware remain unverified. H03 PARTIAL; ARM HOLD.

**H03 2026-11-09 morph RAM/SD route guard:** `Core/Src/menu/menu.c` morph save case calls `preset_saveDrumsetRam(...,1)` and contains no direct SD save; morph load calls `preset_loadDrumsetRam(...,1,0)` and updates destination/current morph. Added executable source guard `tests/test_h03_morph_ram_route.py` (`76df5649`, corrected case selection `c7a2b00b`) and wired `tests/run_host.sh` (`f2d32455`). Important: **this does NOT establish power-cycle persistence or prove that the serializer cannot write indirectly**. Python test/GCC/Clang NOT RUN; H03 OPEN; next inspect actual preset implementation and add round-trip fixture. ARM HOLD.

**H03 2026-11-09 native kit morph-save isolation (candidate FIX, NOT RUN):** `Core/Src/Preset/presetManager.c` blob `1b91b342` unconditionally copied `jb_parameters` into `jb_kits[presetNr]` at entry to `preset_saveDrumsetRam`, even when `isMorph=1`. That could overwrite the saved native extension of the morph target. Commit `8f37e730` moves the copy into the non-morph branch, preserving normal kit saves and morph parameter interpolation. Added focused source guard `tests/test_h03_morph_kit_extension_guard.py` (`83ffbc64`, correction `a3ac1ce2`) and wired into `tests/run_host.sh` (`3c7473c8`). Post-change connector source inspection confirms exactly one extension memcpy in the non-morph branch; **Python test and full GCC/Clang suite NOT RUN**, no full checkout. Need behavioral two-slot RAM round-trip, SD serialization audit and CI before marking H03 DONE. Six voices, 3ms No Clip, skull untouched; ARM HOLD.

**H03 2026-11-09 SD extension contract (PARTIAL, NOT RUN):** inspected `Core/Src/Preset/presetManager.c` SD save/load paths: normal project loop calls `preset_saveDrumsetSD(i,0)` and load `preset_loadDrumsetSD(i,0)`; normal save packs `jb_kits[presetNr]` and normal load unpacks it. Morph SD save, if invoked, writes a packed *live* `jb_parameters` extension but morph SD load explicitly skips extension; asymmetry requires policy review, not proof of corruption in normal project flow. Added executable source guard `tests/test_h03_sd_extension_contract.py` (commits `76defea5`, `832038f8`), wired to host runner (`9bec28a9`). Checkout failed DNS resolution (`Could not resolve host: github.com`), so Python/GCC/Clang **NOT RUN**. Need actual two-slot RAM/SD round-trip, legacy record truncation and failure injection; H03 OPEN; ARM HOLD.

**A03 2026-11-09 additional executable edge fixture (NOT RUN):** `tests/test_noclip_132.c` now exercises both full-scale opposite-polarity retriggers (-32768 to +32767 and reverse), first-sample continuity, final ramp endpoint within one LSB, fade completion and post-fade exact output. Commit `fdbcd18620f46cda4d43586d3ca33d662384ab4f`. Existing `tests/run_host.sh` already invokes this test; no runner change needed. Native checkout via git failed DNS (`Could not resolve host: github.com`), so GCC/Clang execution NOT RUN and A03 remains OPEN. Source is host-only; no ARM build/image/merge. Next run GCC/Clang host suite on exact SHA and record output.

**A03 2026-11-09 host CI execution gate (NOT RUN / pending CI):** Standalone `tests/run_noclip_132.sh` (`3d94a75f`) compiles only `tests/test_noclip_132.c` + `RetriggerSmoother.c` with ASan/UBSan. Scoped GCC/Clang workflow `.github/workflows/jb-a03-noclip-host.yml` (`6ad6f947`) triggers on A03 file changes and supports manual dispatch. No ARM jobs or image packaging. GitHub combined status query at runner commit returned no statuses; do not infer PASS. A03 remains OPEN until both CI jobs are observed green with exact commit SHA and logs, then check full host suite and hardware-only behavior.

**A03 2026-11-09 zero-frame pending edge (NOT RUN):** Added `test_pending_survives_zero_frame()` to `tests/test_noclip_132.c` (commit `4131df513c1ae381d6f400ecd470222fe354ce2b`), asserting zero-frame processing preserves pending retrigger, remaining countdown and buffer, followed by exact first-sample continuity. Already included by standalone and full host runners; no ARM changes. Local GitHub checkout attempted again but DNS failed (`Could not resolve host: github.com`); no GCC/Clang execution or CI PASS verified. A03 OPEN; execute exact-HEAD isolated and full host suites, then inspect logs.


**2026-11-09 H03 CI guard repair (PENDING rerun):** PR #18 run [42663528291](https://github.com/balfourlynn1-byte/lxr-desktop-app/actions/runs/42663528291) checked out merge SHA `bebb4a6c`; Comb focused test passed, GCC host runner stopped at `tests/test_h03_native_morph_contract.py` because its regex required `{` immediately followed by `morph = 0`, but native `Core/Src/menu/menu.c` has a legitimate explanatory comment between them. Updated guard to permit comments without crossing braces in commit `3cd0f13845f4c04f366122cbacb061ddb1d2d928`. This is a source-guard correction, **not a DSP fix**. CI has **not been rerun**; H03 and A03 remain OPEN; Clang skipped, no hardware verification. ARM HOLD unchanged.
