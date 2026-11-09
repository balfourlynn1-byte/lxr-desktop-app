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

## 2026-11-09 user-directed feature priority change — wavetable first
**Highest-priority incomplete core feature: JB-05 / C01–C11 / F03 native wavetable scanning with LFO and velocity modulation.** Research B01–B05/B07 is substantially mapped, but no complete integrated, host-verified audible implementation exists. Prioritize the next *unblocked* smallest wavetable task (finish B06 source/serialization gate, then C01 isolated experiment and C02 exact legacy bypass fixture, followed by bounded interpolation and velocity). Keep one atomic task per run; do not skip prerequisite source/licence checks. A03 132-sample No Clip regression, six original voices, approved v1.8.06 skull, preset safety and full GCC/Clang remain mandatory parallel **release blockers**, not deprioritized safeguards. Comb has historical green host CI, Step FX remains incomplete. No Metallic FM, ARM build, image packaging or main merge. This is a priority decision, **not implementation or test evidence**.

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

### 2026-11-09 — Wavetable C02 audio milestone (isolated experiment branch)
- **Priority JB-05 / C02:** local standalone host fixture PASS: `python3 test_c02_wavetable_audio.py` in reconstructed fixture workspace; WAV 22070 frames, mono PCM16, 44140 Hz, SHA-256 `20396bc4f995544882b27bb49b0b68f001a3093b2822012818062bd93b41a94c`. Repository fixture: `tests/test_c02_wavetable_audio.py` commit `3823de5ff81a804eb69a5952e8dda32b63a7c1ec`.
- Dedicated GitHub Actions workflow `.github/workflows/jb-c02-wavetable-host.yml` committed at `614f2ead51c0ba554605e5039917417900e987a0`; **no remote Actions run observed**, so CI remains UNVERIFIED.
- **Important:** C02 original task requires a native oscillator bit-identical OFF/bypass comparison; the standalone four-wave WAV generator is NOT that comparison. C02 remains PARTIAL. No native integration, browser emulation, velocity/LFO test or CPU evidence yet.
- **Next atomic task:** build a native oscillator legacy OFF/bypass audio regression with exact native engine linkage and host GCC/Clang, then integrate bounded wave scan on this isolated branch. Preserve original six voices, No Clip, skull, storage, Metallic OFF, and ARM hold. Emulator is a later validation gate, not a substitute for host test.

### 2026-11-09 — C03 native wavetable kernel, isolated host PASS
- Independently authored `Core/Inc/Soundengine/DSPAudio/JimbonesNativeWaveScan.h` committed `4d1023994207c3237079b5d1a619437845eff61e`; test `tests/test_c03_native_wave_scan.c` committed `e21c5d99816f802ed8c5f6d6f01883fb144df9db` and NAN fix `c2ca319ccece723efed393c96034c614af81175b`; runner `tests/run_c03_native_wave_scan.sh` committed `b6dbe664bf98f6de66db7602ac7306af5cb9e5a0`.
- Local focused GCC and Clang C11 `-O2 -Wall -Wextra -Werror -fsanitize=undefined,float-cast-overflow` checks PASS for scan position, interpolation, clamping and NaN. Executed an equivalent locally reconstructed test source/header; exact GitHub runner not independently run. No native oscillator dispatch connection or legacy OFF comparison yet; no emulator test. Next task is guarded integration with OFF bit-identity tests, preserving all original voices/No Clip/skull. No ARM build.

### 2026-11-09 — C04 velocity-to-wave-scan DSP connection
- Added opt-in `jb_wave_velocity_position` and `jb_wave_scan_velocity_sample` to `Core/Inc/Soundengine/DSPAudio/JimbonesNativeWaveScan.h`, commit `924bf73facc7028df7fd9149bda5e4dfea696ae5`; zero velocity depth leaves base scan position unchanged, positive/negative depth and clamping supported. No OscInfo ABI, waveform IDs, native dispatch or kit serialization changed.
- `tests/test_c04_wave_velocity.c` commit `f6ebf6f2a3c0558bc7e9d633e8cf4d2aeee1c874`; local GCC and Clang sanitizer host tests PASS using reconstructed equivalent C source: velocities 20,50,80,110,127 monotonic, zero-depth sample bypass, bounded extremes. CI workflow `.github/workflows/jb-c04-wave-velocity.yml` committed `dea5fbff6f3a5565f0ef7044705e3d5057fddcaa`; remote CI not yet verified.
- **C04 PARTIAL, not hardware-connected:** captured MIDI note-on velocity has NOT been threaded from parser/voice trigger to the scanner, scanner is NOT invoked by native oscillator dispatch, and actual audio/CPU/browser/hardware tests remain OPEN. Next task: trace native MIDI velocity through voice trigger and add opt-in voice-local routing, with OFF-path bit-identical regression first. No ARM build.

