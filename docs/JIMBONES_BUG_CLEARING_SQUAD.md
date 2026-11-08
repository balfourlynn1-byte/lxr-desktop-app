# JIMBONES bug-clearing squad — 2026-11-08

**Status:** Dedicated agent-role assignments, not launched autonomous processes. A bug is not closed until a reproducible test passes and the relevant ARM build is verified. No automatic merge or flash.

| ID | Severity | Assigned pair | Observed evidence | Next concrete task | Closure gate |
|---|---|---|---|---|---|
| BUG-01 | P0 | Build Repair A + CI Reviewer B | JB-02 host script had an invalid doubled shell continuation; fixed on CPU branch at f57794c | Check the full script with shell parser, compile host suite, inspect PR #3 CI | Host suite green, ARM compile green |
| BUG-02 | P0 | Test Repair A + Regression Reviewer B | JB-03 host script had doubled continuations; corrected on crunch branch at ebf9d94 | Run host regression and preserve stdout/stderr; check C test format correction 8afe4b1 | Deterministic 1–4 voice runs, sanitizer clean |
| BUG-03 | P0 | Audio DSP Debugger A + Audio Measurement B | User reports crunch with 2+ modulated voices; exact root cause unverified | Add final summed-mix and post-FX stress fixture; distinguish clipping from deadline miss | Reproduced failure, targeted fix, before/after metrics, hardware test |
| BUG-04 | P0 | CI Config A + ARM Build Reviewer B | Workflow previously excluded PRs into experimental branch; trigger fixed at 6995759 | Verify actual runs on PR #2/#3, collect logs, resolve build errors | Both PRs host and ARM green |
| BUG-05 | P1 | Diagnostics A + Timing Reviewer B | Existing CPU diagnostics time rendering only; queue pressure and complete processing not established | Audit DMA/queue signals, instrument safely without ISR logging | Benchmarked overhead and verified interpretation |
| BUG-06 | P1 | DSP Gain A + Mix QA B | Per-voice saturation and global kitWeight may interact with multi-voice headroom; code-level hypothesis | Profile mix path and peak amplitudes; avoid changing gain law before reproducible evidence | Stable output, no regressions in single voice |
| BUG-07 | P1 | Integration A + Release QA B | Multiple unmerged experimental PRs, no newly verified combined firmware | Establish PR dependency/merge sequence and release checklist | Source SHA, host + ARM logs, image checksum, rollback |
| BUG-08 | P2 | UI Engineer A + UX Tester B | CPU display semantics may be confused with total CPU utilisation | Label render-load vs queue pressure and show peak/overrun counter | UI navigation tests, readable on hardware |

## First execution batch
1. BUG-01/02: repair verification and regression CI.
2. BUG-03/06: isolate audible crunch without speculative DSP edits.
3. BUG-04/07: make reproducible builds and preserve working v1.8.02.
4. BUG-05/08: refine CPU diagnostic semantics.

## Rules
- Each pair has implementer and independent verifier.
- Separate bugfix branches and focused draft PRs; never overwrite known-working firmware.
- Log failing and passing evidence with exact revision.
- The bug list records **known process issues and hypotheses**, not eight confirmed firmware defects.
