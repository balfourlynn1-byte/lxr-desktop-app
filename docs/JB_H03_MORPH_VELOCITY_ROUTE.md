# H03 — Upstream MIDI morph and velocity route (2026-11-09)

**Scope:** pinned source audit only, no source copied or DSP implementation. Upstream `brendanclarke/LXR02Open@c896bfd0efe86a4037418bf9f46f1fcc39255958`. Source evidence verified via GitHub fetch; executable tests **NOT RUN**; H03 is **PARTIAL** pending preset/serialization trace.

## Pinned evidence

- Upstream `Core/MIDI/MidiParser.c` blob `cc53c6b131faf1a9bf4097ef0c4c9c7ef340f`, lines **129–135**: mod-wheel morph helper computes `value << 1` (MIDI 0..127 to morph 0..254), writes `parameter_values[PAR_MORPH]`, calls `preset_morph`, notifies `modNode_originalValueChanged(PAR_MORPH)` and UI. A value of 127 does **not** reach 255; do not assume full-range endpoint or a dedicated per-voice exclusion flag.
- Same MIDI file lines **809–821**: six LFO amounts normalized as `msg.data2/127.f`, with three drum voices plus snare, cymbal and hat routed separately. Lines **995–1001**: six velocity modulation amounts use `CC2_VELO_MOD_AMT_1..6` normalized `msg.data2/127.f`.
- Upstream `Core/DSPAudio/modulationNode.c` blob `e3a811bae6dfcb333601b02540956944873b18ee`, lines **61**, **113–117**: six velocity nodes initialized with amount zero; lines **173–203**: reset restores original parameter values and reassign reapplies stored last velocity. Lines **209–247**: destination changes reset targets, restore previous value and snapshot new typed destination. Lines **253–304**: `lastVal` stored; uint8 destination uses current value, amount and incoming value; clamps low to zero, waveform special case clamps upper to available wave ID and uses per-block interpolation budget. This is **not** an independent per-voice morph mixer.
- Native comparison: `Core/Src/Soundengine/DSPAudio/modulationNode.c` blob `14840ec0d2db1334052616ae1ede8d51db736220`, six nodes and reset/reassign paths; `docs/JB_B05_VELOCITY_DESTINATION_MAP.md` documents default-OFF JIMBONES extension. Native MIDI `Core/Src/Soundengine/MIDI/MidiParser.c` blob `04f3d03ffcee0b20f6d58e26d7a91f34e4b89bca` has six velocity amount CCs, existing NRPN banks 96–101 and clock gating. No evidence yet that native morph uses the upstream mod-wheel helper.

## Safe implementation contract (proposed, not implemented)

1. Preserve legacy `PAR_MORPH` mapping and old kit format. For per-voice morph inclusion, use a separately versioned six-bit mask defaulting to all included; ensure excluded voices retain their A state and No Clip remains independent. Never write per-hit velocity-derived morph into saved base A/B state.
2. For new velocity-to-wave position, normalize/clamp 0..127, gate with per-voice enable and zero depth, clamp to the native periodic-wave set 0..3, clear transient state on kit load/disable/target switch; no shared mutable velocity state across voices. Keep original six voice render paths bit-identical when OFF.
3. Do not copy upstream noncommercial code into the public GPL branch. Do not reintroduce Metallic FM. Any new persistence fields require versioned extension and malformed-file fallback.

## Host-only test fixture specification (NOT RUN)

- Morph MIDI values 0, 1, 64, 127: assert helper mapping 0, 2, 128, 254 **if intentionally retaining upstream semantics**, and document the 255 endpoint decision. Exercise 6-bit inclusion masks 0x00/0x01/0x3f; excluded voice A PCM must match its OFF reference exactly.
- Six independent velocity nodes: 0, 1, 64, 127 with depth 0 and nonzero; verify zero-depth exact equality, clamping, original-value restoration, destination reassignment and retrigger; test 3–4 simultaneous voices and CPU.
- LFO and velocity target the same parameter: assert deterministic precedence and no stale `lastVal` after kit switch. Check invalid target pointer, out-of-range CC/NRPN, NaN and repeated block reset.
- Legacy kit, new kit, truncated extension, unknown extension version, globals and morph A/B/C load/save round trips. **Serialization location and actual native morph/LFO precedence remain unverified; block implementation until mapped.**

ARM compiler, firmware packaging, main merge and hardware claims remain prohibited.
