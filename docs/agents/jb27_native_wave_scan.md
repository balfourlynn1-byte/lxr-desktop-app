# JIMBONES JB-27 — native oscillator wavetable scan (v1.8.07)

**Experimental / draft / NOT hardware-verified** — stacked on JB-26 (display glitch and bouncing skull), JB-25 (five dub-send timing pads) and earlier PR #8–#14.

## Upstream reference, copyright and scope

The reference supplied in earlier user sessions was [Brendan Clarke / LXR02Open](https://github.com/brendanclarke/LXR02Open), branch `prime`, notably `Core/DSPAudio/Oscillator.c` and `knowledge_files/OSC_INTERP_AUDIT.md`. Upstream provides per-oscillator fractional waveform-identifier scanning from LFO/velocity modulation, interpolation with a global ON/OFF and support for some sample-based sources. The user explicitly states this is for personal use.

**Important licence issue:** LXR02Open source, including derived Sonic Potions oscillator source, has a noncommercial redistribution licence requiring its notice and complete modified source. The public JIMBONES repo's original Erica firmware is marked **GPL-3.0-only plus NOTICE.md**. Personal use does not automatically give permission to republish the mixed code under GPL. **No original upstream code was copied** into this branch. Only the general functional algorithm and UI pattern were studied; all JB-27 code is independently authored, uses existing JIMBONES/Erica GPL oscillator tables and makes no unsupported new licensing claim. Full exact upstream code redistribution or sample-player port would require a separate, explicitly licensed/private fork and verified hardware/memory constraints.

## What is newly implemented — actual native voice path

JB-24 already added a *separate* per-voice wavetable layer with wave position, phase warp, wave folding and 16×2 UI. JB-27 extends the underlying original LXR sound architecture:

1. Added 32-bit atomic `OscInfo.waveMorphState` (runtime only, not kit/SD data) for waveform position and 0..32767 blend fraction.
2. Existing built-in LFO waveform destinations now recognise the **13 original native oscillator selector fields**: 6 primary (drum 1,2,3, snare, cymbal and hi-hat), plus existing three drum mod oscillators, two cymbal mods and two hi-hat mods.
3. `modNode_updateValue` retains the existing MIDI CC logic and saves **fractional positions** independently, so a smoothly changing LFO is not confined to the nearest whole waveform ID.
4. The native `calcNextOscSampleBlock` now chooses an alternative, non-overclocked, no-new-buffer path **only for active smooth interpolation**. It blends two lookup values from original sine, octave-banded triangle, saw or rectangular tables **at the same phase**, using integer Q15 mixing and inexpensive 8-bit subtable interpolation. This is **not** an additional oscillator voice or second synthesizer engine. Phase increment and output routing remain original.
5. A global **OSC INTERPOLATE** ON/OFF control (already in JB-24) controls native interpolation as well as the JIMBONES wavetable layer. OFF, inactive targets, fixed waveform IDs and nonperiodic native waveforms use the exact original mixer/synthesis path; no per-sample two-table work on those cases.
6. The original noise/PWM engines, FM modulation paths and all existing sample/kit structures are **unchanged**. Metallic FM experimental layer stays compiled out. This keeps CPU exposure limited after previous audible crunch on the older build.
7. `modNode_resetTargets` clears any previous scan word before each fresh LFO/velocity modulation pass, preventing stale waveform morphing when targets are changed.

### How to test on the device (after image/rollback review)

- Select the voice on the original LXR OSC page; keep the original oscillator enabled.
- Set that voice's native **LFO destination** to its OSC waveform parameter (or a native mod-osc waveform parameter). Choose a noticeable LFO amount and a slow rate. This is the same regular LFO parameter system the original synth already has.
- On the JIMBONES wavetable OSC extension, switch **OSC INTERPOLATE** ON. Listen for continuous movement through SINE → TRI → SAW → REC. Switch interpolation OFF to compare stepped native wave selection.
- Also test the separate JB-24 wavetable Position, Shape and Character controls. These remain layered, not copied from LXR02Open.
- Test four overlapping hits and report DSP, PK, LATE, RAIL; *do not assume the original hardware headroom still holds* when several native LFO→wave targets are active.
- Monitor Step FX 1–5 dub timings and the JB-26 bouncing skull/wake screen on this cumulative build.

### Limitations (do not present as full upstream port)

- Only **four periodic native oscillator waves** can be smoothly crossfaded. NOISE and PWM remain discrete to avoid doubling their stateful/random render cost. The stock Erica branch has no working equivalent of upstream user-sample waveform-ID selection or sample/loop installation; those parts of LXR02Open were NOT ported.
- Modulation in this version is **block-rate** (existing native LFO controls); interpolation is per-sample only between two periodic tables, not a new sample playback system. Other FM generation remains on the original path.
- Exact DSP load, sound quality, tone, LCD glyph appearance, LED timing, and memory/runtime stability require hardware testing. Successful host and ARM tests do not prove these.

## Verification and safety gates

- Host: `sh tests/run_jb27_native_wave.sh` tests packing/rounding, off-state, invalid inputs, mixed 16-bit samples and bounds, plus asserts the real LFO→Oscillator wiring in source.
- Full GCC/Clang regressions cover six native voices, JB-24 menu, JB-26 screensaver generator, and JB-25 dub FX.
- ARM cross-compile, image-format/CRC check, >=512B ITCM remaining, checksum-pinned experimental image (no automatic flash/release).
- `v1.8.07` is intentionally distinct from `v1.8.04` (user-tested), `v1.8.05` (dub timing pads), and `v1.8.06` (screensaver/display fix).
- Do not merge or replace known-working image until code review plus real-device acceptance.

## Next engineering stage

Follow up with device waveform-LFO listening and CPU benchmarking. Only after safe CPU margin is confirmed should one attempt nonperiodic cross-class transitions, sample ROM and loop/wavetable installation, user-sample persistence, or expanded waveform arrays in a separately licensed effort.
