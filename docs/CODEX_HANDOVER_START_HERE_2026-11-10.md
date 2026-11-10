# JIMBONES — START HERE: Codex operational handover (2026-11-10, 16:47 GMT)

> **READ THIS FIRST.** This is the concise, current execution index. The four large tracking documents are append-only history, not an instruction to repeat every old task. Follow latest verified evidence and stop at first failure. This handover is **NOT approval to ARM-compile, package, flash or release firmware**.

## 0. Immutable safety / scope

- Repository: `balfourlynn1-byte/lxr-desktop-app`.
- **Writable isolated branch ONLY:** `agents/jb-wavetable-host-experiment`. **Read-only comparison branch:** `agents/jb-28-full-integration-v186` (observed SHA `f797ec786fc57d008afeaabad964906f35a43177`).
- **No** ARM cross-compile, `.img`, ARM workflow re-enable, main merge, flash, or hardware-ready claim unless user explicitly approves the **exact frozen candidate commit and feature manifest**. An approval of a test or this document is not that authorization.
- Preserve all six original drum voices, legacy original-kit serialization, OFF-path audio, 3 ms / 132-sample No Clip retrigger smoothing, approved bouncing skull, existing working Step FX, and SD recovery. Metallic FM remains shelved; do not reintroduce it or its UI.
- Work in **one atomic task per run**. Push and verify each change; update all four documents. If execution environment is unavailable, report **BLOCKED / NOT RUN**, never PASS.

## 1. Live snapshot / evidence hierarchy

At drafting, isolated branch head: `f08588b85351ef4ab01587c4dc99e94761051739` (read back before any edits). Integration head: `f797ec786fc57d008afeaabad964906f35a43177`. **This document and subsequent tracker updates advance the isolated HEAD; always capture actual `git rev-parse HEAD` when testing.**

- C03/C04 standalone host/kernel tests and selected source guards have previously passed GitHub Actions; **they do not prove a complete native DrumVoice build or audio identity**.
- Source-level reinit reset fix: `a3fa755a217c82816d157f96ce9e172994d7bc71`; subsequent source guard was added at `d5d1a7d344eb322d8549d02def180491cefc838e`. Re-fetch/source assertions passed; runtime engine reinit **NOT RUN**.
- Host-only entrypoint `tests/run_codespaces_host_checks.sh`, introduced `0d9caa730c37d1e439f5d8aac19d611d6f9b26f4`, branch guard `92385da595aef391ffc3bb7b3b1df7658c1d38bf`. Script inspected, **NOT EXECUTED**.
- Latest observed native Actions run `42720355331`: **FAIL** on older SHA `6d3ae26f86b868c88bc6d9794a6431e5ed272f47`. Old log showed missing `uint32_t`/`uint8_t`, STM32 `DWT`, `RNG_HandleTypeDef`, ARM DSP intrinsics, and implicit oscillator/filter declarations. Script subsequently added `-include stdint.h` and `-DUSE_HAL_DRIVER`; no new compiler result verifies these fixes.
- Source guard CI `42702335053`: **PASS** at old SHA; structural only. Native link, OFF-path bit-identical PCM, 3–4 simultaneous hits/No Clip DSP, live LFO/MIDI routing, SD save/recovery, Step 1 LED/FX toggle, BPM persistence: **NOT VERIFIED** on latest branch.
- GitHub connector can read/write source and Actions logs; it cannot start Codespaces or dispatch a fresh workflow in this environment. Local `git` checkout previously failed GitHub DNS. Pushes through GitHub connector have not generated a fresh Actions run. Do not assume CI started.

## 2. Exact immediate Codex sequence — NO ARM

Use an actual Linux x86_64 host with Git, Python 3, Bash, native GCC and Clang; install host tools only as needed (e.g. Debian/Ubuntu `sudo apt-get update && sudo apt-get install -y git python3 build-essential clang`). These are **host** tools, not `arm-none-eabi-gcc`. Prefer a fresh Codespace or GitHub-hosted Linux runner.

