# JIMBONES external DSP research — 2026-11-09

Research only; no third-party source copied or ported. Preserve strict ARM hold.

## 1. Native desktop PCM harness — highest priority
Mutable Instruments Braids documents a native desktop oscillator test that generates `oscillator.wav` without flashing hardware:
https://pichenettes.github.io/mutable-instruments-documentation/modules/braids/open_source/

**Apply the technique, not the Braids implementation:** compile the real LXR-02 native DSP sources with a host harness and controlled dependencies; render deterministic WAV/PCM for factory OFF, wavetable ON, velocity/LFO scan, repeated retriggers and 3–4 overlapping voices. Compare peak, full-scale counts, continuity/clicks, and sample identity where OFF regression requires it. C09/C10 only test a small native kernel, not the complete voice engine.

## 2. Waveform quality and cost
Mutable Instruments Braids v1.8 describes band-limited waveform improvements, smoother wavetable scanning and interpolation/phase alignment:
https://pichenettes.github.io/mutable-instruments-documentation/modules/braids/firmware/
DaisySP offers MIT-licensed reference oscillators (including polyBLEP waveforms), DC blockers and effects:
https://github.com/electro-smith/DaisySP
https://github.com/daisyaudio/DaisySP/blob/master/Source/Synthesis/oscillator.h

**Experiment:** quantify aliasing, scan-position discontinuity and DC offset in the host renderer before adding more synthesis. Compare current linear interpolation against any candidate at matched levels and cost. Avoid importing C++ DaisySP wholesale into C firmware; port only a proven small algorithm after license and benchmark review. Preserve all original voice behavior when wavetable is OFF.

## 3. Cycle-accurate profiling
ARM Cortex-M7 DWT CYCCNT hardware documentation:
https://documentation-service.arm.com/static/5e906b038259fe2368e2a7bb
ARM profiling discussion:
https://community.arm.com/forums/f/armds-forum/55401/profiling-in-arm-development-studio---with-fvp/182745

**Plan:** add optional lightweight per-block cycle instrumentation (compile-time guarded, OFF by default) after approval for hardware testing. Record average/max cycles for 1/2/3/4 simultaneous voices, wavetable ON/OFF, step FX and long retriggers. Host runtime measurements are comparative only; not hardware cycle-accurate. Do not build or flash ARM without explicit approval.

## Immediate atomic sequence
1. Identify minimum native DSP dependency closure and existing emulator/host harness for real `calcDrumVoiceSyncBlock` and `jb_calcNativeWaveScanBlock`.
2. Build a host-native end-to-end PCM regression that exercises the actual DrumVoice/oscillator path. Assert OFF unchanged and ON materially audible with finite bounded output.
3. Add test cases for scan transitions, aliasing, DC, velocity and LFO at multiple pitches.
4. Draft optional hardware cycle-counter plan; do not implement hardware build yet.
5. Maintain mandatory 10–15 minute atomic commit/push and live checkpoint protocol in `docs/CODEX_CHECKPOINT_AND_RECOVERY.md`.

All references are research only. Confirm exact third-party file licenses before copying or adapting code.
