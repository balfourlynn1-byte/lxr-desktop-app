# JIMBONES P0 investigation — initial code audit

Status: **research completed, implementation pending**. Reviewed `Core/Src/Soundengine/DSPAudio/Jimbones.c` on the experimental branch. These are code-level hypotheses, **not confirmed hardware defects**.

## JB-02 — CPU monitoring: proposed measurement plan
- Use STM32F765 Cortex-M7 DWT CYCCNT to time the **entire audio processing callback**, not just `jb_render()`.
- Determine real callback buffer length, sample rate, core clock, and wrap/interrupt behaviour from actual integration code before implementation.
- Record last, peak, smoothed CPU cycles and over-budget count; make counter reads safe for UI. Do not allocate, log, or format text in audio ISR.
- Add a compile-time opt-out and host-testable budget math. Validate with all six voices and maximum simultaneous effects.
- Distinguish CPU deadline misses from clipping; zero clipped samples alone cannot establish absence of crunch.

## JB-03 — multi-voice crunch: code audit observations
1. `jb_render()` processes each voice with `out*gain*15500*mix`, then applies a per-voice saturation curve and a headroom-dependent sum. This is not the same as a final output peak limiter; inspect the complete voice mixer and post-FX for overflows.
2. `jb_stageVoice()` uses `kitWeight` shared across voices, computed in `jb_prepareBalance()`. Verify call ordering once per audio block and test cases with mixed active/inactive layers. The current rule `kitWeight*8` can substantially change dry gain on non-kick voices.
3. `jb_layerSaturationHits` counts local layer saturation, **not** full-mix clipping or audio deadline misses. Expose distinct counters rather than treating it as an all-clear.
4. `jb_trigger()` computes expensive coefficients outside its short IRQ lock and publishes a full layer state inside the lock. Confirm triggering context, interrupt latency and concurrent rendering semantics.
5. `jb_render()` calculates motion, phase distortion, wave folding, FM and drive inside the sample loop. Profile their combined worst-case cost; do not infer CPU load from host stress-test speed.

## Regression fixtures required
- Six voices, 3–4 simultaneous hits, long overlapping decays.
- Single modulated voice vs 2, 3 and 4 modulated voices.
- High-velocity repeated snare/tom hits, kick unaffected.
- Separate metrics: output saturation, layer saturation, deadline overruns, discontinuities at retrigger and edits.
- Compare against v1.8.02 on identical patch/audio setup when baseline becomes available.

## Next engineering handoff
**Embedded agent:** identify the audio callback entrypoint and clock configuration, then instrument DWT cycle timing behind an experimental build flag.

**DSP agent:** add a reproducible 4-voice modulation stress fixture and investigate `kitWeight` gain staging with testable before/after audio.

**Verification agent:** gate all changes on host regression, ARM compilation, image integrity and hardware confirmation.

No source DSP behaviour was changed in this research pass. v1.8.02 remains untouched.
