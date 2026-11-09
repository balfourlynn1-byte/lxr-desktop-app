# H03 — pinned upstream MIDI morph/velocity source trace (2026-11-09)

**Scope:** source-only research at `brendanclarke/LXR02Open@c896bfd0efe86a4037418bf9f46f1fcc39255958`. This is not evidence that JIMBONES has the upstream behaviour. No upstream code is copied here.

| Path / blob | Exact source lines | Observed behaviour |
|---|---|---|
| `Core/MIDI/MidiParser.c` / `cc53c6b131faf1a9bf4097ef0c4c9c7ef4ef340f` | 129–136 | Mod wheel morph value is left-shifted by one, stored in `parameter_values[PAR_MORPH]`, passed to `preset_morph`, and announces external parameter change |
| same | 995–1002 | Six CC2 velocity modulation amounts indexed by `CC2_VELO_MOD_AMT_1`, scaled `data2/127.f` |
| same | 794–820 | Six voice LFO frequency/amount handlers; amount scaled `data2/127.f` |
| `Core/DSPAudio/modulationNode.c` / `e3a811bae6dfcb333601b02540956944873b18ee` | 197–203, 209–249 | Velocity modulators reapply `lastVal`; changing destinations resets targets, restores previous original and captures new parameter value |
| same | 252–305 | `TYPE_UINT8` modulation scales current value by `amount * val`, floors negative to zero, optionally derives adjacent waveform IDs and fractional mix under per-block active budget; otherwise writes truncated integer waveform ID |
| `Core/Preset/presetManager.c` / `5b277adf7321a32d678aea5f3c7b3f851ffe7445` | 490–500 | Morph intentionally excludes velocity destination slots and LFO voice/target assignments |
| same | 511–555 | Morph requests are generation-tracked; `preset_morphTick` sends one parameter per tick and restarts when a newer request arrives |

## Compatibility and hazards

1. **No direct claim of velocity→wavetable in JIMBONES.** Upstream modulation is destination-driven and requires a valid waveform parameter target and an active interpolation slot. Existing JIMBONES velocity scan work on an isolated branch is not yet linked to native MIDI note-on or native oscillator dispatch.
2. **Range hazard:** upstream mod-wheel mapping `value << 1` maps 127 to 254, not 255. Retain exact mapping only if compatibility is intended; test endpoints.
3. **State hazard:** `modNode_updateValue` reads the current destination and can accumulate if the caller does not reset original values before each modulation pass. Verify zero-amount bypass, successive note-ons and LFO/velocity order before porting behaviour.
4. **Morph persistence hazard:** destination assignments are deliberately skipped during morph, so target routing must be independently stored/recovered. JIMBONES H03 SD extension load is currently incomplete; do not assert round-trip persistence.
5. **CPU:** upstream wave interpolation has an active-slot budget; never enable an unbounded per-voice second renderer without host overlap/CPU evidence.

## Exact host-only acceptance fixtures for follow-up H03 implementation

- `CC1=0,64,127` must map to `0,128,254` for strict upstream compatibility; newer design may choose 255 but must document migration.
- Velocity `0,20,50,80,110,127`: verify bounded monotonic scan for positive depth, inverted response for negative depth, and bit-identical audio at zero depth.
- LFO amount `0,64,127`: zero amount must not change native legacy sample sequence; test waveform scan position bounds and per-block slot limits with 4 overlapping hits.
- Rapid morph `0→127→0`: verify newest generation completes; targets/destinations remain unchanged; legacy and truncated preset records load without alteration.
- Run focused GCC and Clang host fixtures, ASan/UBSan where possible; only after exact native integration run audio/CPU comparisons. No ARM build or image.

**H03 status: PARTIAL source mapping**; the broader task also asks for complete CC/NRPN routing and actual executable fixtures. This document does not claim those complete.
