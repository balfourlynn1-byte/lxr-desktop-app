# JB-19 / JB-20 / JB-21 — parallel Step FX dub-send: implementation handoff

**Target:** JIMBONES v1.8.04 **experimental**, no image ready yet.  
**Status:** Source- and manual-researched design; implementation, compilation, CI and hardware testing **not performed**.  
**Owners:** DSP (audio routing); Embedded (controls/clock); Verification (regression and load); Integration (safe merge).  
**Priority:** P0 after protecting the current known-good firmware and diagnosing the existing overload.  
**Design intent:** A normal live snare/tom/hat strike stays on its chosen physical output. While a Step FX send gate is active, a short *copy of its rendered audio* is sent to the existing global effect (particularly dub delay). When the gate closes, the original dry voice is unaffected **and the delay feedback tail continues**. This is not a retrigger of the voice and not an exclusive FX output route.

## Research evidence: official hardware manual

Official Erica Synths LXR-02 desktop owner's manual: https://www.ericasynths.lv/media/LXR_manual.pdf
- Page 15 / MIX: voice OUT selects stereo, individual mono, or **FX** destination. It does not describe an independent adjustable parallel send.
- Page 26 / FX: one active global effect at a time (drive, ring modulation, compressor or delay); FX is effectively an insert-style shared processor, selected via **SHIFT + FX**, and saved with kit.
- Page 27 / DELAY: delay time, range, tone, feedback and dry/wet; existing document specifies approximately **20 ms to 0.7 seconds** in the high range. Standard effects are mono/ping-pong, not independent per-voice processors.
- Page 7 / panel: performance buttons 1–7 have manual-roll functions; avoid stealing existing performance behaviour on the original page. A dedicated Step FX performance subpage is available in JIMBONES.
- Page 34 / outputs: four mono physical outputs can be paired; do not silently reroute those outputs.

## Source audit (branch agents/wavetable-research, 2026-11-08)

- `Core/Src/Soundengine/DSPAudio/mixer.c`: `mixer_calcNextSampleBlock` renders six voice blocks into `sampleData`, applies `renderLayer`, decimates, then calls `mixer_addDataToOutput(..., fxBusData)` for the **exclusive** destination route. `fxBusData` is a zeroed stereo block. `FxBus_process(fxBusData, ...)` runs after all voice renders, and `jb_audioDiagScan` scans outputs. **Best candidate injection:** after each voice's `renderLayer` / decimation, *before* or beside its existing `mixer_addDataToOutput`: bounded, saturated, independently gated COPY to `fxBusData`, followed by unchanged legacy dry routing. Do not re-render the voice.
- `Core/Src/Soundengine/FxBus.c` / `Core/Inc/Soundengine/FxBus.h`: one global `FxBus` with `fxType`, `audioRouting`, `dryWet`, and `Delay delay`. Existing FX_DELAY path calls `Delay_process` / `Delay_processPingPong`. Reuse this object. A dub throw while non-delay FX is selected must explicitly indicate unavailable or route to the *selected* effect; **never silently replace kit effect selection**.
- `Core/Src/Soundengine/DSPAudio/FX/Delay.c` / `Delay.h`: `Delay_setDelayTime`, `Delay_setFeedback`, `Delay_setFilter` already exist; delay changes use interpolation to reduce clicks. Feedback must be clamped to a safe value and no-delay-input tail checked.
- `Core/Inc/Soundengine/DSPAudio/FX/FxRamBuffer.h`: `FX_BUFFER_SIZE=0x7FFF`, storage `FX_BUFFER[32768]` of half floats. At measured nominal 44,140.625 samples/s, maximum rough memory span is **0.742 seconds mono, 0.371 seconds/channel ping-pong**. Review actual calculation and safety margins. **Do not enlarge buffer blindly** or promise unsupported subdivisions.
- `Core/Src/Soundengine/StepFX.c` / `StepFXRuntime.c`: the current effects scheduler emits *additional voice triggers* for repeat/stutter profiles. A send throw is **not** a new note trigger. Create a separate, bounded send-gate control path so existing repeat scheduler remains independently usable.
- `Core/Src/Soundengine/Sequencer/sequencer.c`: `seq_getBpm`, `seq_getExtSync`, `stepfx_live` and `stepfx_clock` integration. `seq_getBpm` returns sequencer tempo; verify external clock fallback, clock timeout and transport changes.
- `Core/Src/menu/JimbonesPerfUI.c`, `Core/Src/menu/menu.c`, `Core/Src/buttonHandler.c`, `Core/Src/ledHandler.c`: Step FX performance subpage exists but currently selects 0..15 with no repeated-press OFF logic. For the v1.8.04 interface, press once activates selected action, press again deactivates; **Step 1 is a normal effect button**, not a master OFF. Keep the CPU diagnostic screen's special step 1 reset separate. LED index 0 maps to `LED_STEP1`; investigate subsequent LED clearing and mode transitions.
- `Core/Src/Soundengine/AudioDiagnostics.c`: `RAIL` counts near-full-scale samples, *not confirmed clipping*. `Soundengine.c` DSP meter times render-block work, not total all-interrupt CPU work. Both must be described accurately.

## Hardware evidence supplied by tester on v1.8.03