### 2026-11-09 — C05 native oscillator scan entry point (SOURCE PATCHED, ENGINE UNTESTED)
- Added opt-in `jb_calcNativeWaveScanBlock` declaration to `Core/Inc/Soundengine/DSPAudio/Oscillator.h` commit `8d8a1a2e3220babd444d2dda90d00bdf1b37dcbc`, and implementation to `Core/Src/Soundengine/DSPAudio/Oscillator.c` commit `8bbad59c62ec776a508f3207b3d44cf372fca01d`. Function uses existing `OscInfo.phase/phaseInc`, four-frame scanner, note-on velocity and signed LFO block modulation; factory `calcNextOscSampleBlock` dispatch deliberately unchanged. No new waveform IDs, struct fields or saved-kit bytes.
- Source-only guard added at `tests/test_c05_native_scan_source.py` commit `6d9fe69befa0831befadede4e3c46da9c3839bf9`; **not yet executed**, and native `Oscillator.c` has not been compiled/linked or audio-tested. **C05 PARTIAL.** Next: execute source guard, native GCC/Clang build, add call-site/voice velocity capture and LFO routing behind OFF-by-default controls, then bit-identical factory bypass and overlap CPU/audio regression. No ARM build, firmware image, main merge or browser success claim.

### 2026-11-09 — native trigger and render call-site audit (implementation unblock)
- `Core/Src/Soundengine/MIDI/MidiVoiceControl.c`: MIDI velocity `vel` already passed to `Drum_trigger(voice, vel, note)`, `Snare_trigger`, `Cymbal_trigger`, and `HiHat_trigger`.
- `Core/Src/Soundengine/DSPAudio/DrumVoice.c`: `Drum_trigger` lines 125–140 receives raw `vol` 0..127, calls `lfo_retrigger(voiceNr)`, then routes normalized velocity to `modNode_updateValue(&velocityModulators[voiceNr],vol/127.f)`. Capture raw note-on velocity per voice here without modifying retrigger smoother ordering.
- `calcDrumVoiceSyncBlock` lines 252–275 calls factory `calcNextOscSampleBlock` for the mod oscillator, then main oscillator in mix mode; otherwise `calcNextOscSampleFmBlock` for FM. Guard new scan at this main-oscillator render decision, retaining mod oscillator, FM and original dispatch when OFF. Need dedicated per-voice OFF-by-default settings, 4x256 waveform data, note-on capture, actual LFO block signal and persistence strategy before enabling.
- **No new render call-site code in this audit.** Next atomic task: implement minimal guarded voice-local main-oscillator call with OFF-path sample-identity test and verify with GCC/Clang; do not bypass FM/No Clip or change stored kits. ARM hold unchanged.

### 2026-11-09 — C06 guarded main-drum wavetable call site (CODED, NOT VALIDATED)
- `Core/Src/Soundengine/DSPAudio/DrumVoice.c` commit `465c0eb61080fa3f7d11fe36eae81a014f7e0902`: added volatile-only per-drum opt-in scan configuration, captures raw note-on `vol` before original retrigger handling, and routes main oscillator in `mixOscs` mode to `jb_calcNativeWaveScanBlock` only when explicitly enabled and supplied four persistent tables. Disabled path calls original `calcNextOscSampleBlock` unchanged; FM-only path untouched. `Core/Inc/Soundengine/DSPAudio/DrumVoice.h` API declaration commit `aec77b8ce4045afb1e06f3d838e965cc67555cb0`.
- **C06 PARTIAL / UNVERIFIED**: no host compilation or audio regression, no source tables or caller that enables the path, no native LFO feed, no per-kit persistence or UI, no FM-only integration. Shared table pointer currently applies to all enabled drum voices and must be made voice-specific before supporting independent tables. Next atomic task: validate compile and OFF-path, add owned waveform tables and explicit test harness, then live LFO wiring. Keep ARM compile/image/merge HOLD.

### 2026-11-09 — C06 test execution request
- Confirmed GitHub Actions C04 velocity regression run `42682444356` **SUCCESS** (GCC and Clang matrix), https://github.com/balfourlynn1-byte/lxr-desktop-app/actions/runs/42682444356 . C02 fixture run `42680038099` also SUCCESS.
- Added `tests/test_c06_drum_scan_source.py` commit `3723bc5560db6aecb01756f8adc06e202066f8ce` and `.github/workflows/jb-c06-drum-scan-guards.yml` commit `36fc6aac62a74409f226f0432a504520a65e5757`. GitHub Actions runs API at inspection still listed only C02 and C04; **C05/C06 guards have NOT been observed running or passing**. Source guards do not substitute for native compilation or sound testing.
- C06 remains **PARTIAL/UNVALIDATED**; no ARM build, no hardware candidate. Next: get C05/C06 workflow executed, fix failures, then host-native compilation, waveform-table owner and live LFO call-site wiring, and OFF-path bit identity.

### 2026-11-09 — one-day Codex handover preparation
- Created `docs/CODEX_EXECUTION_BRIEF_2026-11-10.md` commit `a1df5a447788729777e5840ea02a7da75635bbc6`: prioritized executable handover, exact branch references, C02/C04 confirmed Actions, C05/C06 unverified status, native build/legacy OFF identity/wavetable ownership/LFO/UI/SD/CPU gates, candidate manifest, explicit ARM approval hold and first Codex instruction. No build or new validation claimed.

