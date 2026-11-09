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
