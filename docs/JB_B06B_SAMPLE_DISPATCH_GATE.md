# B06b — Native sample dispatch boundary (source-only, 2026-11-09)

Status: VERIFIED SOURCE FACTS / NO HOST EXECUTION. This closes only the native sample-dispatch subsection of B06; upstream memory ownership, serialization, MIDI/clock and display remain OPEN.

Pinned integration branch `agents/jb-28-full-integration-v186`:
- `Core/Inc/Soundengine/DSPAudio/Oscillator.h` blob `9b1be9f39646e636523b530fe04c70f38f3a852b`, lines 25–35: native enum `SINE=0, TRI=1, SAW=2, REC=3, NOISE=4, PWM=5, NUM_OSC_WAVEFORMS=6`.
- `Core/Src/Soundengine/DSPAudio/Oscillator.c` blob `009226ea23cc07bd0706a42bd56d7ed6a3fe93ac`, lines 375–409: the **non-FM** block renderer dispatches the six waveforms above; the `default` sample playback path is **commented out**, so unknown waveform IDs are not handled as playable samples in this function.
- Same blob, lines 555–615: `calcUserSampleOscFmBlock` and `calcUserSampleOscBlock` contain sample-memory playback logic wrapped in `#if 0` (not compiled). Do not treat their internal sample pointer arithmetic as a live memory or CPU path.
- `Core/Src/Soundengine/DSPAudio/Samples.c` blob `97df08df1df985f382a3d289879735bbcbd5fad2` contains a 32,768-byte crash sample array. Its existence does **not** establish a reachable oscillator playback path. `Samples.h` blob `c01908420c4964c42c2aed41c3d44da62f9d4a2c` has the array declaration commented out.

**Design gate:** do not assume that upstream sample IDs can be grafted onto native waveform IDs; retain 0–5 native dispatch unchanged. A fractional waveform scan must reject/clamp out-of-range IDs **before** dispatch, default OFF, and preserve exact old output when OFF. Crash/sample integration is deferred pending explicit memory/ownership and live-call-path verification. Avoid copying non-commercial upstream code/assets into GPL-only repository.

**Exact future host tests:** (1) render all native IDs 0–5 with interpolation OFF and compare byte-identical reference buffers; (2) pass 6, 7, 127, 255 through new proposed bounds adapter and assert no sample read/out-of-range; (3) sweep 0–3 fractional positions with monotonic, finite bounded samples; (4) verify 3–4 concurrent voice stress and no change to 132-sample No Clip fade. None executed here; no ARM/image/hardware action.