```bash
set -euo pipefail
git clone https://github.com/balfourlynn1-byte/lxr-desktop-app.git
cd lxr-desktop-app
git fetch origin agents/jb-wavetable-host-experiment agents/jb-28-full-integration-v186
git switch --track origin/agents/jb-wavetable-host-experiment
git status --short
git rev-parse HEAD
git rev-parse origin/agents/jb-28-full-integration-v186
gcc --version | head -1
clang --version | head -1
python3 --version
# SOURCE GUARDS: structural, NOT compiled audio
python3 tests/test_c06_drum_scan_source.py
python3 tests/test_c12_wave_off_source.py
# FIXTURE/PREFLIGHT: NOT engine compilation
bash tests/test_native_probe_preflight.sh
# REAL host C syntax: NOT linking or PCM verification
CC=gcc bash tests/run_native_drumvoice_probe.sh
CC=clang bash tests/run_native_drumvoice_probe.sh
```

If a named file is missing, **stop and inspect `tests/`**, do not invent a passing test. Alternatively `bash tests/run_codespaces_host_checks.sh` performs the guarded GCC subset on the isolated branch. Save full stdout/stderr, exit status, compiler versions and exact SHA. The probe uses `-fsyntax-only`, not an executable, DSP test, firmware build or release. If it fails, fix the **first genuine diagnostic only**, commit, rerun, record remaining errors.

**Known old diagnostic families:** CMSIS STM32 register dependencies (`DWT`), HAL RNG handle, ARM-only DSP intrinsics (`__QADD16`, `__QSUB16`), oscillator/filter declarations. Do not replace target firmware behavior with fake no-op implementations merely to turn CI green. For host harness, isolate hardware-dependent stubs explicitly, prove they cannot enter ARM builds, and clearly label what was not tested. No global silencing of warnings.

## 3. Ordered task queue and gates

| Rank | Task | Completion evidence / stop condition |
| --- | --- | --- |
| P0 | C05/C06/C12 source guards + host compiler | Exact SHA, executable command, exit 0, GCC/Clang logs; separate source PASS from real host syntax PASS. First failing diagnostic blocks downstream claims |
| P1 | Complete native host **link**, no target build | Host-only stubs limited to peripheral boundary, real DSP symbols linked; capture missing symbols and executable; no silent ARM dependencies |
| P2 | OFF-path **bit identity** | Same deterministic original kit/notes/seed and input, experimental wavetable OFF, compare every PCM sample to pinned integration reference; include FM path, Step FX, retriggers, long tails. **0 mismatches required** |
| P3 | Durable wave bank / pointer lifetime | Confirm four immutable 256-sample waveforms, normalization and lifetime; re-init clears opt-in; sanitizer + bounds test, no dangling pointers |
| P4 | Live LFO and MIDI velocity | Trace source to actual live voice calls; sweep endpoints, clamp, test velocity 0/1/64/127, no change with scan OFF |
| P5 | 3–4 overlapping hits and No Clip | Long tom tails, rapid retrigger, pad velocity, 132-sample smoothing, CPU headroom and clipping evidence; compare old approved audio |
| P6 | SD failure/restart + kit BPM | Full-card, interrupted writes, rollback to previous globals/project, kit tempo restore, cold start; do not claim success from host-only syntax |
| P7 | Step 1 LED and FX toggle | Armed Step 1 LED works, other LEDs unaffected; each Step FX button toggles ON/OFF as specified; preserve original sequencer |
| P8 | Remaining A–G / H01–H07 | Pick only after gates; one independent atomic task, commit, test and checkpoint |

**Stop on any regression** of six original voices, original kits, No Clip, approved skull, legacy OFF audio or SD data. Do not let a source guard or isolated wavetable kernel stand in for full-engine PCM verification.

## 4. Efficient workflow / reporting contract

