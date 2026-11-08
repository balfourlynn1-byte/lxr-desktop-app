# JB-22 — JIMBONES v1.8.04 integration / release gate

**STATUS:** Source changes committed to isolated `agents/jb-22-integration-gate-v184` branch. All JB-19A/B, JB-21, JB-20 commits are ancestors (draft PRs #8, #9, #10, #11). **NOT A STABLE OR USER-APPROVED FLASHABLE FIRMWARE.** Cross-build status is reported by `.github/workflows/jimbones-jb22-integration.yml`, never assume GREEN until the actual run finishes.

## Explicit user decision: remove Metallic FM from the audio render

User reports the experimental Metallic FM sounded too similar to the original voice and cost too much CPU. Testing on v1.8.03: idle DSP ~62%, PK 66, LATE 0, RAIL 0; crunch with two modulated toms at DSP ~94%; one stress case DSP 101%, PK 105, LATE 999, RAIL 240. These **are NOT readings from v1.8.04**.

On the original JB-20 branch, FM selectable Modes 1 and 3 and the FM per-sample loop still existed, even though earlier handoff marked Metallic FM shelved. JB-22 now compiles FM synthesis, oscillator modulation and carrier phase math OUT with `JB_METALLIC_ENABLED=0`. Legacy data layout, 32 controls/voice and v1/v2 kit trailers remain unchanged for rollback/compatibility. Legacy mode=1 (Metal only) now has no extra layer and preserves original dry sound, including pre-filter and fader bypass. Legacy mode=3 (Metal + Wave) now sounds **identically** to mode=2 (Wave only) for the same patch values. Wavetable interpolation, wave position, phase warping and folding remain enabled. UI labels state that FM is unavailable. New host regression confirms all of this.

Removing code from the signal path *should* lower DSP demand when previously configured Metallic FM was active, but **NO new DSP %, PK, LATE or RAIL measurement can be claimed** until tests on the actual LXR-02 with the same kits, velocities and pad combinations.

## Integrated functionality to verify

- JB-19A: six independent short send envelopes, global OFF by default, kick excluded by default
- JB-19B: send an additive copy to the shared FX bus, do not reroute/retrigger dry voice, avoid duplicate when the voice is already OUT=FX
- JB-21: each of the sixteen buttons toggles ON/OFF on second press; Step 1 now DUB SEND, LED 1 should track armed state. Host/source tests do not prove the actual LED lights up.
- JB-20: Step 1 send throws into existing FX DELAY; repeat subdivisions 1/16, 1/8, 1/4, dotted 8th, triplet 8th if the 32,768-sample buffer can hold it; clock freshness and buffer validity statuses; protected feedback tail when gate closes, without clearing buffer.
- No additional FX processor; one global FX type only. Existing sound/kit/output routing must be preserved with DUB OFF.

## Host / ARM test gates

1. `sh tests/run_host.sh` using both GCC and Clang — all original host tests, StepFX scheduler, full send UI/mixer tests, timing and delayed-tail tests, new FM off exact-bypass test
2. `bash tests/run_combined_stepfx.sh` — existing trigger FX regression
3. `cmake --preset release && cmake --build --preset release --parallel 2` under ARM cross compiler — ensure no missing symbols or overflow
4. `python3 tests/check_jb22_memory.py build/release/arm-build.log` — require >=512 bytes remaining in 16KiB ITCM for code growth; the JB-20 build used **16,376/16,384 bytes (8 bytes spare)**, a severe maintenance risk. JB-22 moves control-rate `FxBus_updateParameter` and `FxBus_setParameter` out of ITCM, retaining real-time `FxBus_process` there.
5. `python3 tests/validate_image.py build/release/LXR_V2_V1.8.04.img --binary build/release/LXR_V2_STM32F765.bin` — container/vector/header/CRC check, outputs SHA256 for reproducibility, **does not provide flash approval**. No firmware artifact is uploaded from CI.

## Mandatory manual hardware test before user can flash an experimental candidate

- Back up the device's existing working firmware and all kits. Keep the exact currently bootable image/rollback method documented. Do not overwrite or auto-merge working firmware.
- Confirm firmware boots from backup image and all SIX original voices play, with original kits, MIDI pads and 4 hardware outs.
- Record PK, DSP, LATE, RAIL **after restart at idle**, with original voices only, with wavetable enabled but no Metallic FM, then with Step FX DUB SEND OFF and ON.
- Play **two overlapping modulated toms** and **four simultaneous pad hits** with long overlapping decays, both with and without FX sends, first with FX delay feedback low and output volume low. Any crackling, DSP>=100%, LATE accumulation or unexpected RAIL spike fails the performance gate.
- Verify physical Step 1 LED lights when armed and clears when turned off; 2–16 toggle and display consistently; normal Performance manual rolls and CPU diagnostic Step 1 reset remain unaffected.
- Check FX OFF, OUT=FX, original mono/stereo output mappings, dry source plus wet repeats, mono and pingpong supported/unsupported division warnings, feedback cutoff/recovery after send closes.
- Test external MIDI clock stable and unplugged/stopped, time subdivision changes with no repeated smearing. Do not infer perfect external clock accuracy from host tests.
- Listen for zipper/clicks, especially on mono↔pingpong switches; verify saved kits are loadable and the UI clearly indicates disabled Metallic FM modes.
- Final sign-off: no observed glitches with 4-voice stress plus acceptable headroom, no sustained late deadline count; **hardware test results, device version and exact image checksum must be recorded**.

## Remaining known limitations / warnings

- ITCM margin may still be tight; this is independently gated in CI.
- FM removal may have little effect if the old kit never actually activated FM; do not assume DSP reduction quantitatively.
- DELAY is the global effect type. DUB wet-only mode also affects voices exclusively routed to the shared FX bus; dry physical source output remains independent.
- The echo tail is preserved for a bounded four-second period on DUB OFF, not infinitely.
- Host simulated FX control cannot prove full physical sound or timing under heavy load.
- Existing JB-20 synchronous send module uses MIDI/control-thread → audio-thread atomic requests; all races require stress validation.
- Do not integrate further comb/reverb/extra waveform engines until measured four-hit CPU headroom is acceptable.
