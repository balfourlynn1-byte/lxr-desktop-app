# JIMBONES source and repository register — Codex review

**Verified 2026-11-08 using connected GitHub metadata and selected repository files.** This is an inventory of **known referenced repositories and branches**, not a claim that every upstream file has been downloaded or audited. Prefer pinned donor commit for actual code transfers. External examples are research only; licence review precedes reuse.

## Canonical source
- [JIMBONES repo](https://github.com/balfourlynn1-byte/lxr-desktop-app), public, default `main`, source of all current development. **Do not use `main` as a substitute for the feature branch**; stacked branches contain unmerged changes.
- [Current Codex integration branch](https://github.com/balfourlynn1-byte/lxr-desktop-app/tree/agents/jb-28-full-integration-v186): JB-28A target and handover files.
- [Canonical historical job tree](https://github.com/balfourlynn1-byte/lxr-desktop-app/blob/agents/wavetable-research/docs/JIMBONES_JOB_TREE_V184.md): JB-29 approved four-knob UI; separate branch, NOT automatically synchronised.
- [Agent board](https://github.com/balfourlynn1-byte/lxr-desktop-app/blob/agents/wavetable-research/docs/JIMBONES_AGENT_BOARD.md): historical job status; some statuses may predate newer experimental branches.
- [Working branch queue](JIMBONES_WORK_QUEUE.md), [checkpoint](CODEX_CHECKPOINT_JB28A.md), [persistence protocol](CODEX_PERSISTENCE_PROTOCOL.md).

## Verified external GitHub repositories
| Repository | Verified default branch | Purpose | Code reuse policy |
|---|---|---|---|
| [brendanclarke/LXR02Open](https://github.com/brendanclarke/LXR02Open) | `prime` | Native oscillator waveform-ID LFO scanning, waveform interpolation, user-sample ideas; reference `Core/DSPAudio/Oscillator.c`, `knowledge_files/OSC_INTERP_AUDIT.md` cited by JB-27 | **Licence incompatibility risk**: non-commercial terms; do not copy into public GPL JIMBONES without explicit permission and full licence audit |
| [tunefish777/openelixir-02](https://github.com/tunefish777/openelixir-02) | `main` | Alternative LXR-02 firmware/release reference (historically v1.2) | Research only until exact licence and source changes reviewed |
| [complex-ping/digitone-2-modding-skill](https://github.com/complex-ping/digitone-2-modding-skill) | `main` | Digitone II feature and modding research | Inspiration only; no assumption of LXR hardware compatibility |
| [balfourlynn1-byte/lxr-desktop-app](https://github.com/balfourlynn1-byte/lxr-desktop-app) | `main` | JIMBONES firmware, tests, docs and CI | Actual target repository; retain GPL-3.0-only and NOTICE obligations |

## Other references requiring URL identification or validation
- **octabam** — Octatrack USB audio reference; exact repository owner/path not established in this review. Search only when JB-06 USB feasibility begins.
- **modwerk USB audio out tracks** — USB audio routing reference; exact owner/path not established. Not a code dependency for Comb or Step FX.
- **lxr-desktop-app emu branch** — browser emulation reference; precise branch/path must be confirmed before using it.
- **Elektron Digitone II, Syntakt, Analog Heat +FX, Octatrack, PolyBrute** — musical/product design references, **not** transplantable firmware sources.

## GitHub PR integration chain (all currently draft and unmerged as retrieved)
| PR | Head branch | Exact verified head SHA | Scope |
|---|---|---|---|
| [#8](https://github.com/balfourlynn1-byte/lxr-desktop-app/pull/8) | `agents/jb-19a-send-gate-v184` | `1483258c4eee3068120553db1003c3dc4867964e` | Step FX send gate |
| [#9](https://github.com/balfourlynn1-byte/lxr-desktop-app/pull/9) | `agents/jb-19b-parallel-fx-send-v184` | `cc8cd0ff20fafb52e05269b487246889ca0fa806` | Parallel dry+FX send |
| [#10](https://github.com/balfourlynn1-byte/lxr-desktop-app/pull/10) | `agents/jb-21-stepfx-toggle-dub-ui-v184` | `df28cc52eebd9bfdfbecf61264e87b711188b070` | 16-button toggle, Step 1 LED candidate |
| [#11](https://github.com/balfourlynn1-byte/lxr-desktop-app/pull/11) | `agents/jb-20-dub-delay-sync-v184` | `943302d4e03d41b5a24caca04fe8242633243fd4` | Tempo dub delay |
| [#12](https://github.com/balfourlynn1-byte/lxr-desktop-app/pull/12) | `agents/jb-22-integration-gate-v184` | `636ceab0795ee1c3dded3970ebec27fe54fe02e1` | v1.8.04, Metallic FM disabled |
| [#13](https://github.com/balfourlynn1-byte/lxr-desktop-app/pull/13) | `agents/jb-24-wavetable-ui-v184` | `46a40b942c08a6ee9ed0e0130a15208bf06152fa` | JB-24 wave-first UI |
| [#14](https://github.com/balfourlynn1-byte/lxr-desktop-app/pull/14) | `agents/jb-23-from-waveui-v184` | `3006145594c731ea842ca4301911ff116a99a045` | Test pack |
| [#15](https://github.com/balfourlynn1-byte/lxr-desktop-app/pull/15) | `agents/jb-12-comb-filter-v184` | `cb27f684a108a3fe257ac4e4dfe437a041fbe40b` | Comb **donor**, forked from #14; do not merge wholesale |
| [#16](https://github.com/balfourlynn1-byte/lxr-desktop-app/pull/16) | `agents/jb-25-dub-timing-pads-v184` | `2dc643723db4f60ea290aea3188d5ae481560894` | Five dub timing pads, forked from #14 |
| [#17](https://github.com/balfourlynn1-byte/lxr-desktop-app/pull/17) | `agents/jb-26-skull-screensaver-v185` | `21d7c7e74a89909c122d3edb8cc37030f4c8473a` | Skull + LCD, forked from #16 |

**Crucial topology:** #15 Comb and #16→#17 skull are divergent descendants of #14. JB-28A must transplant Comb changes onto the #17 lineage. Do **not** merge #15 wholesale and overwrite newer code.

## Additional experimental branches discovered
- [JB-27 native wave scan](https://github.com/balfourlynn1-byte/lxr-desktop-app/tree/agents/jb-27-native-wave-scan-v187) and [its design](https://github.com/balfourlynn1-byte/lxr-desktop-app/blob/agents/jb-27-native-wave-scan-v187/docs/agents/jb27_native_wave_scan.md): **actual independently authored experimental native periodic waveform interpolation** exists on this separate branch. Four periodic waves only; does not port upstream sample engine. Not hardware accepted; do not confuse with JB-24 or claim unimplemented merely because absent from current branch.
- [JB-28 native wave UI cleanup](https://github.com/balfourlynn1-byte/lxr-desktop-app/tree/agents/jb-28-native-wave-ui-cleanup-v188): branch exists, but its guessed design file path could not be verified. Inspect branch diff before assigning status.
- Many specialist/research branches exist: `agents/cpu-research`, `agents/filters-research`, `agents/morph-research`, `agents/stepfx-research`, `agents/tape-research`, `agents/usb-research`, `agents/hihat-research`, `agents/random-research`, `agents/wavetable-research`, plus JB-01…30 specialist/submenu branches. **Branch existence does not mean implementation or testing.** Only inspect a branch for its assigned job.

## Primary code map (verified from target Core/CMakeLists.txt)
- Audio engine: `Core/Src/Soundengine/Soundengine.c`, `StepFX.c`, `StepFXRuntime.c`, `StepFxSend.c`, `StepFxSendMix.c`, `DubDelaySync.c`, `FxBus.c`, `AudioDiagnostics.c`.
- DSP: `Core/Src/Soundengine/DSPAudio/ResonantFilter.c`, `Oscillator.c`, `DrumVoice.c`, `Jimbones.c`, `wavetable.c`, `FX/Delay.c`; headers in `Core/Inc/Soundengine/DSPAudio/`.
- UI: `Core/Src/menu/JimbonesPerfUI.c`, `JimbonesWaveUI.c`, `screensaver.c`, `JimbonesSkull.c`, `menu.c`; `Core/Src/encoder.c`, `buttonHandler.c`, `ledHandler.c`, `Core/Inc/menu/MenuText.h`.
- Storage: `Core/Src/Preset/JimbonesPreset.c`, `presetManager.c`, `initKit.c`.
- Host test driver: `tests/run_host.sh`; target CMake: `Core/CMakeLists.txt`. New Comb focused tests live on donor #15.
- UI physical controls in source: `Core/Src/IO/adcPots.c` and `Core/Src/encoder.c`. **Check physical control semantics** before implementing JB-29 knob pickup or relative changes.

## Confirmed upstream design docs
- [Comb engineering and test plan](https://github.com/balfourlynn1-byte/lxr-desktop-app/blob/agents/jb-12-comb-filter-v184/docs/agents/jb12_comb_filter.md).
- [LCD/skull engineering and test plan](https://github.com/balfourlynn1-byte/lxr-desktop-app/blob/agents/jb-26-skull-screensaver-v185/docs/agents/jb26_skull_and_lcd.md).
- [JB-27 native wave implementation and limitations](https://github.com/balfourlynn1-byte/lxr-desktop-app/blob/agents/jb-27-native-wave-scan-v187/docs/agents/jb27_native_wave_scan.md).

## Source retrieval rule
Do not copy whole third-party repositories into JIMBONES. For each job, fetch the pinned donor commit's exact changed files and relevant tests. Record source revision, licence, attribution, test evidence and whether code was copied, adapted or independently authored. No unreviewed non-commercial source in the public GPL firmware.