### 2026-11-09 — reference review: RN01MA LXRE_MX v1.1 (RESEARCH ONLY)
- Verified release: https://github.com/RN01MA/lxr-eurorack-app/releases/tag/v1.1 (published 2026-11-09); Eurorack-only, not compatible desktop .img. Includes GLT 17 microsound recipes, CLS/EXP kit generator, per-voice regeneration, automatic level balancing, click-free kit morphing, CPU display, DJ filter, FX-bus voice routing.
- Source tree tag v1.1: `Core/Src/Preset/Randomizer.c`, `RandomizerUi.c`, `Core/Src/Soundengine/DSPAudio/FX/DjFilter.c`, `Core/Src/menu/menu.c`, `Core/Src/Soundengine/Soundengine.c`. **No code ported or tested**. Prioritize source-level review of CPU monitoring and morph click mitigation as potential help for current blockers; GLT recipes and DJ filter are **future features** subordinate to Nov 10 handover. Check GPL and NOTICE attribution before any reuse. No ARM build or hardware claim.

### 2026-11-09 20:19 UTC — C05/C06 source guards verified by GitHub Actions
- Previously unobserved workflow `JIMBONES C05-C06 source guards` run **42687294056** is now **COMPLETED SUCCESS** at commit `36fc6aac62a74409f226f0432a504520a65e5757`: https://github.com/balfourlynn1-byte/lxr-desktop-app/actions/runs/42687294056 . Both C05 and C06 Python **source guards passed in CI**. This is source-pattern verification only; native C compilation, actual OFF-path audio identity, LFO feed, waveform-table lifetime, CPU and hardware remain NOT RUN / OPEN.
- Committed `.github/workflows/jb-c04-wave-velocity.yml` update `84bf478e8e9428318543f0496287299768f3fcaa` to additionally execute both source guards in the existing GCC/Clang host regression matrix. **No run of this new matrix revision observed yet**. Next task: real native host compilation and OFF-path reference audio fixture; do not claim C05/C06 engine PASS. ARM hold unchanged.

### 2026-11-09 — C07 factory OFF-path source comparison prepared (NOT YET PASS)
- Added `tests/test_c07_factory_off_source_compare.py` commit `be82fd6c2493e12afa3fe82a555de94f4607feb6` comparing original oscillator/FM/mix/transient call expressions with integration branch `agents/jb-28-full-integration-v186`, plus guarded factory fallback checks.
- Added CI `.github/workflows/jb-c07-factory-off-compare.yml` commit `3e53281a3e7091d13be2a71454423f1f048c7542`, fetching the baseline in CI. At last Actions API check C07 had **no observed run**; status NOT RUN, not PASS. This is SOURCE equivalence only; real native compile and PCM sample-identity comparison remain BLOCKED/NOT RUN. No ARM build/image/merge.

### 2026-11-09 — USER PRIORITY: wavetable ON must work for Codex hardware candidate
- **P0 / required, not future proposal:** complete an explicitly enabled, audible wavetable render path before asking user to approve a feature-frozen ARM hardware-test candidate. Current C05/C06 code is partial and has not demonstrated actual wavetable ON audio.
- Acceptance: real native host-compiled wavetable ON PCM fixture; scan position changes timbre; note-on velocities 20/50/80/110/127 cause expected audible scan variation; live LFO moves scan smoothly; enable independently on supported voices with durable wave tables; 3–4 overlapping hits remain within DSP margin without clicks, clipping or accumulating LATE/RAIL; kit settings save/reload or explicitly documented blocker.
- Retain **wavetable OFF factory sample-identity** as a mandatory regression guard, not the product goal. Do not claim all six voices support scanning until implemented and tested; preserve all six original voices regardless.
- Gate: Codex should not present a wavetable-enabled hardware candidate as complete unless ON-path audio tests pass. Preserve explicit user approval before ARM compile/.img/merge; never skip tests to meet deadline.

### 2026-11-09 — C08 voice-local wavetable bank routing
- Replaced shared C06 `jbScanTables` pointer with `jbScanTables[NUM_DRUM_VOICES]` in `DrumVoice.c` commit `dc129b7004cf140281d69750783518bd13feb096`; configure sets bank for target voice only, disable clears target pointer, render reads that voice's bank. This prevents enabling a second voice from silently redirecting the first voice to the second's waveform bank.
- Added source guard `tests/test_c08_voice_local_scan_bank_source.py` commit `24327894ee930025d921e76686903965e96b0550`. **NOT RUN**; no native compilation/audio/CPU or persistent owned wave tables yet. Borrowed pointers must still reference durable data. Next priority: execute guard, native compile, then ROM/owned wave bank and audible ON-path fixture. ARM hold unchanged.

