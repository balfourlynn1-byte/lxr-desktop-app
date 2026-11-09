# H01 — LXR02Open oscillator interpolation source map (2026-11-08)

**Status:** SOURCE AUDIT COMPLETE for identified interpolation path; **host audio/CPU tests NOT RUN**, native implementation NOT DONE. Reference pinned at `brendanclarke/LXR02Open@c896bfd0efe86a4037418bf9f46f1fcc39255958`. No source code copied.

## Exact upstream files and blob SHAs
- `Core/DSPAudio/Oscillator.c` blob `5c1bf056b8016ff1a43a87f7f72ae6bf59bb4467`
- `Core/DSPAudio/Oscillator.h` blob `5d8649ffcd062880fdb92b6b72f3f6f3e8817b3a`
- `Core/DSPAudio/modulationNode.c` blob `e3a811bae6dfcb333601b02540956944873b18ee`
- `knowledge_files/OSC_INTERP_AUDIT.md` blob `6771e807239b01055d991d57a36bf0fb000c6e62`

## Modulation path
1. `modulationNode.c:62-71`: block-scoped interpolation slot counter, generation and enable state. First eligible targets win; remaining targets fall back to integer waveform IDs. This creates **voice-order starvation** under >2 simultaneous interpolating targets; do not assume 4 concurrent interpolated voices.
2. `modulationNode.c:73+`: `modNode_getWaveTargetOsc()` maps waveform parameter IDs to oscillator objects.
3. `modulationNode.c:97-109`: enable and generation APIs.
4. `modulationNode.c:173-179`: `modNode_resetTargets()` increments generation and resets slot count each DSP block.
5. `modulationNode.c:253-304`: `modNode_updateValue()` clamps modulation, chooses base and next wave IDs, stores fractional position and generation in `OscInfo`; if disabled or slots exhausted, stores an integer ID.

## Oscillator path
1. `Oscillator.h`: waveform IDs SINE=0, TRI=1, SAW=2, REC=3, NOISE=4, CRASH=5, user samples from 6. `OscInfo` holds `waveInterpNext`, `waveInterpFrac`, `waveInterpGeneration`, phase and cached sample metadata.
2. `Oscillator.c` `osc_waveInterpActive()`: interpolation only when global enable, generation, frac strictly between 0 and 1, next > base and both IDs in range. The default/OFF path uses original dispatch.
3. `Oscillator.c` `calcPeriodicInterpBlock()`: copies oscillator state, renders both neighbour waveforms into two shared `OUTPUT_DMA_SIZE` int16 scratch buffers, linearly blends `a + frac*(b-a)`, then advances phase once for the original oscillator.
4. `Oscillator.c` `calcPeriodicInterp()`: single-sample path evaluates both waveforms at the same phase then advances once.
5. `Oscillator.c` `calcPeriodicInterpFmBlock()` and `calcPeriodicInterpFm()`: FM-modulated *render paths* have equivalent blends; this does **not** mean the separate FM oscillator is an eligible waveform-modulation destination.
6. `Oscillator.c` `osc_updateSampleCache()`: sample cache invalidates on sample manifest generation changes; avoids repeated metadata lookup for unchanged sample IDs.
7. `OSC_INTERP_AUDIT.md`: describes simple adjacent-ID linear blending including noise/crash/sample transitions; cross-class sounds can be rough, no spectral matching; global default OFF, saved in globals. The audit's historic `make -j4` claim is not a test result for JIMBONES.

## Risks / concrete follow-up tests
- **CPU:** two full neighbour renders + blend for each active interpolator; measure 0/1/2 slots and 3–4 overlapping hits. Avoid previous Metallic-style crunch.
- **Fairness:** slot assignment depends on modulation update order; test which voice loses smooth interpolation with 3+ simultaneous wave targets, and define priority or rotating allocation.
- **Memory/reentrancy:** two global scratch buffers are shared; verify renderer is never re-entered and block length <= OUTPUT_DMA_SIZE. Test 1/32-frame and block boundaries.
- **Sound:** cross-class blends (periodic/noise/sample) may produce harsh transients; evaluate periodic-only alternative and level-matched A/B.
- **State:** test generation invalidation, stale fraction after OFF or target change, phase continuity, user-sample cache invalidation, zero-depth exact bypass and preset reload.
- **Licensing:** upstream Sonic Potions-derived source has noncommercial/source-redistribution terms; treat as reference, not code to paste into public GPL-only JIMBONES. Formal compatibility review B07 still pending.

**Next H01 acceptance:** implement host-only source contract checks and H04 audio fixtures; this map is source analysis, not DSP implementation or hardware validation. ARM hold unchanged.
