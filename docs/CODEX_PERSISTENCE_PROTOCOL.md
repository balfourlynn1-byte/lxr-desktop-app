# Codex persistence — minimal mandatory rules

**Remote GitHub push = saved.** A local edit, local commit, final chat reply or CI log alone is not durable.

1. **Start:** inspect current SHA and existing checkpoint; write the next exact action and push (skip empty commits).
2. **Research mapped:** record only paths, symbols, decisions and blockers needed to resume. Push before editing code.
3. **Meaningful code change:** commit and push small WIP/ready patch with honest test state.
4. **Before full/expensive builds:** push source + checkpoint. **After any failed or passed gate:** push short result and next command.
5. **End or blocked:** push final checkpoint and update `docs/JIMBONES_WORK_QUEUE.md` on same branch. If push fails twice, explicitly report `NOT PERSISTED` and stop rather than assuming recovery.

Per-job checkpoint: `docs/CODEX_CHECKPOINT_JB28A.md`. Append concise milestone records: UTC time, actual pushed SHA (if known), stage, changed paths, test result, blocker, next exact command. Avoid long logs and repeated explanations. Never force-push another agent's work. Never mark device acceptance based on CI. The master tree on `agents/wavetable-research` is a **different branch**, not automatically updated. Future session reads latest remote checkpoint and `CODEX_START_HERE.md` first.

**Failure budget:** two targeted attempts for one error; checkpoint and stop. **Allocation safety:** save at deterministic milestones; do not wait for a low-allocation warning.
