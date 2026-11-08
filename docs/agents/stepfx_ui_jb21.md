# JB-21 — Step FX button + parallel send controls (v1.8.04 experimental)

**Branch:** `agents/jb-21-stepfx-toggle-dub-ui-v184` (stacked on JB-19B #9).  
**Implementation:** code committed; host/ARM CI and hardware tests tracked independently. No flashable release is authorised.

## User-facing controls

- Navigate to **Performance → Performance 2 → Step FX** (third Performance page). The CPU monitor is on the next Performance page.
- **Buttons 1–16:** First press activates the effect; second press on that same button deactivates it. The next button selected replaces the currently active effect, because the existing trigger FX scheduler supports a single active profile. The selected button LED is lit only while the effect is armed, including **physical Step 1**. The encoder's effect selection knob only changes the *editing focus*, not the armed effect.
- **Button 1 (FX01): DUB SEND**. An ordinary pad/MIDI or sequencer hit, including snare and toms, opens a short parallel send gate; the dry voice continues on its original MIX output and the gated copy feeds the **already-selected** global effect. The hardware's FX selection must be set to **DELAY** to get dub echoes. The legacy FX bus processes just one global effect; FX OFF suppresses the parallel send to avoid a doubled dry voice.
- **Buttons 2–16:** Existing retrigger, stutter, probability, velocity, multibar and random trigger profiles retain their original effects. When off, pending scheduled effects are cancelled.
- **Encoder rotation:** Select the voice for mask/level editing (starting at snare V4); encoder press toggles inclusion of that voice. Hat open and closed share the sixth synthesis voice.
- **Step FX page knob 1:** Browse effect slot without changing the armed effect.
- **When FX01 is focused:** Knob 2 selects snippet duration (1/4, 1/8, 1/16, 1/32 note). Knob 3 adjusts the focused voice's parallel send level 0–127. Knob 4 enables/excludes that voice. **Kick defaults excluded**, and initial non-kick send level is 64.
- **When FX02–FX16 are focused:** Original knobs 2/3/4 adjust retrigger rate, probability and voice inclusion.
- **16×2 OLED:** `*` beside the selected FX number denotes armed focus. Bottom line shows snippet division, selected voice, inclusion and send level in DUB SEND view.
- CPU diagnostics page still uses Step 1 for meter reset and does not inadvertently arm any Step FX.

## Source, timing and safety

- `Core/Src/menu/JimbonesPerfUI.c`: focused vs armed slot; physical index 0 mapped to send, NOT master-off.
- `Core/Src/buttonHandler.c`, `Core/Src/ledHandler.c`: armed-only LEDs via `led_idx2StepLed(0) == LED_STEP1`. This is source-code logic, not actual hardware LED validation.
- `Core/Src/menu/menu.c`: Level/voice/snippet controls; encoder knob browse is passive.
- `Core/Src/Soundengine/MIDI/MidiVoiceControl.c`: one send gate per normal note-on, both live pad MIDI and sequencer. Open/closed hat uses shared voice 5. `StepFXRuntime` voice retriggers bypass this call so legacy stutters do not add new send commands.
- `Core/Src/Soundengine/StepFxSend.c`: gate duration at nominal 44,140 frames/s: (44140 × 60 × 4)/(BPM × division), saturating at two seconds. When BPM unavailable, snippet length falls back to 120 BPM only; **this does not establish external clock synchronisation for delay echoes**.
- `Core/Src/Soundengine/StepFxSendMix.c`: original dry voice buffer remains untouched; existing global FX processor receives a gated, stereo-panned, saturating copy; no additional delay engine. If legacy voice OUT=FX, suppress duplicate send. If FX_OFF, suppress duplicate passthrough.
- A dub echo continues after gate closure because `FxBus` feedback state is not reset by the gate; actual audible feedback tail and timing require hardware verification and JB-20.
- **CPU:** v1.8.03 had 62% idle and reached 101% DSP with LATE 999 in tests. This UI+send branch is NOT validated on hardware; ARM compilation alone cannot prove live performance is glitch-free.

## Verification jobs

1. `sh tests/run_stepfx_ui.sh` — all 16 toggles, physical LED source guards, passive browsing, send-level/tempo control, real production `StepFxSendMix` gating and untouched dry buffer.
2. `sh tests/run_host.sh` — JIMBONES host regression plus JB-19A/B tests.
3. GitHub Action `.github/workflows/jimbones-jb21-verify.yml` — host + ARM compilation, no binary distribution.
4. **Not yet done:** live hardware Step 1 LED and six-voice output test, four simultaneous overlapping pad hits, FX bus routing, actual DSP/PK/LATE/RAIL at high feedback. Requires sign-off before any firmware is offered.

**Next:** JB-20 tempo-sync the existing delay processor, with mono/pingpong buffer caps checked at the selected BPM (rough 0.742s mono, 0.371s per ping-pong channel) and a controlled feedback-tail test. No second delay engine.
