# JB-12 — New per-voice comb filter in the original LXR-02 FILTER page

**JIMBONES v1.8.04 experimental.** This branch is stacked on the JB-23 FM-free wavetable UI hardware-test pack. It does not overwrite that previously booted v1.8.04 candidate and **is not a hardware-tested release**.

## Operator mapping

- Select any of the six drum voices and navigate to its normal **FILTER** section (Freq, Reso, Type, Drive). The open hi-hat shares the sixth filter with closed hi-hat.
- On **Type**, turn through the existing eight modes **LP, HP, BP, UBP, Nch, Pek, LP2, off**. New **Comb** follows **off**, as a ninth choice. The eight old numeric filter IDs stay exactly the same for saved kits.
- In Comb mode:
  - **Freq** sets a short recursive delay: lower Freq = longer delay/lower comb pitch; higher Freq = shorter delay/higher comb pitch.
  - **Reso** controls feedback and the depth of the comb notches; at zero the audio output is **bit-identical to the input**.
  - **Drive** is left as existing filter drive parameter; no new overdrive stage was added for Comb.
- The same FILTER controls work on all six synthesis voices, including the three tom/kick voices and snare, cymbal and hi-hat.

## DSP implementation and budget

- `CombFilter.c/.h`: one 256-sample signed 16-bit ring per synthesis voice, embedded in each existing `ResonantFilter`. Six separate histories; no dynamic allocation and no additional FX bus/delay instance.
- Feed-forward normalisation with bounded positive feedback: `y[n]=(1-a)*x[n]+a*y[n-d]`, where `0 <= a <= 220/256`; 16-bit saturation and bounded read/write indices. Delay length 16–255 samples, corresponding to a roughly 173Hz–2.76kHz resonant comb spacing at ~44.14 kHz sample rate.
- Uses integer fixed-point arithmetic **per sample** and computes frequency and resonance coefficients only **per audio block**. Parameter movement changes the delay by at most 1 sample per block, to reduce abrupt zippering (not a true crossfade).
- Filter OFF and all existing LP/HP/BP/UBP/notch/peak/LP2 paths are unchanged. When Comb is disabled or Reso is set to zero, its history is invalidated so old echoes cannot reappear.
- Comb ring storage per voice is approximately 512 bytes + state. Six voices add roughly **3 KB DTCM/RAM**; no second global delay allocation. The additional branch to select Comb in `SVF_calcBlockZDF` runs once per block, not per sample. Actual ARM memory map and hardware DSP margin require CI and device checks.

## Compatibility

- Uses original `PAR_FILTER_TYPE_1..6`, `PAR_FILTER_FREQ_1..6`, and `PAR_RESO_1..6` parameters. Existing kit/pattern layouts remain unchanged.
- The native FM parameters seen on the original OSC/FM subpages are **not the removed experimental Metallic FM layer**. They are part of the six original synthesis engines and have deliberately not been removed.
- The wavetable UI and StepFX/DUB code from JB-24/JB-23 remain intact.

## Verification requirements

- `sh tests/run_jb12_comb.sh` tests zero-feedback bit-identical bypass, impulse echo, separate voice histories, 384,000 heavy input samples, memory canaries, bounded/saturating output and routing source checks.
- `sh tests/run_host.sh` full regression, GCC and Clang; `bash tests/run_combined_stepfx.sh` original StepFX.
- `cmake --preset release && cmake --build --preset release` ARM cross compiler and `tests/check_jb22_memory.py` must satisfy >=512 bytes ITCM free; test image integrity but **do not distribute firmware until review**.
- **Hardware required:** verify filter Type Comb on each voice, frequency/pitch and resonance range, no added dry coloration when Reso=0, no abnormal clipping, sustained two-tom/four-pad load and current DSP/PK/LATE/RAIL. Test on small volume with known rollback.
- If ITCM or runtime DSP headroom fails, keep this PR experimental and reduce/eliminate work rather than relaxing the margin.

## Related future jobs

- The separately requested [LXR02Open](https://github.com/brendanclarke/LXR02Open) waveform modulation / interpolation for the **native oscillators** is not part of this filter PR. JB-24 provides a wavetable UI to the additional JIMBONES layer only, which explains why it does not fully match the demo. Native oscillator interpolation should be a separate CPU-capped feature after the comb and FM-free candidate are measured.
- The upstream non-commercial source should not simply be pasted into the public GPL-only repository without licence resolution, even for an intended personal-use binary; functional UI/algorithm design can be implemented independently.
