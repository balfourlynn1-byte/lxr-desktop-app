# JIMBONES Codex checkpoint and timeout recovery protocol

**MANDATORY: Save progress continuously. Losing work to session timeouts is unacceptable.**

## At the start of every Codex session
1. Read `docs/JIMBONES_WORK_QUEUE.md`, `docs/JIMBONES_ATOMIC_TASKS.md`, `docs/CODEX_HANDOVER_2026-11-10.md`, and this file.
2. Inspect `git status --short`, current branch, `git log -5 --oneline`, latest CI and the latest checkpoint. Never assume the previous session completed its last operation.
3. Work on `agents/jb-wavetable-host-experiment` for isolated wavetable tasks; do not overwrite integration or main branches. Keep strict ARM hold: no ARM compile, firmware image, merge or hardware-ready claim without explicit user approval for a frozen candidate.

## During work: mandatory durable saves
1. Break work into small atomic changes with a concrete test and acceptance criterion. **Commit each coherent change immediately** (source, tests, docs), preferably every 10–15 minutes or sooner. Do not wait until the end of the session.
2. **Push every successful atomic commit to GitHub immediately**; a local commit alone is not a timeout-safe backup. Confirm remote commit SHA and branch, and inspect push errors. Never claim saved remotely if push failed.
3. Before starting any lengthy build/test, context-intensive analysis or risky refactor, push the current coherent checkpoint. Keep changes small enough that an interrupted task can be safely resumed.
4. After every commit, update `docs/JIMBONES_WORK_QUEUE.md` and `docs/JIMBONES_ATOMIC_TASKS.md` with PASS/FAIL/NOT RUN, exact evidence, blockers and next atomic step. Push these notes. Do not claim a test passed until a run actually confirms it.
5. Keep `docs/CODEX_LIVE_CHECKPOINT.md` current with UTC timestamp, branch, last confirmed remote SHA, files touched, completed vs incomplete work, exact failing test/run IDs, next command and recovery instructions. **Commit and push checkpoint updates frequently**. If time is short, save the checkpoint before attempting further work.
6. If there are uncommitted partial changes that cannot pass tests, save a clearly named **WIP commit on the experimental branch**, describe what is incomplete, and push it rather than losing work. Never present WIP as validated. Avoid destructive `reset --hard`, `clean -fd`, force pushes or overwriting another agent's changes.
7. When 10 minutes or less remain in the session, **stop new implementation**, commit/push any recoverable work, update and push checkpoint, and report remote SHA and exact resume step.

## After timeout, restart or tool failure
1. Re-read the remote branch, work tree and live checkpoint; verify last pushed SHA against GitHub before continuing. Resolve divergences safely; do not blindly reapply changes or duplicate commits.
2. If push failed, report that progress is **not safely backed up** and retry; do not falsely mark complete.
3. Resume from the first unfinished atomic acceptance gate. Preserve failed test evidence and log the root cause.

## Immediate priorities
- Confirm C10 ROM-bank CI after fixing a test assertion that compared waveforms at phase zero (where samples can coincide).
- Register the ROM-bank `.c` file with the actual desktop firmware build; verify linkage.
- Run full native DrumVoice wavetable-ON PCM tests, scan/velocity/LFO and CPU overlap tests, and OFF-path PCM regression.
- Preserve original voices, No Clip behavior and kit ABI; no ARM build before explicit user approval.
