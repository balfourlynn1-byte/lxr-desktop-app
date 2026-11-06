# JIMBONES No Clip + Step FX — v1.7.92
This build contains no Metallic/Wavetable synthesis. Defaults: all effects OFF, kick excluded, other voices included. Settings are live-session controls, not saved per kit.

Press Performance normally to cycle original page 1, original page 2, STEP FX, page 1. On STEP FX, buttons 1–16 select the effects below. Slot 1 cancels every pending repeat immediately (existing audio decays normally). Leaving the page keeps the selected effect active; return and press slot 1 to bypass. Stop cancels scheduled repeats. Dry MIDI strikes always trigger immediately through the original path.

|Button|Effect|Default beat fraction|Scheduled repeat opportunities|
|---|---|---|---|
|1|OFF|—|0|
|2|Repeat|1/2|3|
|3|Repeat|1/4|3|
|4|Repeat|1/8|7|
|5|Repeat|1/16|7|
|6|Stutter|1/4|15|
|7|Stutter|1/8|31|
|8|Stutter|1/16|31|
|9|Probability repeat 50%|1/4|15|
|10|Probability repeat 50%|1/8|31|
|11|Velocity variation ±48|1/4|15|
|12|Velocity variation ±48|1/8|31|
|13|2-bar cycle|1/4|32|
|14|4-bar cycle|1/8|128|
|15|8-bar cycle|1/8|256|
|16|8-bar random: 50%, velocity ±64|1/16|512|

Beat means a quarter-note: at 120 BPM, 1/2 beat = 250 ms and 1/16 beat = 31.25 ms. Bar means four quarter-notes. First repeat falls on the next grid division; counts span the stated length from that grid point, not global song-bar alignment. Changing rate retains repeat count, so the duration changes. Each newer live hit replaces its voice's schedule. Both hat articulations share one lane. This is trigger stutter, not sampled-audio looping.

Knob 1 selects slot, knob 2 selects division, knob 3 adjusts probability, knob 4 includes/excludes the selected voice. Encoder rotation selects voice 1–6; encoder click toggles inclusion. LCD `V1-` means kick excluded, `V1+` included; ON/-- shows effect state. Rate changes or selecting any slot clear pending events. Seven MIDI articulations route to six bounded lanes; maximum six emissions per block. Generated strikes go directly to original synthesis, do not recursively schedule or transmit MIDI notes. Muted tracks suppress generated strikes.

Internal BPM and external MIDI clock supported. External effects wait for clock and cancel after 0.5 seconds without clock. Analog clock is not supported by this scheduler: in that sync mode, dry input remains but effects receive no clock. MIDI clock smoothing follows existing tempo estimate, with at most one-tick extrapolation. No new waits, heap allocation or unbounded queues. Real repeats are quantised to the existing 64-sample audio block (~1.45 ms).

Tests: `sh tests/run_stepfx.sh`; shared production DSP suite in `tests/production`. Includes actual button-handler/LCD 1–2–3–1 cycle, all 16 step selectors, engine timing, masking, hat replacement, probability, stop/bypass/clock-loss, finite cycles and recovered smoother cases.

## Foundation and build
Independent branch from recovered No Clip commit df39d7f. That source built byte-for-byte to the user's working v1.7.90: SHA-256 `57ce6e6a6ad729d4e7a4c1d657fce10e39c0c94f94a24abc078442afe74b266c`. The recovered 132-sample smoother and its integration remain in the three original drum voices. Original snare/cymbal/hat algorithms are retained.

Build with ARM GNU 16.1.0, CMake and Ninja: `cmake --preset release -DCMAKE_C_FLAGS=-fstack-usage`, then `cmake --build --preset release`. Existing desktop updater container, no bootloader changes. `python3 tests/validate_image.py build/release/LXR_V2_V1.7.92.img --binary build/release/LXR_V2_STM32F765.bin` verifies header, checksum, payload and vectors. Native tests do not replace physical testing. Debugger variables `jb_audioLastCycles`, `jb_audioMaxCycles`, `jb_audioOverruns` measure render duration; they exclude MIDI/other ISR work and need on-device observation. Emulated counters are disabled.

Back up the SD card and keep the original No Clip image off-card. Place only the chosen update image in the SD root (move other updater images off-card), power off, insert card, hold the encoder while powering on, wait for LXR Updater and Update OK. Never interrupt flashing. Restore v1.7.90 the same way to roll back. Test independently before live use.