### 2026-11-09 — C08 automated guard wired to CI
- Added `tests/test_c08_voice_local_scan_bank_source.py` to existing C04 GCC/Clang workflow and push path filters, commit `dc3fae1b9f59c4832d5be8fd51e32c32a8b1d329`. The check now executes on relevant changes, rather than sitting uninvoked. **CI run result not yet verified**; do not count source guard as audio or native compile. Next: inspect Actions outcome, then add actual durable waveform tables and native ON-path PCM test. No ARM compile or image.

### 2026-11-09 — C08 CI failure diagnosed and source guard repaired
- GitHub runs `42697630903` (C04 matrix) and `42697226224` (C05/C06) FAILED because old C06 source guard asserted the removed global `jbScanTables` pointer; C04 velocity and C05 guard steps PASSED, C08 was skipped. Updated C06 assertion to `jbScanTables[voiceNr]` commit `c3e3aea139921b27d088cd37b86c40d14fc95656`. **Fix not yet confirmed by rerun**. C07 source comparison run `42696281091` PASSED; still source-only, not PCM identity. Next: verify new CI and native ON audio. ARM hold remains.

### 2026-11-09 — C09 C-kernel wavetable ON PCM test
- Latest repaired C04 GCC/Clang + C05/C06/C08 source guard workflow PASSED (`42699202075`). Added `tests/test_c09_wave_on_pcm.c` commit `63473a50c22acd73722bebaa0041b2e028245cba`, using actual `jb_wave_scan_sample` and `jb_wave_velocity_position` C functions to render 1024-sample signatures for different wave positions, note-on velocities, and LFO offsets; checks distinct PCM and zero-depth velocity bypass. Wired into GCC/Clang CI commit `89fc5e2b4a59517ebfb5bab4446e47afd3b968fd`. **New C09 result not yet verified**. C09 is isolated C kernel, not full native oscillator/drum mixer, no CPU budget or real hardware. Next: confirm C09 pass, then integrate owned waveform banks and compile complete host render. ARM hold unchanged.

### 2026-11-09 — C10 durable ROM waveform bank and native voice API
- C09 GCC/Clang kernel PCM fixture **PASSED** GitHub run `42699351706` (not full DrumVoice rendering).
- Added `JimbonesNativeWaveBank.h/.c`: immutable four-wave 256-sample triangle/saw/square/pulse ROM bank; added `jb_drumScanConfigureBuiltin()` API in DrumVoice to explicitly opt individual voices into firmware-owned waveform data. Default remains OFF; no UI caller or kit persistence yet. Commits `d06c3b2c`, `8e67eaac`, `4311a45d`, `3066fc21`.
- Added `tests/test_c10_native_wave_bank.c` and GCC/Clang CI link/run in existing workflow commit `5d229c2c`. **C10 result pending**. Important: firmware build-system source registration for new .c and full native DrumVoice linking/rendering remain unverified; do not claim hardware buildable. Next: confirm C10, register ROM source in desktop firmware build, native complete-render harness, persistent kit controls. ARM hold unchanged.

### CRITICAL — Codex timeout-safe save protocol (2026-11-09)
- **MANDATORY:** Read and follow `docs/CODEX_CHECKPOINT_AND_RECOVERY.md` before and throughout every Codex session. Make small atomic commits and **push to GitHub immediately**, ideally every 10–15 minutes; local commits alone are not safe. Update and push `docs/CODEX_LIVE_CHECKPOINT.md` with remote SHA, precise PASS/FAIL/NOT RUN, unfinished files, blockers and next command. Stop new work with 10 minutes left and save/push everything recoverable. On restart verify remote SHA and resume without overwriting prior work. WIP commits permitted only when clearly marked unverified. Never claim work backed up unless remote push confirmed.
- C10 Actions run `42699661787` FAILED in both GCC/Clang at C10 ROM-bank test; test changed to compare waveforms at quarter-cycle phase commit `5d0e19f2fc7fdca3fceb4b174cb548c63b8fc5c8`. **Rerun not yet verified**. C09 passed run `42699351706`. Continue full native rendering after CI gate. ARM hold unchanged.

### 2026-11-09 — C11 desktop build registration
- Added `Src/Soundengine/DSPAudio/JimbonesNativeWaveBank.c` to the actual desktop `Core/CMakeLists.txt` app source list, commit `04e5089a2f9b8496228be2decc814384672b5356`. Added source guard `tests/test_c11_native_build_registration.py` and wired into GCC/Clang CI commit `3ee234c3c0d4e8a331ed14bceabad0733d1bbdca`.
- **NOT full native compile yet:** C11 guard checks CMake source registration and API text; the complete DrumVoice/Oscillator compilation, linking, PCM and CPU remain unverified. Continue to prioritize actual native host harness, wavetable ON rendering, OFF identity. Strict ARM hold. Mandatory Codex checkpoint and push protocol remains in force (`docs/CODEX_CHECKPOINT_AND_RECOVERY.md`).

