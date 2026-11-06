# LXR-02 Retrigger Lab — v0.3 source prototype

**DEVELOPMENT SOURCE, NOT A FINISHED FIRMWARE UPDATE. DO NOT COPY THIS ZIP TO AN SD CARD.**

This experimental change was made to the complete Erica Synths LXR desktop application source supplied by the user. The **source base identifies itself as 1.7**, while the user's original update file is named `LXRV2_update_v1.71.img`. Differences between these versions have **not** been audited. This source tree has **not** been tested on an actual LXR-02.

## Changes

- `Core/Inc/Soundengine/config.h`: `LXR_RETRIGGER_SMOOTH` is a compile-time on/off switch. It is set to 1 here; set it to 0 for the original rendering behaviour.
- `Core/Src/Soundengine/DSPAudio/DrumVoice.c`: arm a retrigger correction only if the previous drum's amplitude envelope is still active. Apply it **after** the original oscillator/filter, envelope, distortion, velocity and volume processing.
- New `RetriggerSmoother.c/.h`: preserve the last mono voice sample on a repeated hit and fade the amplitude offset to zero over **132 samples (~3.0 ms at the source's REAL_FS of 44,140 Hz)**. The actual oscillator retriggers at the existing time. Uses no dynamic memory or extra audio buffering.
- `Core/CMakeLists.txt`: register the new module; identify experimental startup splash as `RTEST 1.7.90`.
- `CMakeLists.txt`: experimental project version `1.7.90` (NOT an official Erica Synths firmware version).
- `tests/retrigger/`: a six-case host C unit test and Mac runner.

## Scope and limitations

- **Only the three main drum voices** (Drum 1, 2 and 3) are smoothed. Snare, cymbal and hi-hat retain their original DSP and retrigger behaviour.
- Does not implement or add any wavetable oscillator or USB audio. The pre-existing oscillator wavetable code is not a new oscillator layer.
- No changes to CPU clock, flash layout, bootloader, external MIDI handling, project/preset format, or pad velocity mapping.
- This is an **offset-decay experiment**, not a universal de-click guarantee. It can change the first 3 ms of deliberately sharp overlapping drum transients. Recording and stress testing actual LXR output are necessary.
- The source's envelope and filter behaviour is untouched; a filter reset on trigger may remain a separate cause of audible artefacts.
- No evidence yet of CPU scheduling margin, realtime safety, stability with all six sounds, or compatibility with the user's v1.71 SD-card kits.

## Run the tests on an Intel Mac (no LXR connection required)

Open Terminal, change directory to the extracted firmware folder and run:

```sh
bash tests/retrigger/run_tests_mac.sh
```

The runner uses Apple's `cc` from the Xcode Command Line Tools. `xcode-select --install` installs them if necessary. Tests exercise the self-contained smoothing module only.

## Optional: compile the firmware on macOS, DO NOT FLASH YET

Install Homebrew (if needed) and then follow the firmware source's own toolchain instructions:

```sh
brew install cmake ninja
brew install --cask gcc-arm-embedded
cmake --preset release
cmake --build --preset release
```

If the toolchain is installed correctly the build should attempt to produce an `LXR_V2_V1.7.90.img` in `build/release/`. **That file is experimental and has not been hardware validated. Do not copy it to the LXR SD card or install it yet.**

This project's author could run the desktop module tests and a host syntax check, **but could not complete an ARM cross-build** in the current environment, because `arm-none-eabi-gcc` was missing. The exact embedded build result is unknown.

## Before any eventual device test

1. Preserve an untouched full SD card backup and the original, known-good official recovery firmware image from Erica Synths.
2. Verify the bootloader's update-file expectations and source compatibility with your device's current v1.71 firmware.
3. Check `.elf` memory-region sizes, `.img` format, version identification, and the full ARM build output.
4. Review the patch and begin with low output volume, a single long-decay drum voice, and the A/B comparison; monitor for crashes, MIDI dropouts and added clicks.
5. Make sure you understand how to use the device's updater to recover with the known-good image if an experimental application fails. A backup does not guarantee recovery from every hardware/bootloader fault.

The original project's GPL-3.0-only and third-party license notices are retained.
