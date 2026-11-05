# JIMBONES v1.7.91 — experimental hardware-test build

Custom firmware for the LXR-02 desktop. This is a Phase 1 implementation, not an official Erica Synths release. It has not been tested on physical hardware. Do not infer a live-performance guarantee from the successful build or desktop tests.

## Provenance and fallback

Source starts from upstream commit `a43cd4fcda23ec3d577dc425261c3eb28bd5abff` (v1.7) in `balfourlynn1-byte/lxr-desktop-app`, branch `jimbones-v1.7.91`. The supplied v1.7.90 binary is preserved separately, byte-identically, with SHA-256:

`57ce6e6a6ad729d4e7a4c1d657fce10e39c0c94f94a24abc078442afe74b266c`

Its source modifications are unknown and have not been carried forward or verified. This firmware therefore preserves the public upstream triggering implementation, not an assumed v1.7.90 MIDI patch.

## What is implemented

- Six independent sets of additional-synthesis settings; the original voices always render.
- Four layer modes: 0 Original; 1 Original + Metallic FM; 2 Original + Wavetable; 3 Original + Both.
- Both new engines disabled in defaults and legacy kits, with separate level controls.
- Metallic oscillator pair with inharmonic ratio/depth control, bounded feedback, coloured noise, a transient envelope and a pitch envelope.
- Wavetable interpolation/scanning across the existing sine, triangle, saw and square tables. This is a compact four-wave bank, not an imported collection of arbitrary wavetables.
- Four macros and four signed velocity amounts per voice; 28 accessible parameters per voice.
- Retriggers use the existing drum/snare/cymbal/hat trigger functions. Open and closed hat share a single layer state. A closed hit replaces the open tail; hat layers also follow the original hat amplitude envelope.
- Mix into each voice before existing decimation, pan, output routing and FX routing. Layer levels are additionally subject to that voice's mixer volume and fader.
- Versioned, checksummed settings appended to kit and temporary-autosave files. RAM kit switching, project kit saves, init/clear and temporary-kit restore include the new settings.

The upstream physical engines are three drum voices, snare, cymbal and one hi-hat with two articulation tracks. Patch names such as kick/tom/clap do not create extra engines. No voice or MIDI assignment is remapped. Both hat selection buttons edit the same additional-synthesis settings.

## Quick start

1. Select a voice and press **OSC once** for its original oscillator page.
2. Press **OSC again** for `Met Noi Agg Tex`.
3. To enable a layer, rotate the encoder four positions backwards from `Met` to `Mod`, click to edit, and set 1, 2 or 3. Mode 0 is the original sound only.
4. Click the encoder to leave value editing. Rotate to select parameters; the display follows the selected group of four. The four knobs edit the current group's values.
5. Pressing OSC repeatedly cycles macros → velocity amounts → FM → noise/transient → drive → wavetable → layer mode/levels/decay → original oscillator page.
6. Any other synthesis-page button returns to its existing function. Selecting another voice exits the extension; press OSC twice to enter it again for that voice.
7. Save kits/projects using the normal controls. Temporary autosave is supported, but explicitly save a project before relying on the SD card backup.

The four-parameter display uses short labels. Click the encoder for the full name and, for Mode, a readable engine combination. No long-press/double-click timing is required; unshifted presses use the existing press-event handler.

| Group | Display labels | Meaning |
|---|---|---|
| Layers | Mod FML WTL Dec | Mode, metallic level, wavetable level, amplitude decay |
| Macros | Met Noi Agg Tex | Metal, Noise, Aggression, Texture |
| Velocity | vMt vNo vAg vTx | Independent signed velocity amount for each macro |
| FM | Pit Rat Dep Fbk | Carrier pitch, FM ratio, FM depth, FM feedback |
| Noise | Det Nvl Col Trn | Detune, additional noise level, colour, transient length |
| Drive | Drv Sat Bit Vel | Drive, soft-to-hard saturation character, bit reduction, amplitude velocity sensitivity |
| Wave | Pos Mov Rnd PEn | Wavetable position, movement, randomisation, pitch envelope |

Values are 0–127 except Mode (0–3) and velocity amounts (displayed −64 to +63, default 0). Detune and pitch envelope centre at 64. Carrier pitch is a MIDI-note pitch offset relative to incoming note 48. Amplitude decay spans approximately 15 ms to 4 seconds to −60 dB; closed-hat layer decay is capped at 120 ms in addition to following the original hat envelope. Velocity sensitivity 127 gives full amplitude response; 0 removes layer amplitude scaling by velocity.

