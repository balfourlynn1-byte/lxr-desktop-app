# Codex persistence protocol — survive allocation exhaustion

**MANDATORY for every JIMBONES Codex job.** Saving only a final chat summary is insufficient. GitHub remote is the source of truth. A local file or unpushed commit can be lost when the session expires.

## Checkpoint cadence
1. **At job start:** create/update the per-job checkpoint and push a small commit with current base SHA, exact scope, starting status and next step.
2. **After research / source mapping:** push findings before implementation. Include exact source paths/symbols, key decisions, risks and minimal patch plan.
3. **After each meaningful code milestone:** commit and push compilable or clearly marked WIP changes; update checkpoint with test status and next command.
4. **Before expensive ARM/CI build or broad test run:** push the checkpoint and current source, so work survives a quota cutoff.
5. **After each failure or green test:** append concise command, error/result, commit SHA and next action; push promptly.
6. **At task completion or voluntary stop:** update the job tree and checkpoint with outcome, artifact SHA and remaining hardware acceptance. Do not wait until allocation is nearly exhausted to start saving.

## Where to write
- **Primary live checkpoint:** `docs/CODEX_CHECKPOINT_JB28A.md` (use a new per-job checkpoint for subsequent jobs).
- **Job tree:** `docs/JIMBONES_JOB_TREE_V184.md` if present on working branch; if not, maintain `docs/JIMBONES_WORK_QUEUE.md` on this branch, and link to the canonical tree on `agents/wavetable-research`. Do not assume cross-branch edits are automatically visible or overwrite another branch.
- **Code:** commit/push to the dedicated work branch, never main. Keep partial code labeled WIP; never publish an image unless all integrity gates pass.

## Checkpoint content (small, actionable)
- Current stage: NOT STARTED / RESEARCHED / PATCHING / TESTING / BLOCKED / CODE READY / HARDWARE ACCEPTED.
- Source branch and pushed SHA; donor SHA; relevant changed files and symbols.
- Findings and decisions (including *why*), risks and remaining unknowns.
- Test commands/results with concise error excerpts, not huge logs.
- **Exact next action** executable by a fresh Codex session.
- Hardware acceptance status separate from CI.
- UTC time and links to PR, run and artifacts where available.

## Failure and quota handling
- Do not rely on predicting the remaining allocation; checkpoint at deterministic milestones and before expensive work.
- If push fails, retry once and explicitly report **NOT PERSISTED**; do not falsely claim checkpoint saved.
- On a conflict, fetch/rebase or make a distinct checkpoint commit; never force-push over another agent's work.
- Do not overwrite a previous checkpoint log. Keep entries concise and append-only.
- A new Codex session should first read only the latest checkpoint and one-page task; avoid repeating completed exploration.

## User-facing status
Always distinguish **remote checkpoint pushed**, **local-only work**, **CI passed**, and **physical device accepted**. A workflow-tree checkbox is not evidence of a working feature.