- Boots on device. Idle after restart: DSP **62%**, PK **66%**, LATE **0**, RAIL **0**.
- Overlapping modulated toms with Step FX OFF: crunch at DSP ~**94%**.
- More severe experiment: PK **105%**, DSP **101%**, LATE **999** (display may cap), RAIL **240**.
- User reports current Metallic FM audibly underwhelming for CPU demand: **shelve / default off, not a release blocker to reinstate**.
- Step 1 sequencer LED does not illuminate when armed; steps 2–16 behave normally.
- Per-button one-press-ON, next-press-OFF not yet implemented.

## Small implementation jobs (independent PRs, in this order)

### JB-19A — Gate model, no DSP and no UI
Add optional `sendEnabled`, `sendMask` (six voices; kick protected default), per-voice `sendLevel` 0–127, gate duration in *frames* and click-free attack/release ramp. Define control vs audio-thread ownership, no malloc/no blocking/formatting in audio callback. Allow one active gate per voice with bounded transitions; do not make timing depend on UI render frequency. **Default: disabled**. Provide deterministic host unit tests for 0, 1, 3 and 4 simultaneously active gates; gate close must not reset processor tail. Avoid mutable parameter/timing races.

### JB-19B — Parallel mixer tap (no extra engine)
At the post-voice-render point in `mixer.c`, add a saturating-copy `sendVoiceBlock` to existing `fxBusData`; route original voice unchanged to physical outputs. Set send level 0 to bit-identical dry output and unchanged FX input. Preserve original exclusive FX routing; avoid double-feeding when OUT=FX (explicit rule/test). Keep FX bus's 16-bit interleaved stereo layout, pan, and saturation safe. Where hardware output differs from FX bus output, preserve physical jack assignment and explain limitations. Do not add any DSP oscillators.

### JB-20 — Clocked dub delay + tail lifecycle
Implement optional delay-sync in the same `Delay` / `FxBus` object using BPM-derived subdivision (e.g. 1/16, 1/8, 1/4, dotted eighth, triplet eighth) **only when actual buffer capacity allows**. Validate rates against mono vs ping-pong maximum; never overflow/mask invalid reads or silently alias to a different rhythmic period. Expose supported/unsupported clearly on UI, with graceful fallback to a documented valid subdivision. Use current tone and feedback controls; clamp feedback for no runaway. Send gate OFF only stops new audio entering FX; delay remains processing zero input until tail dies. Test at 60, 80, 100, 120, 140 and 180 BPM and external-clock loss. Do not allocate a second large delay line.

### JB-21 — Performance control and Step 1 LED
Dedicated Step FX subpage: 16 buttons each correctly toggles on/off on a second press, with consistent indicator LEDs including step 1. Keep existing manual roll/normal sequencer/CPU reset behaviour on their own pages. Add selection vs active bit state distinctly. Provide send amount, voice enable/exclude, gate length, delay division, feedback/tone at 16x2 display/four encoder controls without breaking kit/voice menus. Kick default excluded; user can explicitly opt in. UI shows "SEND OFF" when global effect isn't delay (or exposes a generic selected-FX route if deliberately implemented). For live hits, gate is a short window; dry signal is continuous. Make defaults/kit persistence explicit and version-safe; never alter legacy kit layout without migration tests.

### JB-22 — Verification + release gate
Host tests: (1) dry output identical when disabled; (2) one snare tapped once and send off later still echoes; (3) per-voice gated copy does not re-trigger a voice; (4) original exclusive output routing remains; (5) step 1..16 LEDs and toggle parity; (6) alternate mono/ping-pong supported time bounds; (7) four concurrent hits including long decays and FX feedback with current wavetable enabled; (8) no new LATE or RAIL events; (9) buffer overrun/overlap/race assertions; (10) ARM build, .elf size, SHA256 image and rollback. Target meaningful CPU headroom; do not sign off build if DSP reaches or exceeds deadline budget. Hardware confirmation required before flash recommendation.

## Detailed signal-flow requirement

```text
voice trigger -> ORIGINAL SYNTHESIZER (one render only)
                      |
                sampleData[voice] --+--> original MIX out selection --> hardware out (dry)
                                   |
                                   +--> Step FX optional per-voice level x short gate
                                             |
                                       fxBusData stereo accumulator
                                             |
                                   EXISTING GLOBAL FX (dub delay)
                                             |
                                    FX output selected by FX menu
                                             |
                                     hardware out (wet echoes)
```

**Critical:** The send must be an additive copy, not a replacement destination. `FX_BUFFER` holds the feedback state; do not zero it when send gate drops. If the FX bus routes to a different physical output, dry and wet may appear on separate jacks by design; document or offer an explicitly chosen shared-output route.

## Codex execution rules

- Source baseline must be selected by Integration after evaluating outstanding PRs; work only on feature branch(es), no automatic merge to `main` or known-good version.
- Read this file, `docs/JIMBONES_AGENT_BOARD.md`, `docs/JIMBONES_MULTI_AGENT_WORKFLOW.md`, actual source and official manual. Do **not** copy code from `brendanclarke/LXR02Open`: its bespoke non-commercial license is not automatically compatible with this repo's GPL-3.0-only terms.
- Start with JB-19A/B. Keep a PR for each small job; only integration combines after independent regression.
- GitHub Actions compiles/tests, **does not establish hardware safety**. The status of all items above is **research only** until actual code/tests exist.
