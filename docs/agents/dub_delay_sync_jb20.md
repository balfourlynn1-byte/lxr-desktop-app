# JB-20 — Tempo-synchronised dub echoes on the existing global delay

**JIMBONES v1.8.04 EXPERIMENTAL.** Implemented in branch `agents/jb-20-dub-delay-sync-v184`, stacked on JB-21 (PR #10), JB-19B (#9) and JB-19A (#8). **No validated or flash-ready image.**

## What is connected

1. Performance → Performance 2 → Step FX → **Step 1 DUB SEND** (press ON, press again OFF).
2. The source voice keeps normal MIX physical routing. The Step FX gate sends a short stereo-panned copy into the **single existing FX bus**; kick is excluded by default, and send levels are conservative.
3. Set the global FX effect to **DELAY**. At each audio block start, `dub_delay_prepare()` computes the musical interval from current sequencer BPM, clocks, and selected delay mode; it retimes the *existing* `Delay` instance using the original high-range setter. **No second delay buffer or new oscillator**.
4. The FX bus uses wet-only return while DUB is armed and during a bounded four-second tail after disarming. This prevents the additive FX send from doubling the original dry hit. Note: for any voices *exclusively* MIX-routed to FX, this changes wet/dry while DUB is armed. Such an exclusive route is intentionally not parallel-sent again.
5. The FX buffer / feedback state is **never cleared** when a send gate closes. Repeats may continue. During dub mode, feedback is clamped to max 0.82 (the user's normal FX parameter is restored on exit), avoiding indefinite full feedback; tone is still the existing FX tone setting.
6. Other Step FX trigger profiles (buttons 2–16) remain exclusive; dub sends are from normal pad/MIDI/sequencer note-ons only.

## Performance UI

- Step FX **Step 1** selects dub send, press again disables.
- Step FX **knob 1** browses slots without arming.
- On Step 1's focused page: **knob 2** sets the input snippet gate length (1/4, 1/8, 1/16, 1/32 note); **SHIFT + knob 2** sets the delay echo subdivision.
- **Knob 3** sets selected voice send level, **knob 4** enables/disables selected voice; big encoder selects the voice, press toggles mask.
- OLED first row: `DUB*` armed or `DUB ` not armed, then selected echo division and status:
  - `OK`: supported and clock valid; new sends permitted
  - `LONG`: selected time exceeds existing delay memory; **new sends disabled**
  - `SHORT`: below existing high-range minimum; new sends disabled
  - `CLK`: invalid/stale external sync; no new sends
  - `NOFX`: global effect not DELAY; no new sends
  - `OFF`: DUB not armed
- Second row: gate subdivision, voice/mask and level.

## Available echo intervals (not the snippet gate length)

`1/16`, `1/8`, `1/4`, `D8` (dotted eighth), `T8` (eighth-note triplet). The one existing `FX_BUFFER[32768]` half-float buffer has nominal `REAL_FS ≈ 44140 Hz`. Original Delay.c mapping reaches ~0.742s in mono and ~0.356s in ping-pong (high-range `0.48 * FX_BUFFER_SIZE`); the mono ring is 32768 samples and pingpong splits it into 16384-sample halves. The **mapped pingpong high-range maximum is ~15728 frames**, not the full ring-half capacity. The lowest high-range time is ~883 frames (~20ms). The sync planner checks the actual mapped interval and NEVER silently wraps / aliases a too-long division.

At 120 BPM, `1/4` works in mono but not ping-pong. `1/8` (11035 samples) works in both. `D8` (16552 samples) works mono but exceeds ping-pong mapping. Unsupported divisions display `LONG` and block new throws.

## Clock and tail handling

- Internal sequencer BPM 10–400 accepted, with unsupported time status rather than clamping.
- On external MIDI or analogue sync, the mixer checks the most recently measured clock pulse age (from existing clockSync.c), rejecting old/missing sync after ~500ms. Actual timing is read from seq_getBpm(), which the existing clock synchronizer maintains. An external clock loss **does not silently switch to 120 BPM**; it suppresses new sends and continues the tail.
- BPM changes use a simple block-rate retiming hysteresis (~1 BPM tolerance and no more than four major retimes per second). Uses Delay.c's existing approximately 10ms crossfade.
- After switching DUB OFF, leaves the current tempo delay in wet-only mode for at most 4s to preserve echoes, then restores the kit-owned range/time/feedback. No delay buffer clear; it cannot guarantee a complete tail longer than the hold time or with very long/high feedback.
- Mode switches normal→pingpong during dub mask existing ring pointers so old mono offsets cannot read beyond the split ring. Audible transition is still not hardware-validated.
- Existing FX tone and physical output choice remain the user's settings. The module does not change the selected FX type.

## Code and verification

- Added `Core/Inc/Soundengine/DubDelaySync.h` and `Core/Src/Soundengine/DubDelaySync.c`; wired `mixer.c`, `FxBus.c`, `menu.c`, Step FX send accessor, and `Core/CMakeLists.txt`.
- Host: `sh tests/run_dub_delay_sync.sh` uses the production timing adapter with a minimal mock of Delay.c's public setters; it checks subdivisions/limits, clock loss, mode transition pointer safety, restoration, tail, feedback clamp, etc.
- Source: `python3 tests/test_dub_delay_source.py` checks wiring. Full regression: `sh tests/run_host.sh`.
- CI: `.github/workflows/jimbones-jb20-verify.yml` host + ARM compile only (no downloadable firmware binary).
- **Still required before hardware release:** real audio tail listening; output-routing matrix; actual DSP, PK, LATE, RAIL measurements on four simultaneous hits; external sync jitter/dropout and pingpong switch; original-kit persistence; manual DELAY parameter interactions; flash/rollback validation.

## Priority after this

Keep Metallic FM shelved due to confirmed overload and low audible benefit. Continue wavetable interpolation optimization, comb filter feasibility after CPU budget, step effects/four-voice stability. Never call compile success a hardware pass.
