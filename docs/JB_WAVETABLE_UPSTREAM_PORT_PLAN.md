# JIMBONES — upstream wavetable engineering investigation (NO ARM BUILD)

Status: SOURCE AUDIT STARTED; no firmware implementation, tests or hardware acceptance implied.
Reference: https://github.com/brendanclarke/LXR02Open (branch prime)
Upstream pinned files:
- `Core/DSPAudio/Oscillator.c` blob `5c1bf056b8016ff1a43a87f7f72ae6bf59bb4467`
- `Core/DSPAudio/Oscillator.h` blob `5d8649ffcd062880fdb92b6b72f3f6f3e8817b3a`
- `knowledge_files/OSC_INTERP_AUDIT.md` blob `6771e807239b01055d991d57a36bf0fb000c6e62`
- `LICENSE` blob `635ea7ee4e54922c03fad5795ba7c734048857b9`

## Findings from actual upstream source
- `OscInfo` stores `waveform`, `waveInterpNext` and floating `waveInterpFrac`.
- Waveform-aware modulation stores fractional positions between neighboring IDs, not only stepped waveform numbers.
- The oscillator code includes block-oriented two-waveform evaluation and blending, including FM-specific paths, rather than only crossfading four periodic lookup tables.
- Upstream design audit explicitly covers periodic, noise, crash and user-sample waveform IDs; actual runtime CPU cost and cross-class continuity need measurement, not assumption.
- Upstream is a port of Sonic Potions LXR 0.37, so voice/preset/SD format and audio-engine compatibility cannot be assumed identical to Erica firmware.

## Current JIMBONES gap
- JB-24 has an additional wavetable layer with four periodic wave labels.
- JB-27 on separate branch `agents/jb-27-native-wave-scan-v187` implements native LFO waveform scanning, but limits interpolation to four periodic waves. It has not been hardware accepted.
- Integration branch `agents/jb-28-full-integration-v186` still contains original native `Oscillator.c` without JB-27's `waveMorphState`.
- User prioritizes original three-millisecond No Clip retrigger fade and velocity-sensitive modulation; locate its precise original implementation and regression before any engine transplant.

## Targeted plan (host-side only; no ARM build)
1. Pin upstream source commit SHA, read exact interpolation dispatch and modulation write path, map all waveform categories and invalid IDs; inspect licensing provenance of each involved file.
2. Locate the original JIMBONES 3ms No Clip fade, its entry points and test; preserve it exactly at the behavioral level, including retriggers and long tails.
3. Compare native voice and modulation structures against upstream: `OscInfo`, modulation destination enumeration, waveform dispatch, sample memory and FM behavior; document which upstream behavior can be recreated in Erica code without incompatible copying.
4. Build an isolated host waveform test harness: stepped vs continuous modulation; all waveform boundaries; velocity sweep; FM and sample sources; bounds and worst-case concurrent voices. Use measurable waveform differences and listening examples, not source-string assertions alone.
5. Prototype only the smallest legal and compatible oscillator changes on a separate feature branch; keep factory voices and No Clip behavior, avoid new render allocations and metallic FM.
6. Run focused tests then full host GCC/Clang tests. Do not run ARM, package image, or lift CI ARM hold until the user approves the chosen feature-frozen candidate.

## Licence decision
The upstream LICENSE prohibits commercial use and requires complete modified source and notices on redistribution. The JIMBONES repo is public GPL-3.0-only. Personal home use is the user's intention but does not automatically permit posting incompatible source in the public repo. **Do not copy upstream files into this public branch** until distribution terms are reconciled; research, architectural comparison and independently authored implementation can proceed. A private non-commercial fork may be considered only after checking applicable licence and upstream dependencies.

Next exact action: trace original 3ms fade code and tests, and upstream `modNode_updateValue` target handling; produce a line-by-line compatibility map before writing DSP code.

## B02 — upstream modulation code audit COMPLETE (2026-11-08)
Pinned `Core/DSPAudio/modulationNode.c` blob `e3a811bae6dfcb333601b02540956944873b18ee` on `prime`.
- Lines 61–109: six velocity modulator objects; global interpolation flag initially OFF; block generation stamp; per-block active-oscillator count. `modNode_getWaveTargetOsc()` maps **six main oscillator waveform parameters** (three drums, snare, cymbal, hat), not every possible mod oscillator. `modNode_getMaxWaveformIndex()` uses sample count plus `OSC_SAMPLE_START` to bound waveform IDs.
- Lines 173–194: `modNode_resetTargets()` advances generation (avoids zero), clears active budget, restores six velocity destinations and six LFO destinations to original values. Lines 197–203: velocity modulators can be reapplied from last values.
- Lines 253–305: `modNode_updateValue()` for `TYPE_UINT8` computes modulated value from current destination, amount and input; clamps negative. If global interpolation ON and destination matches one of six mapped oscillators and active budget remains, clamps max waveform ID, writes integer base to parameter, stores next waveform and fractional part on oscillator, tags current generation and increments active count. Otherwise falls back to integer cast (stepped wave selection).
- **Engineering implication:** interpolation is both *destination-gated* and *per-block-budgeted*, with generation checks to prevent stale state. The source uses current parameter value in the modulation formula, so we must explicitly test repeated updates and reset ordering when adapting. The audit does not prove full per-mod-oscillator coverage or audio parity.
- No upstream code copied into JIMBONES. No host or ARM test performed in this source-analysis task.

## B03 — upstream oscillator renderer audit COMPLETE (2026-11-08)
Pinned `Core/DSPAudio/Oscillator.c` blob `5c1bf056b8016ff1a43a87f7f72ae6bf59bb4467`.
- Lines 98–105: **two static DTCM scratch buffers** of `OUTPUT_DMA_SIZE` int16 samples; comments specify 32-frame block interpolation. No per-hit heap allocation in this path.
- Lines 106–155: interpolation clear helper, sample metadata cache with generation invalidation, activation gate checks global enabled flag, generation equality, fraction strictly between 0 and 1, adjacent increasing IDs and sample-bound max.
- Lines 186–211: clone oscillator state twice, disable interpolation in clones, render base and next waves into separate scratch buffers, linearly mix, advance parent oscillator phase once. **Important review point:** scratch buffers are global, so verify non-reentrancy and nested calls before reuse.
- Lines 229–253: analogous FM pair rendering with the same modulation input and blend. Native FM differs from the shelved experimental Metallic FM.
- Lines 552–590: interpolation fast-path if active; otherwise native dispatcher handles SINE, SAW, TRI, REC, NOISE, CRASH and sample playback. Sample cache uses flash-backed SampleMemory generation to invalidate stale pointers.
- Risks: two full render paths per interpolating voice; rough cross-class transitions, shared scratch-buffer concurrency, sample ID bounds and sample-cache validity. This is source evidence, **not** a measured CPU or audible quality pass.
