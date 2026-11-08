# JIMBONES agent audit and integration decision — 2026-11-08

## Verified repository inventory
Compared all 12 agents/* branches against `jimbones-v1.8.03-hardware-test` using GitHub compare API.
- `agents/cpu-research`: 8 commits ahead / 6 behind, modifies Soundengine.c, telemetry header, host runner, telemetry test and agent brief.
- `agents/crunch-research`: 8 commits ahead / 6 behind, adds 1–4 voice deterministic and isolated summed-PCM diagnostics, host runner and agent brief.
- Remaining 10 branches (morph, tape, usb, stepfx, fm, wavetable, hihat, random, filters, ui): each 1 commit ahead / 6 behind, **only** a research brief; no feature implementation on those branches.

## CI verified
- CPU branch commit `c57c519c5cce15bf33be3c86d12353f9c2ac35cf`: GitHub Actions run 42456940467 completed success. Host-regression and firmware jobs both passed, including ARM image build and validation.
- Crunch branch commit `28ef6f7f70c5f8ba1fb60a4b5ce3070c83082e40`: GitHub Actions run 42456867458 completed success. Host-regression and firmware jobs both passed, including ARM image build and validation.
- Integration branch audit commit `01bf4125e8ee9798b519f7e61e1e27f703b9fee3`: run 42456811134 passed host and ARM.
- These are **separate branch** builds. No combined new integration build has yet been verified.

## Integration disposition
1. **Accept for integration review now:** CPU timing and queue-pressure math, but verify the actual queue is wired and reconcile duplicated existing DWT counters; keep only meaningful metrics. PR #3.
2. **Accept test infrastructure now:** deterministic 1–4 voice and isolated summed-PCM tests from PR #2. The latter is not a final mixer or post-FX test and cannot prove the reported crunch is resolved.
3. **Hold audio DSP gain/limiter changes:** no verified cause for 2+ modulated-voice crunch. Correlate clipping, DMA backlog, peak render cycles and layer saturation first.
4. **Defer new feature code:** morph A/B/C, tape, USB audio, step FX, FM optimisation, wavetable enhancements, hi-hat, randomisation, filters and UI are research only on their agent branches. Existing v1.8.03 may already contain earlier implementations; this audit is of **new agent work**, not the complete firmware feature set.
5. **Release gate:** update/rebase two code branches (both six commits behind), review overlapping host runner edits, merge sequentially into experimental branch, run host+ARM after each merge, capture source/image SHA256 and memory, then hardware test with pad. Keep known-working v1.8.02 untouched.

## Next practical engineering tasks
- Integrator: bring PR #2 test-only changes into experimental branch and run full CI.
- Diagnostics reviewer: reconcile PR #3 with pre-existing Soundengine/AudioDiagnostics counters and check timer semantics.
- DSP tester: add final mixer/post-FX stress and capture actual headroom and CPU deadline numbers.
- Feature leads: convert research briefs into small implementation PRs only after Gate A/B.

**No autonomous 26-agent runner has been observed.** These are tracked work roles and code branches, not background agents.