### 2026-11-09 21:24 UTC — C12 default-OFF source guard (one atomic task)
- Integration branch `agents/jb-28-full-integration-v186` observed HEAD `f797ec786fc57d008afeaabad964906f35a43177`; isolated branch before this task `a0a027a4517c09dc2ce9aced99349dd90eaeaf89`. No merge.
- C11 Actions run `42700501364` **PASS** at commit `3ee234c3c0d4` (GCC/Clang source guard; **not** complete DrumVoice host compile). C10 rerun `42699996137` **PASS**.
- C12 adds `tests/test_c12_wave_off_source.py` commit `a72afd221a7ecc7855d3916c5fef2e1377734f6c`, and CI step commit `9c26d45cbe3e3917c93adba4319504d4d9ed3c8c`. Command: `python3 tests/test_c12_wave_off_source.py`; checks static default OFF, explicit enable/disable, and legacy `calcNextOscSampleBlock` fallback. **NOT RUN / CI pending at time of checkpoint.** This is only a structural source guard, **not bit-identical PCM evidence**.
- Next unblocked: inspect C12 CI and fix if red; then complete native DrumVoice host compile and OFF-path PCM identity test. No ARM build, .img, main merge or hardware-ready claim. Mandatory checkpoint-and-push instructions apply; each GitHub contents write creates an immediately remote-backed commit.

**C12 CI verified PASS:** GitHub Actions run `42702335053`, commit `9c26d45cbe3e3917c93adba4319504d4d9ed3c8c`, both compiler matrix jobs completed successfully. Source guard only; native DrumVoice compile and OFF PCM bit identity remain NOT RUN. https://github.com/balfourlynn1-byte/lxr-desktop-app/actions/runs/42702335053

### 2026-11-09 — Approved UI roadmap (design only; implement AFTER native wavetable ON audio is validated)
User approved the proposed UI direction. **Do not mistake approval of this roadmap for approval to compile ARM firmware or ship a hardware image.** Maintain strict ARM hold and mandatory commit/push checkpoint policy in `docs/CODEX_CHECKPOINT_AND_RECOVERY.md`.

**Priority order and acceptance gates:**
1. **WT Macro (P1 after native ON audio):** OSC double-press opens a clearly labelled extra page with four controls: WAVE, SCAN, VEL, LFO. Preserve original OSC screen, navigation and all six original voice sounds; ensure no accidental parameter changes when switching pages.
2. **Per-voice wavetable toggle (P1):** explicit OFF/ON for each voice, with six-voice overview and quick kick exclusion. Never silently enable wavetable; ON uses durable built-in waveform data and OFF preserves exact factory path.
3. **Kit save/recall status (P1):** modified-kit indicator, save confirmation, error/recovery warnings; save/reload wavetable per-voice settings and kit BPM safely without breaking factory kit ABI or cold-start recovery. No false 'saved' indicator on SD failure.
4. **Performance FX (P2):** steps 1–16 independent toggle behavior; Step 1 all-FX bypass and Step 16 morph LFO; correct sequencer Step 1 LED when armed. Define unambiguous mode separation so FX mapping cannot interfere with sequencer editing.
5. **Diagnostics (P2):** DSP %, peak-hold, LATE and RAIL counters on an accessible page; keep CPU impact negligible and show resets clearly.
6. **WT detail and modulation pages (P2):** OSC additional presses reveal BANK/POS/BLEND/MODE and velocity/LFO rate/depth controls, subject to hardware display/encoder constraints and real engine support.
7. **Waveform mini-display (P3):** lightweight static/slowly refreshed waveform and scan-position marker; no full oscilloscope unless proven CPU-safe. Drop animation if DSP margin suffers.
8. **Hold-to-audition A/B (P2):** temporary original oscillator audition while holding an assigned control; release returns to configured wavetable without modifying saved state. Choose a non-conflicting physical control after hardware button-map review.

**Codex implementation requirements:** first inspect actual LXR-02 menu/button/encoder architecture and document feasible page mappings; prototype on host/emulator, validate UI state transitions and audio OFF/ON switching, check DSP headroom and persistence. Each atomic UI change must be committed and **pushed** with test evidence and live checkpoint. Do not implement UI ahead of native ON-path PCM acceptance.

### 2026-11-09 — Approved addition: PERFORMANCE / FX ALL page
- Add a second page within the Performance section labelled **FX ALL** (proposed UI; not yet implemented), for controlling effects across all eligible voices together.
- Provide an explicit **ALL FX ON/OFF** master bypass and quick access to global effect amounts/assignments; keep per-voice exclusions, especially optional kick protection, visible and respected.
- Distinguish the FX ALL page from the Step FX 1–16 toggle page and normal sequencer mode; preserve Step 1 sequencer LED and existing Step 1 bypass / Step 16 morph LFO proposals without ambiguous button mappings.
- Design safeguard: master bypass should not erase per-effect toggle states, so re-enabling restores prior configuration. No kit writes until explicit save, and no unintended effect on original sound when disabled.
- First inspect actual performance-menu hardware architecture and existing global vs per-voice effect routing. Specify what 'all' covers before implementation; no new DSP-heavy global effect chain by default.
- **Priority:** P2, after native wavetable-ON audio and P1 controls. Prototype and test on host/emulator, measure CPU, commit/push each atomic step and update live checkpoint. Strict ARM hold remains.

