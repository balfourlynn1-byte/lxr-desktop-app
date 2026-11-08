# CODEX_EXECUTION_RULES — JIMBONES lean build protocol

**Read this after `docs/CODEX_1830_HANDOFF.md`.** This is a time/compute budget protocol, not evidence of hardware acceptance.

## Single-job contract and strict scope
- **Current job: JB-28A ONLY** — transplant Comb from PR #15 into current JB-26-derived branch, run checks, package v1.8.07 TEST ONLY, draft PR, stop. Do not implement JB-29, pitch changes or new engines in this session.
- **Known working device behaviour:** Step FX core works; dub delay/effect works. Preserve both. Metallic FM disabled; preserve original six voices, original kits and protected dry kick.
- **Explicitly unaccepted:** Comb in combined image, wavetable as user envisioned, skull/LCD hardware, five timing buttons, new voice selector. A successful compile is not a feature acceptance.

## Read budget
1. Read this file and `docs/CODEX_1830_HANDOFF.md` once; avoid rereading giant docs.
2. Inspect only changed paths from PR #15 and their matching current files: `Core/Src/Soundengine/DSPAudio/ResonantFilter.c`, `Core/Inc/Soundengine/DSPAudio/ResonantFilter.h`, `Core/Inc/menu/MenuText.h`, `Core/CMakeLists.txt`, `tests/run_host.sh`, new `CombFilter.c/.h` and `tests/run_jb12_comb.sh` / associated tests.
3. Expand search only for a named unresolved symbol, compiler error, failing assertion or storage/ISR dependency. Use targeted `rg` and `git diff -- <path>`; avoid full-repository dumps.
4. Reuse existing tested Comb patch and tests; do not generate a new DSP algorithm.

## Execution order (fail fast)
1. Record source base SHA and exact Comb donor SHA. Verify worktree clean; keep original good image intact.
2. Apply minimal patch; inspect changed-file list and diff to ensure no older LCD/dub code was overwritten.
3. Run focused Comb host test first. If red, fix only that failure before broader CI.
4. Run `sh tests/run_host.sh` (GCC + Clang as supported), then ARM compile; validate >=512 B free ITCM and updater checksum.
5. Package only if every gate passes. Record source SHA, image SHA256, file size, ITCM used/free, test commands/results and `HARDWARE_UNVERIFIED`.
6. Create one draft PR and a compact report. Stop. Do not claim boot, Comb sound, CPU headroom or LCD success without physical test.

## Error budget
- On the **first failure**, report the failing command, shortest relevant error excerpt, root-cause hypothesis and proposed smallest fix. Retry only when the fix directly addresses that failure.
- After **two failed attempts on the same failure**, stop and request review rather than exploring broadly or weakening gates.
- Never remove safety tests, reduce ITCM threshold, silently change existing presets, merge to main or flash the device.
- Keep commits focused; no unrelated refactors, cosmetics or new features.

## Definition of done
**Coding done:** focused and full host tests green, ARM green, >=512 B free ITCM, verified updater image, source and image SHAs, one draft PR and rollback notes.
**Feature accepted:** only after user confirms on real hardware: filter selectable and audible, bypass dry parity, six voices, 4-hit stress, DSP/PK/LATE/RAIL, Step FX/dub regression, LCD behaviour and kit save/load. These are distinct statuses.

## Next queued job: JB-29A, NOT NOW
See the approved four-knob voice-group UI in `docs/JIMBONES_JOB_TREE_V184.md` on branch `agents/wavetable-research`. First implement only group/mask storage and routing at existing FX send/trigger point; knob 1 eight groups, SHIFT+knob1 advanced individual masks, 16 independent slots, protected kick; no algorithm changes. RATE/DEPTH/CHAR and pitch redesign belong to JB-29B.
