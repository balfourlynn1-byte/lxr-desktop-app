# B06a — Native oscillator, modulation and memory compatibility gate (2026-11-09)

**Status: VERIFIED SOURCE SUBTASK ONLY; B06 remains OPEN.** No executable host test or hardware test was run. No upstream code copied.

## Exact native evidence
- `Core/Inc/Soundengine/DSPAudio/Oscillator.h` blob `9b1be9f39646e636523b530fe04c70f38f3a852b`: `OscInfo` contains `output`, `phaseInc`, `phase`, `freq`, `waveform`, `tableOffset`, `pitchMod`, `fmMod`, `modNodeValue`, `midiFreq`, `baseNote`, `startPhase`. Its API consumes a hardware HAL header, integer wavetable blocks and native `calcNextOscSampleBlock`/`calcNextOscSampleFmBlock` dispatch. Source does not declare upstream interpolation generation, two-slot buffers or sample-cache fields. Do not overwrite its ABI with upstream struct layout.
- `Core/Src/Soundengine/DSPAudio/modulationNode.c` blob `14840ec0d2db1334052616ae1ede8d51db736220`: `INDTCMZ ModulationNode velocityModulators[6]`; initial amount zero; `modNode_resetTargets` restores original parameter values; dispatch traverses three drums, snare, cymbal and hat. A new wave destination must preserve original-value restore and per-voice target isolation, rather than reinterpret existing parameter indices.
- `Core/Inc/Soundengine/config.h` blob `b5f49754a3bfebd7133a124fc9d8d56d7a3ae807`: `OUTPUT_DMA_SIZE 64` when `USE_BIG_SAMPLE_BUFFER 1`; `REAL_FS (float)44140.f`; `NUM_DRUM_VOICES 3` (three drum engines, not total six sound voices); `LXR_RETRIGGER_SMOOTH 1`; `IN_ITCM_RAM` and `INDTCMZ` section attributes. An emulator cannot infer STM32 ITCM/DTCM usage from desktop heap timings.
- `Core/Inc/Soundengine/DSPAudio/Jimbones.h` blob `a1910ddce801eac70b3ecfdf5a20f41b3ff14cda`: six JIMBONES layer slots and fixed packed `JB_KIT_BYTES = JB_VOICES * JB_PARAMS`; `JB_METALLIC_ENABLED 0`. Preserve serialized parameter IDs; do not append unversioned bytes or activate Metallic FM.

## B06 remaining evidence gates (NOT DONE)
1. Pin upstream sample flash/SD memory layout, original and native kit/global versioned serializers and recovery paths. Do not assume SD and flash sample formats interchange.
2. Map native and upstream MIDI parser/clock/velocity/note-on call sites, plus screen/menu/encoder ABI and v1.8.06 skull wake/animation source identity.
3. Host-only tests: exact legacy kit round-trip, truncated/unknown extension safe fallback, six-voice isolated target reset, DMA 64-frame split, sample rate/fade continuity, malformed SD state, MIDI timing and skull display/wake; execute GCC/Clang and capture checksums.
4. Evaluate static RAM/ITCM budgets from source/map after user approval; **do not run ARM compiler** in this phase.

**Architectural finding:** adapt Erica-native oscillator in a separately gated extension rather than transplanting upstream structs or memory layout. This is a compatibility recommendation, not implementation proof. See B04/B05 for waveform-ID and velocity mapping.