### 2026-11-09 — Approved design direction: universal hold-to-modify performance controls
User wants Torso Electronics S-4-style press-and-hold behavior and Digitakt TRACK-like held modifier **across all settings, not only FX**. This supersedes treating momentary FX ALL as an isolated feature.

**Design specification (proposal, not implemented):**
- Add a consistent **momentary / latch** interaction model across supported parameters and pages. A held physical modifier temporarily applies an alternate state; release restores the underlying state. Optional latch mode stays engaged until toggled again.
- Apply to FX ALL and individual FX bypass/engage, morph target, wavetable A/B original-engine audition and scan position, filters, randomization, and selected/all-voice parameter edits. Respect voice exclusions, especially kick.
- Support held modifier + encoder/parameter movement for selected/all-voice adjustment, **but do not commit persistent parameter changes unless explicitly confirmed**. Differentiate temporary audition from intentional editing.
- Capture original values at press-down; release restores deterministically even if the user changes page/voice while holding. Define conflict resolution for concurrent modifiers, retriggers, kit changes, transport, and rapid press/release; no stuck state after page exit or kit load.
- Keep ordinary short press and existing sequencer/Step FX buttons functional; avoid conflicts with existing hardware combinations. No specific button assignment until actual LXR-02 control mapping is inspected. Clear held/latch status indicator; avoid CPU-heavy display work.
- Acceptance: host/emulator UI state-machine tests for press/hold/release, latch toggle, simultaneous hits, page switching, exclusions, save/reload, and audio bypass identity; verify no unintended kit writes and no new DSP spikes.
- **Priority P2** after full native wavetable-ON PCM validation and P1 core controls. Atomically commit **and push** every coherent step; update `docs/CODEX_LIVE_CHECKPOINT.md` per mandatory timeout protocol. Strict ARM hold unchanged.

### 2026-11-09 — User-defined SHIFT + RELATIVE EDIT (six-voice proportional offset)
**Confirmed requirement:** Holding SHIFT while adjusting a compatible parameter applies the **same signed delta** to that parameter on every included voice, **not the same absolute value**. Example cutoff [20,45,60,35,75,90] +10 -> [30,55,70,45,85,100] (0–100 range). Preserve each voice's independent offsets/timbre; clamp each voice at its own valid bounds. This is a **persistent in-memory edit**, NOT momentary; release SHIFT does not revert. Kit file changes only after normal explicit save.

- Candidate parameters: pitch, decay, filter cutoff/resonance, drive, wavetable scan and morph, **only where each target voice actually supports equivalent semantics**. Avoid applying unrelated values or incompatible parameter types.
- Every physical encoder tick produces one delta, with no accumulation from redraws or repeat frames. Use correct native resolution/scaling per parameter (not naive raw byte increments for differently scaled controls). Preserve voice exclusion masks including optional kick protection. Clearly indicate REL EDIT / included voices on display.
- Handle boundaries, negative movement, voice changes while held, undo/cancel policy, kit load, parameter automation and MIDI overrides; verify no surprise jumps or kit writes. Do not conflate this SHIFT edit behavior with separate hold-to-audition momentary effects.
- Before implementation inspect existing SHIFT button and encoder mappings; do not break normal SHIFT shortcuts or Step FX/sequencer controls. Test distinct initial values, positive/negative delta, clamps, exclusions, and saved/reloaded state in host/emulator.
- **Priority P2** after native wavetable ON audio gate and core UI. Atomic commit/push and checkpoint after each task; strict ARM hold unchanged.

### 2026-11-09 — SHIFT relative-edit implementation research R00
- Completed first source trace and saved `docs/JB_SHIFT_RELATIVE_EDIT_CODE_RESEARCH.md` commit `85e738138b55be82b45753d4834ea1c92c0805bb`. Confirmed `buttonHandler_getShift()` in `Core/Src/buttonHandler.c`, encoder edit entry `menu_encoderChangeParameter(int8_t inc)` in `Core/Src/menu/menu.c`, shared `activeParameters` and MIDI_CC/CC_2 dispatch, and existing SHIFT+Performance shortcut. **Do not assume parameter IDs have a per-voice stride**. Next R01 enumerate semantic per-voice parameter mappings; R02 trace per-voice dispatch and persistence; R03 pure signed saturating delta tests; R04 gated UI; R05 host acceptance. Research only, no runtime changes, ARM hold. Follow mandatory continuous commit/push and live checkpoint protocol.

### 2026-11-09 — SHIFT relative edit R02b code trace
- Research updated in `docs/JB_SHIFT_RELATIVE_EDIT_CODE_RESEARCH.md` commit `7370f34e88b530bf505a160e8cc8a3b0cebee264`. Discovered `Core/Src/Soundengine/MIDI/ParameterArray.c` maps individual per-voice parameter IDs to engine fields via `parameterArray_init()`; voice types use different structures, so semantic ID groups required. `paramArray_setParameter()` active implementation forwards to `midiParser_ccHandler`, not a guaranteed safe direct write. Next enumerate compatible six-voice ID groups and automation/preset behavior; no runtime code change. Continuous save/push protocol and ARM hold unchanged.