Macro values and their velocity modulation are captured on each hit. Pitch, decay, feedback, detune and transient coefficients also update on the next hit. Levels, drive/character/bit reduction and wavetable position/movement can be changed during a tail. New layers start on a trigger; enabling a mode does not synthesize a new hit by itself.

Metal changes FM relationships/depth. Noise changes coloured noise amount and brightness. Aggression drives waveshaping and quantisation without increasing FM ratio/depth. Texture introduces movement/instability in metallic phase and wavetable position/pitch. Feedback remains a separate FM control. These are deliberately bounded initial algorithms; they are not yet the future tape/tube processor.

## Storage compatibility

The original kit name and parameter bytes keep their original offsets. A 180-byte trailer follows: `JB91`, schema version 1, voice count 6, parameter count 28, reserved zero, 168 parameter bytes, and a 32-bit little-endian FNV-1a integrity check. This check detects accidental corruption; it is not a security signature.

Missing, truncated, unsupported or corrupt trailers load safe defaults with both engines off. Legacy presets are not rewritten merely by loading them. Saving with old firmware will discard the extra trailer. Back up the SD card before testing round trips between firmware versions.

The existing morph system still controls original parameters. Additional-synthesis settings remain fixed during original morphing; saving a morph result snapshots the currently active additional settings. A/B/C interpolation and velocity-to-morph are deferred to Phase 2.

## Build and automated checks

On macOS with CMake, Ninja, Python 3, a host C compiler and a complete ARM bare-metal toolchain:

```sh
cmake --preset release
cmake --build --preset release
sh tests/run_host.sh
python3 tests/validate_image.py build/release/LXR_V2_V1.7.91.img --binary build/release/LXR_V2_STM32F765.bin
```

The host tests compile the production DSP, production waveform tables and storage codec with AddressSanitizer and UndefinedBehaviorSanitizer (including float-cast checks). Menu/storage fixtures compile actual functions extracted from the firmware against minimal hardware/FatFs stand-ins; they do not emulate the full UI or SD hardware. `build/host/layers-demo.s16` is mono signed 16-bit native-endian PCM at 44,140 Hz: metallic, wavetable, combined; four rising-velocity hits each. It contains only the added layers, not the original drum engines.

No extra audio buffer or scheduled MIDI delay is added. Additional computation still costs CPU time. Expensive per-hit coefficients are calculated before publishing state inside a short interrupt-masked copy. This is not a measured end-to-end latency claim.

## Required physical tests

1. Keep the v1.7.90 fallback on the Mac, and back up the SD card. Put only the intended update `.img` in the SD root (move other update images elsewhere rather than deleting the fallback). Eject the card properly.
2. Power on holding the encoder, following the upstream SD updater procedure. The image updates only application firmware, not the bootloader. Confirm the `JIMBONES custom` / `FW 1.7.91` startup screen.
3. Load existing kits with Mode 0 on every voice. Compare against the upstream baseline for sound/routing; separately compare pad/pedal behavior against v1.7.90 because its changes are unknown.
4. Check every voice, all four modes, separate levels, faders, pan, routing, FX and init/clear. Check both hi-hat buttons and rapid open→closed sequences.
5. Save/reload multiple kits and a project, restart, and confirm temporary edits restore. Verify old kits remain original-only.
6. Test DIN and USB MIDI, repeated notes, note-offs, velocities 1/16/64/127, simultaneous hits, rapid alternating-velocity rolls, clock, sequencer playback and UI edits while playing. Use the Alesis pad, KT-10, FD-8 and Ableton routing.
7. Measure worst-case rendering cost with all layers active on all voices, long decays, maximum modulation/noise/drive and heavy existing FX. Debugger-readable `jb_audioLastCycles`, `jb_audioMaxCycles` and `jb_audioOverruns` are updated using Cortex-M7 DWT. Reset max/overrun counters between scenarios. The deadline comparison uses the configured block size and sample rate; it measures rendering only, not full MIDI/interrupt load or external latency. Retain headroom for those costs. Nonzero overruns or audible dropouts fail the test.
8. Audition aliasing, transient clicks, gain buildup and rapid retrigger behavior. Reduce levels if sums clip. Modulation and nonlinear distortion are not oversampled, so high settings may produce digital aliasing.

If boot or performance tests fail, reinstall the preserved v1.7.90 image via the same updater with only that image in the SD root. Do not treat this build as stage-ready until the physical tests pass.

## Deferred work

A/B/C sound states, analogue-style tape/tube saturation, expanded filters, performance-step effects, velocity curves, new MIDI CC mappings, continuous pedal interpretation, multichannel USB audio, advanced displays and companion applications are not implemented in this release. See the master roadmap for their sequencing.
