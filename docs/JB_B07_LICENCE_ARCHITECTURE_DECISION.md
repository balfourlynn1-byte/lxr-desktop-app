# B07 — Upstream licensing and architecture decision (2026-11-09)

**Decision: independent Erica-native implementation only for the public GPL-3.0-only JIMBONES branch.** This is an engineering/repository policy decision, not legal advice or a licence clearance for copied code. **B07 source-research gate DONE; implementation and tests NOT RUN.**

## Immutable evidence
- Upstream: `brendanclarke/LXR02Open` commit `c896bfd0efe86a4037418bf9f46f1fcc39255958`, `LICENSE` blob `635ea7ee4e54922c03fad5795ba7c734048857b9` (verified by exact-commit fetch 2026-11-09).
- Upstream LICENSE permits redistribution/use subject to **no sale and no commercial activity**, requires complete modified source (with system component exception), reproduction of copyright, conditions and disclaimer. Those additional field-of-use restrictions cannot simply be incorporated into the public GPL-only JIMBONES codebase. Personal home use does **not** waive the public redistribution issue. An independently implemented algorithm does not require copying upstream protected expression; contributors must still check provenance and third-party assets.
- Upstream renderer `Core/DSPAudio/Oscillator.c` blob `5c1bf056b8016ff1a43a87f7f72ae6bf59bb4467`; modulation `Core/DSPAudio/modulationNode.c` blob `e3a811bae6dfcb333601b02540956944873b18ee`. Existing research is in `docs/JB_WAVETABLE_UPSTREAM_PORT_PLAN.md`, B02–B04.
- Native `Core/Inc/Soundengine/DSPAudio/Oscillator.h` blob `9b1be9f39646e636523b530fe04c70f38f3a852b`, `Core/Src/Soundengine/DSPAudio/modulationNode.c` blob `14840ec0d2db1334052616ae1ede8d51db736220`; incompatible OscInfo ABI, waveform IDs (native PWM ID 5 versus upstream crash ID 5), sample memory and modulation dispatch. See B04/B06.

## Selected route and explicit exclusions
1. Retain native six voice engines, existing voice/preset IDs, No Clip 132-sample smoother and approved v1.8.06 skull; keep experimental Metallic FM OFF. No replacement of native OscInfo or voice engines.
2. Independently author small native extension with OFF/zero-depth bit-exact bypass. Start with bounded periodic waveform IDs 0..3, phase-stable crossfade and optional velocity-to-position; keep noise/PWM/sample/crash boundaries outside the first prototype. Use separate tests and test vectors written from native source/functional specifications, **not** pasted upstream functions or buffers.
3. Require independent review of new source diffs for provenance, third-party samples/assets and public licence compatibility. Record every author and file-level licence; block integration if uncertain.
4. Alternative **separate private noncommercial upstream-derived fork** is only a research option: it would have to comply with upstream terms, distribution obligations and all other component licences. It must never be cherry-picked into the public GPL branch without explicit compatible relicensing/permission and review. No such fork is authorised or created by this decision.

## Codex acceptance gate
- Before C01/C03: record a native-only implementation specification and provenance manifest, assert no upstream source or asset copy, and create a dedicated host-only experiment branch from pinned integration SHA.
- Test legacy OFF path bit identity, malformed waveform IDs, phase wrap, velocity 0/64/127, six-voice isolation, No Clip split blocks, preset rollback and worst-case 3–4 simultaneous hits. Run focused and full GCC/Clang with published logs. No unexecuted PASS claims.
- Before any ARM build: feature freeze, user explicit approval, exact SHA and rollback plan. No ARM compile, workflow re-enable, `.img`, or main merge in this phase.

**Residual uncertainty:** legal interpretation and provenance of all upstream components require qualified review if any actual code reuse is proposed; hardware DSP/ITCM and audible quality are unmeasured. B06 remains PARTIAL. This decision does not close C/F/G implementation tasks.