### 2026-11-09 — External DSP research and implementation gates
- Saved `docs/JB_EXTERNAL_DSP_RESEARCH_2026-11-09.md` commit `3b7acce2fc39f3fe39333086166666ccd5af1cb2`. Researched Mutable Instruments Braids native desktop WAV harness and waveform quality improvements, DaisySP oscillator/polyBLEP reference, ARM Cortex-M7 DWT cycle-counter profiling. **Priority:** full real native DrumVoice PCM ON/OFF host harness; then scan/aliasing/DC quality comparisons; then optional CPU cycle profiling design. No external source imported, no native full-engine test run yet, no ARM compile. Respect license checks, strict ARM hold and mandatory atomic GitHub commit/push + live checkpoint recovery.

### 2026-11-09 — LXR ecosystem research
- Saved `docs/JB_LXR_ECOSYSTEM_RESEARCH_2026-11-09.md` commit `d88a8a436f038822cdc64e565a0dddbce1128ae3`. New leads: independent LXR-02 browser emulator for cross-validation, `lxrpy` kit/config parser for SD save/rollback fixture checks, Brendan Clarke's LXR02Open DSP/oscillator audit notes (research only; different licence), and ARM CMSIS-DSP fixed-point interpolation for benchmark candidates. Next E01 verify experimental-source emulator compatibility; E02 read-only kit round-trip fixtures; E03 review DSP audits; E04 compare interpolation only after real native PCM harness. No experiments run, no source imported, ARM hold unchanged. Follow continuous GitHub commit/push and checkpoint protocol.

### 2026-11-09 — First actual host experiment X01
- Ran C11 host test with `-Wall -Wextra -Werror -fsanitize=address,undefined`: **PASS** for six distinct initial voice values, positive/negative relative delta, voice exclusion and saturation. Checked-in source `tests/test_jb_shift_relative_host.c` commit `9d28bce4cfb251066feb14e74323a07e5ecaa273`. Recorded details/limits in `docs/JB_EXPERIMENTS_2026-11-09.md` commit `805232bf34b26f1683a41c5d9baf2f3e211552a8`. This is only a pure host algorithm experiment, NOT firmware integration, CI, emulation, DSP or hardware. New source lead: LXR QEONDA already advertises change-all knob edits; investigate exact implementation. lxrpy docs describe BPM as project-global, so per-kit BPM requires separate compatibility-safe design. Next: wire checked-in C test to host CI, inspect QEONDA code, disposable lxrpy round-trip fixtures. Strict ARM hold unchanged.

### 2026-11-09 — Experimental findings consolidated into job tree
**Completed (host-only):** X01 SHIFT + RELATIVE EDIT signed-delta algorithm test PASS for six independent voice values, positive/negative edits, exclusion mask, and saturation; checked-in `tests/test_jb_shift_relative_host.c`. This does **not** validate actual firmware routing, UI, persistence or audio. Details: `docs/JB_EXPERIMENTS_2026-11-09.md`.

**New actionable jobs, ordered:**
1. **X01a (P1):** Run the checked-in host C test in GitHub native CI with GCC/Clang and sanitizers; record actual CI results.
2. **X02 (P2):** Inspect LXR QEONDA's existing change-all knob feature, source and licence; compare relative vs absolute behavior, SHIFT handling, voice mappings and clamping. Research only until compatible.
3. **X03 (P1):** Build disposable `lxrpy` fixture tests for `.SND`, `.PAT`, `GLO.CFG` round-tripping and truncated/corrupt files; never touch the user's SD card. Confirm per-kit BPM needs custom persistence because stock format documents global BPM.
4. **X04 (P1):** Review FatFs async/save ordering and rollback failure windows against LXR02Open memory notes; verify actual local code rather than assuming identical architecture.
5. **R01/R02 (P2):** Complete semantic six-voice parameter ID mapping via `ParameterArray.c` and `MidiParser.c`, including automation-safe per-voice setters, before SHIFT firmware integration.
6. **Audio gate (P0):** Finish real native DrumVoice wavetable-ON/OFF PCM rendering and overlapping-hit/CPU tests before any UI feature promotion.

**Guardrails:** Experimental branch only, no ARM compilation or flashable image without explicit approval, no claims of hardware readiness, commit and push each atomic task and update live checkpoint.

### 2026-11-09 — X01a host CI integration
- Added `tests/test_jb_shift_relative_host.c` to push-path triggers and GCC/Clang native sanitizer test steps in `.github/workflows/jb-c04-wave-velocity.yml`, commit `42e79402d821d205a766c2d578822f7bc23d1892`. New workflow execution **not yet verified**; last observed previous successful run `42702335053` (head `9c26d45cbe`, before this commit). Next inspect CI run for commit `42e79402`, confirm both compiler jobs and record logs; if failed, fix and push atomic commit. Strict ARM hold remains, no hardware firmware change.

