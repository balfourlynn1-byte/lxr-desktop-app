# JIMBONES v1.8.04 EXPERIMENTAL — Codex low-context handover

**As of 2026-11-08:** Research and work assignment, **not a compiled firmware**, and **not a running multi-agent system**. This is designed to save Codex context and tokens by handing it small independent implementation tasks with source locations and verification gates.

## What the user actually wants

Keep all six Erica Synths LXR-02 voices and existing sounds, optimise for expressive live pad drumming (normally 3–4 overlapping hits and long decays), preserve the protected kick, and build **parallel Step FX sends**: momentarily copy sound to the global processor for dub-style echo throws while physical dry output continues unchanged. In addition prioritise wavetable/waveshaping, proper 16-button Step FX toggle + LED, comb-filter feasibility and existing send/delay improvements. **Shelve current Metallic FM** (insufficient sound change for CPU load); bypass/default off is required. Do not conflate an ARM compile with a tested or safe device.

## Current source and external reference

- Repository: https://github.com/balfourlynn1-byte/lxr-desktop-app
- Research/workboard branch: `agents/wavetable-research` (not the default `main`).
- Agent assignment board: `docs/JIMBONES_AGENT_BOARD.md`.
- Primary implementation audit: `docs/agents/dub_step_send_v184.md`.
- Original JIMBONES wavetable research: `docs/agents/wavetable.md`.
- Official LXR-02 desktop manual: https://www.ericasynths.lv/media/LXR_manual.pdf (MIX p15, FX p26, delay p27, physical routing p34).
- Upstream research ONLY: https://github.com/brendanclarke/LXR02Open (non-commercial bespoke license; **not** a drop-in GPL-3.0-only code donor). User previously supplied a YouTube demo; locate exact video in earlier thread if needed rather than inventing its URL.
- Existing six-voice tables: `Core/Src/Soundengine/DSPAudio/wavetable.c`, oscillators `Oscillator.c`, and JIMBONES interpolation `Jimbones.c`, `jb_setWaveInterpolation`; avoid redundant table banks.

## Verified vs unverified evidence

**Actual device v1.8.03:** boots; idle DSP 62%, PK 66, LATE 0, RAIL 0. During overlapping modulated toms, crunch at ~94% DSP even when Step FX OFF; another high-load case: DSP 101%, PK 105%, LATE 999, RAIL 240. `RAIL` counts near-full-scale samples, not definitively clipped frames; `LATE` counts render-budget overruns. Existing DSP % measures audio render work, not full interrupt/system load. Root cause not yet established. Step 1 LED does not light when armed; 2–16 normal. 16 Step FX buttons currently select a profile, not repeat-press toggles. Do **not** deliver this as stable.

## Small one-PR jobs, ordered for minimal Codex context

### Job A — JB-19A: tiny gated-send state machine (DSP, 1 PR)
**Read only:** `Core/Inc/Soundengine/StepFX.h`, `Core/Src/Soundengine/StepFX.c`, `Core/Src/Soundengine/StepFXRuntime.c`, `docs/agents/dub_step_send_v184.md`.  
**Implement:** lightweight control/audio handshake and bounded per-voice send level/gate with 2–5 ms de-click ramp, kick excluded by default and global bypass OFF. No new oscillator, no note retrigger, no buffer copy yet. New independent host tests near `tests/test_stepfx.c`.  
**Done:** zero-state bit-parity, predictable gate time, deterministic multiple voices and no allocation/lock in render ISR.

### Job B — JB-19B: parallel dry+FX audio copy (DSP, 1 PR)
**Read only:** `Core/Src/Soundengine/DSPAudio/mixer.c`, `Core/Src/Soundengine/FxBus.c`, header, Job A API, and audit.  
**Implement:** mix *copy* of already-rendered voice into `fxBusData` only while send gate active, alongside legacy `mixer_addDataToOutput` dry physical output. Exactly one global existing FX engine. Keep interleaved stereo sum safe, avoid double-feed if legacy OUT=FX.  
**Done:** dry unchanged with send bypass, no extra voice renders, wet tail after send off, routing/panning/saturation regression tests, 3–4-voice CPU profile.

### Job C — JB-21: Step FX button on/off + step 1 LED (Embedded, 1 PR)
**Read only:** `Core/Src/buttonHandler.c` near lines 425–440 and 828–847, `Core/Src/menu/JimbonesPerfUI.c`, `Core/Src/menu/menu.c` Step FX display, `Core/Src/ledHandler.c` `led_idx2StepLed`, `tests/test_perfui.c`.  
**Implement:** selected slot distinct from active state; press button once ON, again OFF, all 1..16. LED1 parity, consistent UI refresh, zero loss of manual rolls or CPU diagnostic step 1 RESET. This is a **required bug fix**.  
**Done:** tests for all 16 ON/OFF, LED index 0=LED_STEP1, mode transitions and no user-visible selected-vs-active discrepancy.

