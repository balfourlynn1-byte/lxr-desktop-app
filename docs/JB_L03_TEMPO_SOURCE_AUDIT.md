# L03 — Tempo reset to 120 BPM: source audit (2026-11-09)

Status: PARTIAL SOURCE VERIFIED; persistence root cause not proven; no code change or test execution.

- `Core/Src/Soundengine/Sequencer/sequencer.c:57` initializes `seq_tempo = 120`; `seq_setBpm` changes runtime tempo around line 438.
- `Core/Src/menu/menu.c:767` initializes `activeParameters[PAR_BPM] = 120`; `menu.c:3728–3729` routes PAR_BPM changes to `frontPanel_sendData(SET_BPM,...)`.
- Two independent initial defaults to 120 are consistent with reported restart reset, **but do not prove** a missing save path. Need inspect globals/project/kit serializers and startup load ordering before assigning tempo to a kit. BPM may be global or project-scoped, and external MIDI clock (BPM=0) must remain compatible.

Next L03 gates: trace all PAR_BPM writers and save/load order, identify actual serialization owner, create deterministic old-kit round-trip and power-cycle simulation, preserve external clock and tempo changes on kit switches. Do not blindly replace 120 with last tempo or write SD on every encoder tick; save safety and corruption recovery apply.

No ARM compilation or hardware readiness claims.
