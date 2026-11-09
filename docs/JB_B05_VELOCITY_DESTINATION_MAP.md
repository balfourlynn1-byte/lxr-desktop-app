# B05 — velocity destination map and default-OFF proposal (2026-11-09)

**Status: DONE, source-only. No host or hardware test executed.** Upstream pinned to `brendanclarke/LXR02Open@c896bfd0efe86a4037418bf9f46f1fcc39255958`. Research only; no upstream source copied.

## Source evidence
- Upstream `Core/DSPAudio/modulationNode.c` blob `e3a811bae6dfcb333601b02540956944873b18ee`: six `velocityModulators` at line 61; `modNode_init` sets amount 0 at 113–117; `modNode_resetTargets` restores original values at 173–192; `modNode_reassignVeloMod` reapplies stored lastVal at 197–203; `modNode_setDestination` restores prior target and captures new original at 209–247; `modNode_updateValue` computes fractional waveform target only when enabled and within per-block budget at 253–305. This is generic velocity routing, **not** proof of any preset velocity-to-wave default.
- Native `Core/Src/Soundengine/DSPAudio/modulationNode.c` blob `14840ec0d2db1334052616ae1ede8d51db736220`: six velocity nodes at line 27, default amount 0 at 33, original-value reset at 90–104, reapply at 108–114, destination assignment at 120–160, native CC-normalized modulation at 164–230. Native CC path lacks upstream fractional waveform-target special case.
- JIMBONES `Core/Inc/Soundengine/DSPAudio/Jimbones.h` blob `a1910ddce801eac70b3ecfdf5a20f41b3ff14cda`: per-voice controls include `JB_POSITION`, `JB_MOVEMENT`, `JB_SHAPE`, `JB_DRIVE`, `JB_VELOCITY`, `JB_BLEND`, `JB_VBLEND`; Metallic remains `#define JB_METALLIC_ENABLED 0`.
- JIMBONES `Core/Src/Soundengine/DSPAudio/Jimbones.c` blob `149807fd1574f6f130f6272949920908c4f5e536`: `jb_trigger` stores normalized hit velocity at 103–113, `JB_VELOCITY` scales envelope at 135, `JB_VBLEND` affects blend at 141–144, `JB_POSITION` is mapped to 0..3 at 188. Existing controls do **not** establish velocity-to-position behavior.

## Proposed first destinations (not implemented)
1. **Wave position (first):** add separately gated signed velocity depth per voice; clamp normalized velocity to [0,1], position to [0,3] for four periodic wave IDs 0..3. Choose neutral velocity 64/127 for bipolar depth; compute at note-on with finite checks. Default depth **zero** and explicit enable **OFF**, so stock position and output remain unchanged. Avoid repurposing `JB_VELOCITY` (already envelope) or `JB_VBLEND` (already blend).
2. **Shape (second):** optional velocity-to-shape with signed depth, clamped to existing 0..127 shape control, no mutation of stored base setting. Default OFF/zero. Prefer over drive for first test because drive can worsen peak/crunch; measure sound usefulness before shipping.
3. **Drive (deferred):** potentially compelling but headroom-sensitive; add only after peak/full-scale and overlap regressions.

## Integration/persistence contract
- Keep existing six-voice parameter IDs and serialized kit bytes unchanged. New enable/depth values need a versioned, length-checked extension or separate optional sidecar; **do not** append to old packed kits without migration. Missing extension => OFF/zero. On invalid/malformed extension => safe OFF and preserve old kit. Do not change existing JB_VELOCITY or JB_VBLEND semantics.
- Compute modulation as transient effective state; never write velocity-derived values into saved base parameters. Recompute on each note-on and reset on kit load, target reassignment, and disable; avoid stale waveInterp generation.
- Preserve No Clip 132-sample correction, six original voice OFF path, approved bouncing skull and Metallic-disabled setting.

## Exact host-only acceptance plan (NOT RUN)
- Velocity 0/64/127 × depth -max/0/+max × base wave positions 0,1.5,3 and shape 0,64,127; assert finite/clamped, monotonic endpoints where appropriate and zero-depth exact PCM equality.
- Six independent voices, rapidly alternating Tom 1/2 and 3–4 overlapping long decays; ensure one voice's velocity cannot alter another's state. LFO destination switch mid-ramp and kit reload must clear transient interpolation state.
- Legacy kit round trip, missing/truncated extension, unknown version and OFF bypass; run GCC and Clang host sanitizers plus deterministic WAV/hash comparison on pinned integrated source. Hardware DSP/RAIL remains unknown.

**Decision:** recommend wave position first, shape second, drive deferred; no implementation or sonic validation implied. Next B06 (memory/kit/MIDI/display compatibility) or A03 host execution when environment permits.