### Job D — JB-20: tempo clock and dub echo-tail integration (DSP, 1 PR)
**Read only:** `Core/Src/Soundengine/DSPAudio/FX/Delay.c`, `Delay.h`, `FxRamBuffer.h`, `FxBus.c`, `seq_getBpm`/external sync; audit.  
**Implement:** optional clock-divided time for existing FX_DELAY with safe bounds, feedback cap and no tail termination when send gate goes low. Mono buffer 32768 samples ~0.742 s at 44.14kHz, ping-pong approximately half ~0.371 s/channel; **do not exceed buffer**. No second large delay. Consider 1/16, 1/8, 1/4, dotted/triplet eighth but expose any unsupported subdivision at given BPM/mode, no silent mis-sync.  
**Done:** 60–180 BPM, mono/ping-pong and clock-disconnect tests; audible delayed echoes post-gate; zero unstable feedback.

### Job E — JB-09: wavetable optimisation (DSP + Research, 1 PR)
**Read only:** `Core/Src/Soundengine/DSPAudio/Jimbones.c` `wave()` and `jb_setWaveInterpolation`, `Oscillator.c`, `wavetable.c`, `docs/agents/wavetable.md`.  
**Implement:** profile and optimise existing interpolation/scan using current bandlimited ROM (not third-party code), feature-flag opt-in, benchmark with 4 overlaps and legacy parity with OFF.  
**Done:** difference audible or measurable enough for CPU, no aliased glitches, no memory explosion, host+ARM tests.

### Job F — JB-12: comb filter prototype (Research -> DSP, 1 PR)
**Read only:** `Core/Src/Soundengine/DSPAudio/ResonantFilter.c`, `Core/Inc/Soundengine/DSPAudio/ResonantFilter.h`, `Core/Src/Soundengine/FxBus.c`, memory map and audio diagnostics.  
**Implement first:** technical audit comparing short comb delay inside existing per-voice filter vs new global FX. Only code a bounded opt-in primitive once CPU/memory tradeoff is measured. Feedback <1 and stable DC/bandwidth behaviour; kick off default.  
**Done:** sweep/noise impulse response, no runaway, memory/CPU budget, six-voice bypass parity. **Not yet fully implementation-ready** until the audited design decision is recorded.

### Job G — JB-22: CI + verification gate (Verification, 1 PR)
**Read only:** `tests/run_host.sh`, `tests/run_stepfx.sh`, `tests/run_combined_stepfx.sh`, `tests/run_audio_diagnostics.sh`, `tests/validate_image.py`, `.github/workflows/jimbones-v1.8.03-arm.yml`.  
**Implement:** v1.8.04 branch-specific host + cross-compile workflow; `v1.8.03` gate currently targets older branches and checks old version strings and must **not** be relabelled as v1.8.04 without updating checks. Tests must include dry bit-parity, multi-voice 4-hit worst-case, limited send/tail, LED parity, feedback limits, feature bypass, memory map. Every candidate artefact labelled ARM-built/HARDWARE-UNVERIFIED, checksum produced, never overwritten known-good v1.8.02.  
**Done:** test evidence + exact SHA/checksum and independent integration review. LXR hardware validation is a separate final step.

## Real-time and safety constraints

- Never unconditionally add waveshaping+comb+echo sends+FM and then call it ready without four-voice performance measurements. Default OFF for new DSP and no extra audio-thread allocation.
- Preserve original six voices / kits / output destinations and old firmware rollback.
- `FxBus` handles **one** global effect per kit; optional Step FX send must not hijack the FX type.
- At end of every little PR produce: changed paths, code diff summary, host tests/ARM status, CPU delta, memory delta, risk and next job. Resist fetching whole repo again.
- CI is delegated to GitHub Actions; the "agents" in `docs/JIMBONES_AGENT_BOARD.md` are **roles, not live workers**. Separate Codex jobs require actually starting independent workers (or doing them serially).

## Codex startup prompt (copy only when resuming)

> In `balfourlynn1-byte/lxr-desktop-app`, check out the `agents/wavetable-research` workboard branch and read only `docs/CODEX_V184_HANDOVER.md` and `docs/agents/dub_step_send_v184.md`. Work on one isolated child branch for **Job A (JB-19A)**. Do not alter main, do not merge, do not flash the LXR-02. Implement the small gated-send state machine, its host tests, and report exact commit, test outcomes and CPU/memory changes. Keep all default audio paths identical. Stop at a reviewable PR. 

## Order for integration

A (send gate) -> B (parallel mixer) -> D (sync/tail) + C (UI independent) -> G (regression and ARM) -> E (wavetable optimised) -> F (comb after resource audit) -> independent hardware test.

**Nothing in this document authorises automatic implementation, future work promises, or merging code.**
