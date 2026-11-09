# B06c — native MIDI/clock compatibility map (2026-11-09)

**Status: source evidence recorded; host tests NOT RUN; B06 remains PARTIAL.** Working source: `agents/jb-28-full-integration-v186`. No upstream code copied and no ARM work.

## Exact native evidence
- `Core/Src/Soundengine/MIDI/MidiParser.c` Git blob `04f3d03ffcee0b20f6d58e26d7a91f34e4b89bca`, lines 1497–1555: `midiParser_noteOn(voice,note,vel,do_rec)` gates muted tracks and optional exact note matching; in normal play calls `voiceControl_noteOn(voice,note,vel)` then `stepfx_live(voice,note,vel)`. On SEQ_PAGE it edits a step instead of sounding a note. With recording enabled it records and MIDI-echoes. New velocity modulation must not intercept these semantics.
- Same file lines 1858–1891: global channel 7 routes note-on to the front-selected track, then matching per-voice channels 0–6 are scanned, excluding the already-routed front voice. Only NOTE_ON with nonzero velocity triggers. NOTE_OFF and zero-velocity NOTE_ON are explicitly ignored (commented note-off calls). This is an existing behaviour to preserve/test, not a newly approved note-off feature.
- Same file lines 1766–1789: external MIDI CLOCK calls `seq_sync(); stepfx_clock();` only when MIDI_FILTER_SYSTEM and `seq_getExtSync()` both hold; START resets song position and starts; CONTINUE starts without reset; STOP stops and invokes `stepfx_stop()`. New scan or Step FX timing must respect this gate and reset policy.
- Same file lines 1171–1178: six `CC2_VELO_MOD_AMT_*` destinations update `velocityModulators[0..5].amount`; `Core/Src/Soundengine/DSPAudio/modulationNode.c` blob `14840ec0d2db1334052616ae1ede8d51db736220`, lines 90–113, resets and reassigns modulation targets. Preserve six-slot mapping and reset semantics.
- Same MIDI file lines 39–52: custom JIMBONES NRPN bank 96–101 selects voice, CC98 parameter index, CC6 bounded value. Access is restricted to global/voice channels (special channel mapping for voice 5), and valid parameters use `jb_limit`. New controls must not silently renumber `JB_PARAMS` or hijack NRPN banks.

## Deterministic host-only acceptance fixtures to implement/execute
1. Global-channel note-on triggers selected voice exactly once even if selected voice shares a dedicated channel; dedicated-channel note-on targets only mapped voices; 0-velocity and note-off do not trigger.
2. With track muted, mismatched configured MIDI note, or SEQ_PAGE active, no unintended audio trigger or velocity-mod state mutation; SEQ_PAGE still edits selected step.
3. Feed MIDI CLOCK/START/CONTINUE/STOP with external sync OFF, filter OFF, then both ON. Assert `stepfx_clock` counts and `stepfx_stop` invocation exactly follow source gates, and CONTINUE does not reset position.
4. NRPN banks 96 and 101, invalid bank, out-of-range parameter, wrong channel and over-limit CC6; verify six-voice isolation and clamping, legacy parameter IDs unchanged.
5. Velocity 1/64/127, repeated same-note retriggers and two simultaneous mapped channels; verify default-OFF audio identity and No Clip 132-sample fade using existing host fixtures.

**Execution:** these fixtures are specified, not implemented or run in this audit. Requires complete host checkout and stubs for `MidiParser.c` dependencies. GCC/Clang PASS and actual DSP/RAIL cannot be inferred. Upstream MIDI/clock API still needs separate pinned comparison; screen/serializer mapping remains open.
