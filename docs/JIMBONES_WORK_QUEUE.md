# JIMBONES active work queue (branch-local, persisted by Codex)

Canonical planning tree: [JIMBONES_JOB_TREE_V184.md on agents/wavetable-research](https://github.com/balfourlynn1-byte/lxr-desktop-app/blob/agents/wavetable-research/docs/JIMBONES_JOB_TREE_V184.md).

| Job | Stage | Last pushed checkpoint | Next action |
|---|---|---|---|
| JB-28A Comb integration | CODE PATCH PUSHED — CI UNVERIFIED | Draft PR #18; checkpoint `46a9296` | Inspect JB-28A Actions run, fix failed gates; report ITCM and validated artifact only if green |
| JB-28B integrated physical regression | QUEUED | — | Wait for JB-28A candidate |
| JB-29A per-slot Step FX voice targeting | QUEUED | — | Wait for isolated integration and hardware test |
| JB-29B four-knob parameter redesign | QUEUED | — | Wait for JB-29A validation |

**Codex:** update this queue and `docs/CODEX_CHECKPOINT_JB28A.md` at each milestone; commit and PUSH, not just edit locally. Do not mark a feature accepted before user hardware confirmation.
