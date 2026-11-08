# JIMBONES — Codex handover for 10 November 2026
**Status: PREPARATION IN PROGRESS; NOT A BUILD AUTHORIZATION.**
**User goal:** finish research and preparatory work by Nov 9, hand Codex a tightly scoped final-candidate implementation/build brief on Nov 10. User testing v1.8.06 reports major feature shortcomings but strongly approves bouncing skull screensaver. Desired improvement: functional native wavetable scanning and velocity-sensitive modulation, not experimental Metallic FM.

## Source of truth
- Repository: `balfourlynn1-byte/lxr-desktop-app`
- Working integration branch: `agents/jb-28-full-integration-v186`; draft PR #18 based on `agents/jb-26-skull-screensaver-v185`.
- Experimental native-wave branch: `agents/jb-27-native-wave-scan-v187` (four periodic waves only; not hardware accepted).
- Canonical granular work: `docs/JIMBONES_ATOMIC_TASKS.md`; roadmap: `docs/JIMBONES_WORK_QUEUE.md`; technical source analysis: `docs/JB_WAVETABLE_UPSTREAM_PORT_PLAN.md`; progress checkpoint: `docs/CODEX_CHECKPOINT_JB28A.md`.
- Upstream reference: `brendanclarke/LXR02Open` branch `prime`; source `Core/DSPAudio/Oscillator.c` blob `5c1bf056b8016ff1a43a87f7f72ae6bf59bb4467`, `Core/DSPAudio/modulationNode.c` blob `e3a811bae6dfcb333601b02540956944873b18ee`, audit `knowledge_files/OSC_INTERP_AUDIT.md`. Recheck HEAD SHA before coding.
- Upstream is a Sonic Potions 0.37 port with non-commercial licence constraints; public JIMBONES is GPL-only. **Do not copy incompatible upstream source into public repository without licensing resolution.**

## Non-negotiable preservation
1. Original six voices and factory FM sound when new options OFF.
2. Original **3 ms No Clip fade** on retrigger/overlap; first exact implementation commit and sample-count test remain to be located.
3. v1.8.06 bouncing skull animation, timing and wake behaviour (user hardware-approved).
4. No Metallic FM experimental layer; avoid CPU crunch, LATE/RAIL growth, clipping.
5. Rollback to prior known-working hardware firmware; no merge to main.

## Known research (verified source, not hardware acceptance)
- B02 DONE: upstream six voice waveform destinations; fractional base/next blend and generation reset; global OFF default and per-block active budget. See upstream modulationNode.c lines 61–109, 173–194, 253–305.
- B03 DONE: upstream dual reusable block buffers, pair render and blend, FM path, native noise/crash/sample dispatch; oscillator.c lines 98–155, 186–211, 229–253, 552–590.
- JB-28 Comb patch pushed in PR #18; focused Comb CI tests passed in run 42511490948. Full regression originally failed on stale version assertion, fixed commit 442d8be; **full host suite not yet independently confirmed green**.
- ARM workflow job explicitly disabled. No new hardware image approved.

## Research and prep required by Nov 9
- [ ] Locate and pin original No Clip 3ms source, code path, sample rate and regression.
- [ ] Pin approved skull code and wake/display test.
- [ ] Map exact Erica/upstream oscillator and modulation types and waveform IDs.
- [ ] Evaluate licence-compatible architecture and recommend whether to adapt Erica engine or isolate noncommercial fork.
- [ ] Define minimum compelling v1.8.08 candidate feature matrix: smooth native waveform scan, useful velocity-to-wave and second velocity destination, original voice bypass, No Clip, skull, reliable Step FX; distinguish must-have from defer.
- [ ] Audit current PR #18 CI host regression, known defects, CPU and sample/persistence constraints.
- [ ] Prepare exact host test commands, expected metrics, acceptance and rollback plan.
- [ ] Record each unresolved issue honestly with owner and next command.

## Codex execution order (once available)
1. Read this handover, atomic tasks, roadmap, and checkpoint; inspect exact Git SHA/working tree.
2. Resolve A/B research blockers and source compatibility before copying or implementing.
3. Implement smallest isolated native oscillator changes, no source licence breach, with OFF-path bit identity and 3 ms No Clip preservation.
4. Implement velocity and essential performance controls; preserve skull; run focused host tests and full GCC/Clang on exact candidate SHA.
5. Produce feature-freeze report with audio metrics, CPU/stack/memory risks, remaining issues and proposed version.
6. **STOP AND REQUEST EXPLICIT USER APPROVAL FOR ARM BUILD.** Only then re-enable cross-compile and image packaging, verify ITCM >=512 bytes, updater checksum and artifact hash. Never claim hardware acceptance without user tests.

## Candidate acceptance checklist
- Native waveform position changes audibly with LFO/velocity; expected transitions measured and rendered.
- Velocity zero/mid/max; default OFF original audio bit-identical.
- Fast successive long-tone triggers and overlapping toms: no click/crunch regression vs No Clip baseline.
- 3–4 simultaneous hits, DSP headroom, no LATE/RAIL climb on hardware.
- Comb and Step FX no cross-voice surprises; Step 1 LED and control persistence.
- Bouncing skull unchanged in user-facing behaviour.
- No hardware image until explicit user approval.

## Handover progress
- 2026-11-08: Initial handover drafted. B02/B03 verified. Remaining items above are **not yet done**.