### 2026-11-09 22:26 UTC — X01a CI evidence gate closed (single atomic verification task)
- Verified GitHub Actions run **42705075831**, head commit **42e79402d821d205a766c2d578822f7bc23d1892**, `2026-11-09T21:42:46Z`, conclusion **SUCCESS**. Direct API job inspection confirmed both `velocity (gcc)` job **129459626510** and `velocity (clang)` job **129459626965** SUCCESS, with the `X01a SHIFT relative edit host regression` step SUCCESS in each. Also observed C05/C06 source guard steps, C09 isolated C-kernel PCM, C10 ROM bank and C12 OFF source guard SUCCESS in both jobs. URL: https://github.com/balfourlynn1-byte/lxr-desktop-app/actions/runs/42705075831 .
- Repro command from workflow: `gcc -std=c11 -O2 -Wall -Wextra -Werror -fsanitize=undefined -ICore/Inc tests/test_jb_shift_relative_host.c -o build/host/test_jb_shift_relative_host && build/host/test_jb_shift_relative_host` (replace `gcc` with `clang`). **Observed result is GitHub Actions step SUCCESS, not separately observed terminal stdout.**
- Scope: X01a **PASS (native isolated algorithm CI)**. Full native DrumVoice compilation, six-voice PCM OFF bit identity, real live LFO/MIDI integration, overlapping 3–4 hits and hardware testing remain **NOT RUN**. CI source guards do not prove audio identity. No ARM build, image, branch merge or hardware changes.
- Next exact atomic task: inspect `Core/Src/Soundengine/DSPAudio/DrumVoice.c`, `Oscillator.c` and build dependencies; add a host-only compile smoke test of the *actual native* DrumVoice compilation unit without ARM firmware targets, record unresolved dependencies and stop on errors. If impossible, document precise blocker. Maintain strict ARM hold and remote checkpoint protocol.

### 2026-11-09 23:25 UTC — Atomic native DrumVoice compilation probe
- Inspected actual `DrumVoice.c`, `DrumVoice.h`, `Oscillator.c`, `Oscillator.h` on isolated branch and compared `DrumVoice.c` with integration branch `agents/jb-28-full-integration-v186` (different blob SHA: isolated `be67780122fbc946439a4211a96cebef9fdbc38f`, integration `1f1d110c36eeb734f3694500a859faa19df4c300`). **No merge.**
- Created host-only real translation-unit syntax compile probe `.github/workflows/jb-native-drumvoice-probe.yml` commit **`6d3ae26f86b868c88bc6d9794a6431e5ed272f47`**. Exact command: `gcc -std=gnu11 -fsyntax-only -Wall -Wextra -ICore/Inc -ICore/Src -IDrivers/STM32F7xx_HAL_Driver/Inc -IDrivers/CMSIS/Device/ST/STM32F7xx/Include -IDrivers/CMSIS/Include Core/Src/Soundengine/DSPAudio/DrumVoice.c`. Runs on ubuntu-24.04 native GCC, **not ARM**, no linking or PCM output.
- Actual GitHub run **42720355331** https://github.com/balfourlynn1-byte/lxr-desktop-app/actions/runs/42720355331 . At checkpoint job was **IN PROGRESS**, compilation result **NOT YET VERIFIED**. Stop on errors; do not call it PASS until job outcome inspected. Next exact action: inspect run 42720355331 jobs/logs; if FAIL, record first missing dependency and design a host-only shim or isolated dependency fix without altering ARM firmware path. If PASS, proceed to native link/PCM harness in a separate atomic task.
- Full engine audio, factory OFF bit identity, overlapping hits, CPU, SD and hardware all **NOT RUN**. Strict ARM hold, six voices, No Clip, kits and skull preserved; Metallic FM shelved. Continue atomic remote checkpoint and push protocol.

### 2026-11-09 23:26 UTC — Native compile probe result: FAIL (stop on defect)
- GitHub Actions run **42720355331**, job **129512433903**, completed **FAILURE** for actual host GCC syntax compilation of `DrumVoice.c`. https://github.com/balfourlynn1-byte/lxr-desktop-app/actions/runs/42720355331 . Source/workflow commit `6d3ae26f86b868c88bc6d9794a6431e5ed272f47`.
- First diagnostic: `Drivers/CMSIS/Device/ST/STM32F7xx/Include/stm32f7xx.h:145:3: error: #error "Please select first the target STM32F7xx device used in your application"`; cascading HAL header unknown `uint32_t` and implicit function warnings. This probe used native GCC without a target device macro; **failure is a host-configuration dependency**, not yet evidence of a DSP source defect. Do not count as a native compile PASS.
- Exact failed command is recorded in preceding checkpoint. **Next atomic task:** build host-only compile configuration/stub or determine correct STM32F7 target macro and isolate ARM-specific intrinsics; rerun and inspect first remaining failure. Avoid executing ARM compiler or enabling ARM CI. Full native link/PCM OFF identity NOT RUN. No hardware build/merge.
