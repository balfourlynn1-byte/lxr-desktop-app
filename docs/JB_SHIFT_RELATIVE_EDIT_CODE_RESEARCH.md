# JIMBONES — SHIFT + RELATIVE EDIT code research (2026-11-09)

Status: **research complete; implementation not yet started**. Desktop experimental branch only. Strict ARM hold.

## Confirmed source entry points
- `Core/Src/buttonHandler.c`: `buttonHandler_getShift()` reads the existing `BUT_SHIFT` bit from `din_inputData`. SHIFT already modifies mode selection in `buttonHandler_handleModeButtons` and select-button actions; do **not** globally repurpose it or break combinations.
- `Core/Src/encoder.c`: low-level quadrature decoding, debounce and detent steps; **do not** alter encoder hardware handling. Relative editing belongs in the higher-level parameter path.
- `Core/Src/menu/menu.c`: `menu_encoderChangeParameter(int8_t inc)` is the main menu parameter edit path. It reads parameter ID via `menuPages[menu_activePage][activePage].bot1 + activeParameter`, then edits `activeParameters[paramNr]`, clamps by `parameter_dtypes[paramNr]`, and dispatches `frontPanel_sendData(MIDI_CC / CC_2, paramNr, value)` or `menu_parseGlobalParam`. `preset_touchTimer(PRESET_KIT_TIMER)` runs on edit.
- `Core/Src/menu/menu.c` declares `mainParameters[NUM_KIT_SLOTS][END_OF_SOUND_PARAMETERS]` and `activeParameters[NUM_PARAMS]`; **do not assume** `mainParameters[voice]` is an independent voice array or that a sound parameter ID has a simple voice stride. Trace the parameter namespace, kit loading and CC routing first.
- `Core/Src/Parameters.c` and `Core/Inc/Parameters.h`: parameter order and IDs. Voice pages and parameter ranges may differ by engine.
- `Core/Src/frontPanelParser.c`: inbound CC handling; trace whether CC dispatch targets a specific voice or a global sound parameter, and how `activeParameters` mirrors values. The source may update via asynchronous parser; verify before multi-voice writes.
- `Core/Src/menu/menu.c` also contains existing SHIFT+knob behavior on the Performance page (`dub_delay_adjust_division`); new modifier must be limited to compatible voice-parameter pages to avoid stealing existing shortcuts.

## Key architectural risk
The editor uses a *shared parameter ID namespace* and routes edits as MIDI_CC / CC_2. Merely looping over `activeParameters[paramNr]` or blindly adding a constant stride would overwrite the wrong settings. Research must identify how voice-specific values are stored and how voice routing is selected for each CC. Also, current unsigned encoder edit code does not always saturate correctly for fast increments: validate bounded signed arithmetic before introducing global edits.

## Proposed safe implementation sequence
1. **R01 mapping table:** enumerate voice-specific parameter IDs by engine/page and identify semantic equivalence, min/max, and whether each is editable for all six voices. Mark incompatible parameters unsupported.
2. **R02 routing trace:** trace `menu_encoderChangeParameter` -> `frontPanel_sendData` -> `frontPanelParser` -> engine and kit persistence. Identify a safe per-voice update API that does not change the active voice/page or produce spurious UI state.
3. **R03 pure helper:** add host-tested `clamp(old + signed_delta, per_parameter_min, per_parameter_max)`; include negative, large detent, mixed initial values, exclusions and boundary tests. **No live UI wiring until R01/R02 complete.**
4. **R04 gated UI integration:** only when SHIFT is held on eligible voice parameter pages, apply each encoder delta once to every included voice via verified per-voice update API. Avoid changes to Step FX, global pages, mode selection and SHIFT+Performance controls. Exclusion mask, especially kick, is explicit.
5. **R05 acceptance:** host/emulator tests verify 6 independent initial values +10/-10, saturation, disabled voice, encoder acceleration, no extra writes, unchanged save behavior and unchanged normal SHIFT shortcuts. Verify no audible DSP spikes.
6. Update work tree and live checkpoint; commit and **push** each atomic step per `docs/CODEX_CHECKPOINT_AND_RECOVERY.md`.

## Required semantics
SHIFT + turn encoder adds same signed parameter delta to every included voice **without equalizing absolute values**. Example [20,45,60,35,75,90] +10 -> [30,55,70,45,85,100] for 0..100. Changes remain in working kit after SHIFT release; save only through normal kit save mechanism. Distinct from temporary hold-to-audition effects.

## R02 additional confirmed dispatch trace (2026-11-09)
- `Core/Src/frontPanelParser.c` `frontPanel_sendData` directly calls `frontParser_parseUartData` in non-AVR builds. The front-panel `frontPanel_ccHandler` updates `activeParameters[parNr]` for received parameter CC; it does **not** demonstrate independent six-voice storage.
- `Core/Src/Soundengine/MIDI/CortexParser.c` declares `frontParser_activeTrack` as the active front-panel track. The `MIDI_CC` and `FRONT_CC_2` handlers forward to `midiParser_ccHandler(cortex_midiMsg,1,0,0.f)` and record automation with `seq_recordAutomation(frontParser_activeTrack,...)`. **Important:** sending six ordinary parameter CCs while one track is selected could update/record the same track repeatedly; avoid this shortcut.
- Next: inspect the definition of `midiParser_ccHandler` and all call sites plus `frontParser_activeTrack` selection commands; identify how engine parameter storage selects a target voice, and whether an explicit voice-scoped setter exists. Verify `mainParameters` semantics and kit serialization before any edits.
- The LXR owner's manual confirms voice-specific sound pages with different parameter sets and four encoders, supporting a per-voice semantic compatibility table rather than a naive all-parameter broadcast.

## R02b discovered: explicit per-voice parameter IDs and engine pointer map
- `Core/Src/Soundengine/MIDI/ParameterArray.c` defines `Parameter parameterArray[END_OF_SOUND_PARAMETERS]` and `parameterArray_init()`. It maps **distinct parameter IDs** to actual voice-specific engine fields: e.g. `PAR_OSC_WAVE_DRUM1/2/3` point at `voiceArray[0/1/2].osc.waveform`, `PAR_OSC_WAVE_SNARE` points at `snareVoice.osc.waveform`, and other voices use `cymbalVoice` / `hatVoice`. This confirms a per-voice semantic ID lookup table is feasible, **not a uniform numerical stride**.
- `paramArray_setParameter(uint16_t idx, ptrValue newValue)` is present, but in its active `#if 1` path it routes through `midiParser_ccHandler` using `activeParameters[idx]`; it is **not automatically a safe direct setter**. Must inspect callers, type conversions and MIDI side effects before reuse.
- `Core/Src/Soundengine/MIDI/MidiParser.c` defines `midiParser_ccHandler` with a large switch mapping individual CC IDs to DSP fields. This confirms a potential supported per-ID dispatch mechanism but requires exact ID/value mapping and automation policy. Investigate how UI `activeParameters` is kept consistent with DSP state.
- R01 next: extract six-voice equivalent ID groups from `ParameterArray.c` and `Parameters.h` for cutoff/decay/pitch first; exclude waveform selection, discrete enums and incompatible engines until proven equivalent. R02 next: inspect `paramArray_setParameter` and `midiParser_ccHandler` caller semantics and `preset` serialization.
