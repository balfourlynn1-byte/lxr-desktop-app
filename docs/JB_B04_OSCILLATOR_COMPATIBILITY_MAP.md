# B04 — Erica / LXR02Open oscillator and modulation compatibility map

**Status: source audit complete 2026-11-08; NO compilation/audio/hardware tests.** Target branch `agents/jb-28-full-integration-v186`. Upstream pinned to `c896bfd0efe86a4037418bf9f46f1fcc39255958` (Brendan Clarke LXR02Open, Sonic Potions lineage). This document independently describes interfaces; no upstream code imported.

## Pinned exact source blobs
| Role | Erica/JIMBONES path and blob | Upstream path and blob |
|---|---|---|
| Oscillator definition | `Core/Inc/Soundengine/DSPAudio/Oscillator.h` `9b1be9f39646e636523b530fe04c70f38f3a852b` | `Core/DSPAudio/Oscillator.h` `5d8649ffcd062880fdb92b6b72f3f6f3e8817b3a` |
| Modulation node | `Core/Inc/Soundengine/DSPAudio/modulationNode.h` `a9a0bd4d1e591bf162aaad2719eaeb378ff10d0d` | `Core/DSPAudio/modulationNode.h` `03aa4619922f7e614bba2aa64a6e2cc5f1308b1e` |
| Native modulation write | `Core/Src/Soundengine/DSPAudio/modulationNode.c` `14840ec0d2db1334052616ae1ede8d51db736220` | `Core/DSPAudio/modulationNode.c` `e3a811bae6dfcb333601b02540956944873b18ee` |
| Native parameter registry | `Core/Inc/Soundengine/MIDI/ParameterArray.h` `4db991cecda528c8adf1d4f0d193d367eee8cab9` | See upstream parameter registry; exact enum values require separate pin |

## Layout and waveform differences
- Shared base `OscInfo` fields in order: `output` int16, `phaseInc` and `phase` uint32, `freq` float, `waveform` uint8, `tableOffset` uint8, `pitchMod`, `fmMod`, `modNodeValue` floats, `midiFreq` uint16, `baseNote` uint8, `startPhase` uint32. The upstream inserts additional fields **before** `midiFreq`; binary layout, `sizeof`, and offsets differ. Do NOT memcpy or reinterpret structs across firmware variants.
- Erica enum: `SINE=0 TRI=1 SAW=2 REC=3 NOISE=4 PWM=5 NUM_OSC_WAVEFORMS=6`. Upstream macros: `SINE=0 TRI=1 SAW=2 REC=3 NOISE=4 CRASH=5 OSC_SAMPLE_START=6`. **ID 5 means PWM in Erica but CRASH in upstream**; ID >=6 is upstream sample range, not an Erica waveform enum. Do not carry raw IDs across without an explicit dispatch policy.
- Upstream additions to `OscInfo`: `waveInterpNext` uint8, `waveInterpFrac` float, `waveInterpGeneration` uint32; frequency cache (`freqCacheValue`, `freqCacheWaveform`, `freqCacheValid`); sample cache (`sampleCacheIndex`, `sampleCacheLooped`, `sampleCacheGeneration`, `sampleCacheSize`, `sampleCacheData`). The cache pointer and sample dependencies require a memory and lifetime audit; no byte count is claimed without target ABI compilation.
- Erica oscillator API takes explicit `pwm` in `calcNextOscSampleBlock` and `calcNextOscSampleFmBlock`, while upstream API lacks this parameter and exposes crash/sample dispatch; upstream includes `SampleMemory.h` and `stm32f4xx.h`, Erica uses STM32F7 HAL and leaves SampleMemory include commented. Direct source replacement is both ABI-unsafe and licence-incompatible.
- Both modulation headers expose six `velocityModulators[6]` and `ModulationNode` base fields. Upstream adds `modNode_setWaveInterpEnabled`, `modNode_getWaveInterpEnabled`, and `modNode_getWaveInterpGeneration`; Erica has no matching API. Erica `modNode_updateValue` uses `activeParameters[destination]/127.f`, bipolar amount and MIDI CC/CC_2 handlers; upstream has special waveform-ID fractional handling (see B02). Modulation cannot be ported by only extending `OscInfo`.
- Six primary waveform destinations in the upstream modulation mapping are three drum mains, snare, cymbal and hat. Erica parameter registry includes named main waveform destinations for those classes, but also secondary mod-wave, cymbal/hat sub-wave and transient wave parameters. **Only main six should be considered for first native interpolation scope**, and exact integer destination values/registry pointer targets must be checked before implementation.

## Implementation decision and host-only acceptance
1. Retain Erica native oscillator ABI, parameter enum and all six voice render paths. Add independent bounded interpolation state only after explicit size/memory analysis; never transplant upstream structs or IDs.
2. For first pass, permit periodic pair interpolation only among `SINE TRI SAW REC` (0..3). Keep Erica NOISE=4 and PWM=5 on their original paths; never treat PWM as crash or sample. Explicitly reject/step across nonperiodic boundaries; later sample/noise policy needs B06.
3. Keep modulation global/per-voice interpolation default OFF, preserve original byte output and 132-sample No Clip when OFF, and clear stale target state on reassignment/retrigger.
4. Host fixture must assert ID 5 maps to Erica PWM (not upstream CRASH), invalid IDs clamp/reject, 6 main destinations only, disabled PCM exactness, velocity 0/64/127 and 3–4 overlapping long-tail hits. Actual tests remain NOT RUN.

**B04 conclusion:** Direct upstream firmware/struct transplant is unsafe. A separately authored native Erica extension is the recommended public-GPL route, subject to B07 licensing signoff and host evidence. This is a source-only compatibility finding, not a performance claim.
