# JB-24 — wave-first oscillator UI, Metallic FM hidden

**2026-11-08 — JIMBONES v1.8.04 experimental.** Branch `agents/jb-24-wavetable-ui-v184`, stacked on JB-22. No automatic merge or hardware approval.

## User request and result

- User explicitly wants Metallic FM removed from the OSC interface and wants wavetable controls similar to the referenced GitHub LXR-02 firmware.
- The Metallic FM render path was **already disabled/compiled out in JB-22**; JB-24 now **hides all six legacy FM-specific parameters from OSC navigation**: `JB_METAL_LEVEL`, `JB_METAL`, `JB_VMETAL`, `JB_RATIO`, `JB_DEPTH`, `JB_FEEDBACK`. Never expose the old `Original + Metal` mode labels.
- `JB_MODE` is presented as **Wave ON/OFF**; UI writes **0=OFF** and **2=WAVE** only. Previously saved modes 1/3 continue to load without data loss; they display OFF/ON respectively and the DSP remains FM-free. The **32-field kit format and all original parameter IDs remain unchanged**.
- OSC page order is now **wavetable-first** (same functional *pattern* as the upstream approach, not a source-code copy):
  1. `Wav` wave layer ON/OFF, `Lvl` level, `Pos` waveform position and `Mov` motion
  2. `Shp` waveshape, `Chr` character, `Drv` drive, `Bit` reduction
  3. pitch/detune/pitch envelope/randomization
  4. decay/transient/velocity/blend
  5. texture/aggression and velocity modulation
  6. noise/tone and velocity response
  7. extra expression / Step FX amount (only two populated columns)
  8. dedicated interpolation page, ON=smooth blend, OFF=stepped waveform
- Editing waveform position shows names `SINE`, `TRI`, `SAW`, `PULSE`, and transitional names `SINE>TRI`, `TRI>SAW`, `SAW>PULSE` when interpolation is on.
- Retain six original LXR voices, the existing wavetable layer, low-cost phase warp and wave folding, existing shared FX bus, and existing Step FX sends/delays.

## Relationship to upstream

Reference: [Brendan Clarke / LXR02Open](https://github.com/brendanclarke/LXR02Open), branch `prime`. Files investigated include `Core/Menu/menu.c`, `Core/Menu/menuPages.h`, `Core/DSPAudio/Oscillator.c`, `knowledge_files/OSC_INTERP_AUDIT.md`, and `LICENSE`.

The reference exposes `WAVEFORM` on an OSC page, includes global oscillator interpolation and sample-file names when samples are present, and can blend adjacent oscillator waveform IDs during LFO modulation. Its source licence permits **non-commercial use** (the user's stated purpose) but has redistribution terms that are **not automatically compatible with this repo's GPL-3.0-only licensing**. Because the repository is public, this implementation uses only *independently authored UI helpers* and existing GPL-licensed JIMBONES DSP code rather than copying upstream source. **It is not a verbatim port of the upstream oscillator engine or sample-loader UI.**

The native LXR's existing sine/triangle/saw/pulse bandlimited tables and the JIMBONES wavetable interpolation are used. New user-sample wavetable loading and full native-oscillator LFO interpolation across sample IDs are **not implemented** in this PR; each would require a separately licensed and measured DSP design, particularly given the tight 16KiB ITCM and prior 101% load.

## Validation checklist

- `sh tests/run_jb24_waveui.sh`: 26 unique visible parameters, none of the six obsolete FM fields, shape labels, page transitions, encoder ordering.
- `sh tests/run_host.sh`: existing six-voice and saved-kit backward compatibility, Step FX send/dub delay, interpolation sound comparisons.
- `python3 tests/test_menu.py`: actual OSC menu transition code, seven banks + interpolation, exits after last page, preservation of other menu pages.
- `.github/workflows/jimbones-jb24-waveui.yml`: both GCC and Clang host suites + ARM cross-compile, image header and SHA256, >=512 bytes free ITCM. A successful run is **not a device proof**.
- Hardware remains needed: ensure exact visual names fit on LCD, all six voice selections, stored old FM kits silent extra-layer but normal native voice unchanged, waveform knob selection, smooth/stepped interpolation audible at moderate output, no crunch on 3–4 overlapping pads, Step 1 LED and dub echo.

## Next stages

1. Confirm hardware CPU and UI behaviour against v1.8.03 using PK / DSP / LATE / RAIL.
2. If user wants the upstream sample/loop loading capability specifically, prepare a **separate** storage/flash/memory/format review; do not graft in sample loader unchanged.
3. If user wants interpolation on original oscillators while an LFO modulates waveform number, scope a separate feature with CPU cost testing. Do not add this implicitly to JB-24.