1. Read this file and `docs/CODEX_EXECUTION_BRIEF_2026-11-10.md`. Use the four tracker files for task IDs and historical details; **latest dated entry wins** where older TODOs conflict. Avoid rereading/repeating tens of thousands of lines of append-only logs.
2. Fetch both branch SHAs and diff only files relevant to the **first unblocked** priority. Capture baseline and protect integration.
3. Make one minimal change on isolated branch. Run the narrowest real check available; then wider host checks if unblocked. Never fabricate a result when no runner exists.
4. Commit and push **before context/time limits**. Verify remote HEAD and re-fetch changed file. Record **source commit** separately from **documentation commits**; tracker updates advance HEAD, so do not confuse them.
5. Update `docs/JIMBONES_WORK_QUEUE.md`, `docs/JIMBONES_ATOMIC_TASKS.md`, `docs/CODEX_LIVE_CHECKPOINT.md`, `docs/CODEX_HANDOVER_2026-11-10.md` with **one short pointer** to this canonical guide plus task ID, exact SHA, command, PASS/FAIL/NOT RUN, next blocker. Do not append the same full essay to all four.
6. If the remote runner remains unavailable, say **BLOCKED** and hand off reproducible commands. Do not endlessly add test wrappers and call it native progress.

## 5. Freeze / build gate — template, NOT approval

Only after user approval of an **exact** frozen commit and feature list, prepare and ask to confirm:

```text
Candidate label: JIMBONES-<version>
Frozen branch: agents/jb-wavetable-host-experiment
Frozen commit SHA: <40-char SHA>
Integration baseline SHA: <40-char SHA>
Included feature IDs / exclusions: <list; Metallic FM excluded>
Voice and original-kit preservation evidence: <links>
OFF-path PCM SHA256 and mismatch count: <actual>
Native host compiler/link tests and logs: <URLs>
DSP 3–4-hit / No Clip logs: <URLs>
SD power-loss and kit BPM tests: <URLs>
Toolchain and build environment: <actual compiler/version/flags>
ARM build authorization: NOT GRANTED / explicit approval timestamp and exact SHA
Artifact filenames and SHA256: NONE until authorized
Rollback image + recovery plan: <verified known-good, checksum, steps>
User sign-off: <explicit message referring to exact frozen SHA>
```

**Post-approval-only toolchain/build planning:** use the repository's actual STM32F765 target configuration, pinned ARM GNU toolchain version and original build scripts; inspect `Makefile`/project settings first, document versions and commands **before** executing. Build from a clean, immutable tag or SHA, record image and binary checksums. Do not infer a safe flash procedure from a host compile. Preserve a verified known-good image and backups of kits/globals. First hardware session: confirm boot/UI/skull and all six original voices with scan OFF, verify MIDI pad/hi-hat/kick routing, No Clip retriggers and Step 1, then opt-in wavetable on one voice, expand to 3–4 hits while watching DSP/LATE/RAIL and listening for crunch; verify saves, BPM, power cycle and recovery. **Abort/rollback** on boot failure, audio regression, runaway DSP, corrupted SD or unexpected reset. Do not flash unverified candidate.

## 6. Known incomplete/deferred at handover

- Fresh remote native GCC/Clang syntax compile **BLOCKED/NOT RUN**; host link and real PCM identity **NOT RUN**.
- Runtime wavetable reinit behavior, live LFO/MIDI routing, overlap DSP, No Clip, SD cold-start failure, Step 1, BPM **NOT VERIFIED**.
- Exact release toolchain, firmware candidate SHA, binary/image checksums, final acceptance logs **UNSET**. No hardware-ready status.
- Metallic FM remains intentionally shelved. Remaining A–G/H01–H07 queued, not assumed completed.

**Deadline: 2026-11-10 18:30 Europe/London.** At deadline deliver evidence and blockers as-is, even if unfinished. Do not extend the schedule or invent completion.
