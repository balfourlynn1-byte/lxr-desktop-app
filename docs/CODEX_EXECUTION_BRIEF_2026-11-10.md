# JIMBONES — Codex execution brief for 10 November 2026

**Status:** handover preparation only. **NO ARM BUILD / NO .img / NO MERGE / NO FLASHABLE RELEASE** without explicit user approval of a feature-frozen candidate. Do not interpret this document as approval.

## Repository / source of truth
- Repo: `balfourlynn1-byte/lxr-desktop-app`
- Integration: `agents/jb-28-full-integration-v186`
- Isolated wavetable experiment: `agents/jb-wavetable-host-experiment`
- Read `docs/JIMBONES_ATOMIC_TASKS.md`, `docs/JIMBONES_WORK_QUEUE.md`, `docs/CODEX_CHECKPOINT_JB28A.md`, and `docs/CODEX_HANDOVER_2026-11-10.md`. Some earlier entries are historical; latest dated entries override older pending status.
- Preserve six original voices, original-kit serialization and OFF-path audio, 132-sample No Clip retrigger fade, v1.8.06 skull, existing Step FX, and SD save/recovery. Metallic FM remains shelved/OFF.

## Verified evidence, not assumptions
- C02 standalone Python wavetable fixture: GitHub Actions success, run 42680038099. **Not native engine audio**.
- C04 velocity scan: GitHub Actions success (GCC/Clang), run 42682444356. **Not physical MIDI routing**.
- C03 isolated C scan kernel exists; C05 native `Oscillator.c` renderer exists; C06 opt-in `DrumVoice.c` mix-mode route and note-on velocity capture exist. **Neither C05 nor C06 has a verified full native compile or audio comparison**.
- C05/C06 source guard workflow `.github/workflows/jb-c06-drum-scan-guards.yml` was committed but no run was visible when last checked. Recheck Actions; a passing source guard is not an engine test.

## Critical blockers, in strict order
1. **Run existing source guards and C03/C04 host tests.** Record exact commands, SHA, logs, compiler versions. Fix any failures.
2. **Native compilation / link:** compile `Oscillator.c` and `DrumVoice.c` with real headers and native configuration in a host-safe harness, or explain precisely which STM32 dependencies block it. Test no warnings, sanitizer errors or missing symbols. Do not ARM-compile until approval.
3. **OFF-path bit identity:** with wavetable disabled, compare reference integration audio, note triggers and original kit load/save byte-for-byte. Include FM mode, No Clip, long decays, repeated hits, Step FX.
4. **Waveform table lifetime:** current `jbScanTables` is a single shared borrowed pointer. Introduce durable owned/ROM four-by-256 waveform data; avoid dangling pointers, dynamic allocation and accidental sharing. Define actual audible wave shapes, normalization and aliasing strategy.
5. **Live LFO:** `jb_drumScanSetLfoValue` exists but no production caller. Identify actual LFO signal and update it at a safe cadence. Avoid discontinuities at block boundaries; add bounded smooth modulation tests.
6. **Enable path:** `jb_drumScanConfigure` has no production caller/UI or persistence. Make explicit opt-in per voice; retain existing waveform IDs and saved-kit ABI. Decide whether persistence requires a versioned extension; never silently alter kit bytes.
7. **Velocity:** `Drum_trigger` captures raw note-on `vol`; test 20/50/80/110/127 on actual render path, with retrigger overlap and soft/hard timbre change. Ensure 0-depth bypass.
8. **FM and voice coverage:** C06 only routes main oscillator when `mixOscs` true. In FM-only mode it retains factory oscillator. Decide supported scope explicitly; do not advertise six-voice scanning while only three main drums have opt-in routing.
9. **CPU/audio:** render 3–4 simultaneous hits and long tom tails; verify DSP stays below deadline with margin, no growing LATE/RAIL, no clipping or clicks, and full bypass remains bit-identical. Host timing is indicative, not STM32 DSP proof.
10. **UI, SD and recovery:** only after audio passes. Test Step 1 LED/Step FX toggles, BPM per-kit persistence (not default 120), and SD disconnect/cold-start rollback before claiming hardware ready.

## Candidate freeze and approval gate
- Codex should prepare a **candidate manifest** listing exact commit SHA, features included/excluded, tests and failures, CPU/memory estimate, file hashes, rollback steps and hardware smoke-test plan.
- Present this to user and **request explicit approval to lift ARM hold for this exact frozen candidate**. Until then: no ARM compile, .img generation, merge to main or 'ready to flash' claim.
- Once explicitly approved, perform toolchain/version-pinned ARM build, binary format/checksum validation, compare against known bootable images, preserve rollback image and label as **experimental hardware test**, not stable firmware.
- Stop immediately on failed legacy audio identity, corruption/recovery failure, unexpected UI regressions, over-budget DSP or missing compile evidence. Report blockers instead of silently downgrading tests.

## First Codex instruction
'Open the isolated wavetable branch and all four tracking docs. Work one atomic task at a time. First execute C05/C06 source guards and real native host compile; report exact command/output and commit. Do not build ARM or flash image. Preserve OFF-path identity and existing six voices. Update job tree/checkpoint/handover with evidence at every step.'
