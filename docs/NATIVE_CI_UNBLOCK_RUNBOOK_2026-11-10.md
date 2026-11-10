# JIMBONES — unblock host-only native CI (10 Nov 2026)

**Purpose:** obtain a genuine GCC/Clang syntax result for `Core/Src/Soundengine/DSPAudio/DrumVoice.c`. Do **not** build ARM, create an image, merge branches or flash hardware.

## Evidence
- Experimental branch: `agents/jb-wavetable-host-experiment`; integration branch: `agents/jb-28-full-integration-v186` (read-only for this operation).
- Latest observed host-only workflow run: `42720355331`, failed at older SHA `6d3ae26f86b868c88bc6d9794a6431e5ed272f47`. No new run confirmed for corrected probe.
- Workflow: `.github/workflows/jb-native-drumvoice-probe.yml`; host script: `tests/run_native_drumvoice_probe.sh`.
- Repository-content API writes succeed, but container Git clone fails: `Could not resolve host: github.com`.
- **Hypothesis, NOT proven:** commits made through automation may not generate downstream push workflows if authenticated with `GITHUB_TOKEN`. GitHub documents this restriction: https://docs.github.com/en/actions/how-tos/write-workflows/choose-when-workflows-run/trigger-a-workflow

## Fastest workaround: user-authenticated local Mac checkout (no ARM)
On a network-connected Mac with Git and Xcode Command Line Tools:
```bash
xcode-select --install # only if command line tools are absent
git clone https://github.com/balfourlynn1-byte/lxr-desktop-app.git
cd lxr-desktop-app
git fetch origin agents/jb-wavetable-host-experiment
git switch --track origin/agents/jb-wavetable-host-experiment
git rev-parse HEAD
bash -n tests/run_native_drumvoice_probe.sh
bash tests/test_native_probe_preflight.sh
CC=clang bash tests/run_native_drumvoice_probe.sh
```
Stop and save the complete compiler output if any command fails. The last command is **syntax-only**, not link/audio/hardware verification. If branch already exists locally, use `git switch agents/jb-wavetable-host-experiment` instead of `--track`.

## Check whether GitHub Actions can run
1. Open https://github.com/balfourlynn1-byte/lxr-desktop-app/actions and check whether workflows are disabled/restricted. Only enable the **host-only** workflow if necessary; never enable ARM workflows.
2. Because `workflow_dispatch` generally requires the workflow to be present on the default branch, do not merge or copy firmware to `main` just to obtain a Run workflow button.
3. From a user-authenticated checkout of the experimental branch, if a harmless trigger is required, run:
```bash
git commit --allow-empty -m "ci: request host-only DrumVoice probe"
git push origin HEAD:agents/jb-wavetable-host-experiment
```
   This may generate a push event independently of automation credentials. Confirm the workflow run's exact `head_sha` matches the pushed commit; if not, treat the trigger as failed. Do not use `--force`.
4. Inspect the run's first failing compiler line and record command, toolchain version, job URL, exact SHA, PASS/FAIL/NOT RUN in all tracking docs.
5. If no run appears after a user-authenticated push, investigate repository Actions settings, workflow enablement, account permissions and GitHub event delivery. Do not claim native compilation succeeded.

## Acceptance and stop rules
- PASS only when actual `DrumVoice.c` host syntax compilation exits 0 and full logs are available; distinguish GCC and Clang.
- STOP on missing STM32/CMSIS headers, compiler errors, workflow failure or unexpected changes. No synthetic fixture can substitute for the actual translation unit.
- After compilation, separately establish native link, OFF-path PCM bit identity, 3–4 hit/No Clip regressions, SD save/recovery and UI/BPM behavior.
- Preserve all six voices, kits, 3 ms No Clip, approved skull; Metallic FM shelved.
