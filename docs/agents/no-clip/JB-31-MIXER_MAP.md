# JB-31 native mixer signal-path audit — 2026-11-08

**Scope:** Source-level trace only, not hardware diagnosis or root-cause proof.

## Native per-voice path
`Core/Src/Soundengine/DSPAudio/mixer.c` `mixer_calcNextSampleBlock` renders six voices in this order:
1. `calcDrumVoiceSyncBlock(0..2)`, `Snare_calcSyncBlock`, `Cymbal_calcSyncBlock`, `HiHat_calcSyncBlock` into a reused `int16_t sampleData[OUTPUT_DMA_SIZE]`.
2. `renderLayer(voice,sampleData,gain)` invokes `jb_render` then `jb_stageVoice`, including optional JIMBONES DSP. `jb_layerOutputRailHits` counts only the JIMBONES layer and **does not observe downstream native mixer clipping**.
3. `mixer_decimateBlock(voice,sampleData)` processes each voice.
4. `mixer_addDataToOutput(mixer_audioRouting[voice],panL,panR,sampleData,output,output2,fxBusData)` sends each voice to selected DAC output or FX bus. Both output DMA buffers and the stereo FX input buffer are cleared before rendering.
5. Optional `stepfx_dubTap(voice,sampleData,fxBusData,OUTPUT_DMA_SIZE)` inserts a reduced-level stereo send when global FX type is `FX_DELAY`, dryWet >= 0.999 and that voice is not already FX-only routed. The StepFX controller defaults off; kick excluded; 64-sample send ramp.
6. `FxBus_process(fxBusData,output,output2,OUTPUT_DMA_SIZE)` applies one selected global FX and adds its output into the selected DMA channel(s).
7. `jb_audioDiagScan(output,output2,OUTPUT_DMA_SIZE)` observes the final **already quantised 16-bit** PCM output, not the pre-saturation internal sum.

## Potential No Clip sites requiring measurement
- Per-voice `jb_render` and `jb_stageVoice` before native mixer: layered saturation counter alone cannot prove the final native mix is clean.
- Native `mixer_addDataToOutput` summation and routing: saturation can occur when two modulated voices overlap.
- `stepfx_dubTap` stereo int16 saturation: intentional bounded arithmetic can still clip if multiple loud sends coincide.
- `FxBus_process` and `Delay_process` use saturating 16-bit additions; rail counts at final PCM cannot reveal pre-clamp overshoot.
- CPU render deadline misses are a separate possible cause of audible crunch; need block-level DWT cycle telemetry and physical evidence.

## Follow-up gates
- JB-32: pre/post native mixer rail and peak counters without changing audio.
- JB-34: two-voice overlapping modulation fixture that uses native mixer, not synthetic quarter-gain summing.
- JB-33: full-block CPU cycle budget and missed deadline count.
- JB-45/47: real native delay impulse-tail regression; shared global FX means sends are not independent of user delay settings.

**Evidence status:** source trace only; no hardware reproduction, no confirmed No Clip root cause, no hardware-ready signoff.
