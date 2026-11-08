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

## 2026-11-08 B01 immutable upstream reference
Verified upstream `prime` HEAD at `c896bfd0efe86a4037418bf9f46f1fcc39255958` through GitHub commit lookup, then fetched six source/audit/licence blobs by exact commit; all blob hashes are recorded in `docs/JB_WAVETABLE_UPSTREAM_PORT_PLAN.md`. The LICENSE expressly forbids commercial use and imposes modified-source redistribution obligations; copying code to the public GPL repository remains blocked pending B07. Evidence commit `a5c51dc2a60b25108ca8689593e81932c5629711`. No compilation or audio testing.

## 2026-11-08 A01 — No Clip original source trace (PARTIAL; earliest commit pending)
Located historical branch `jimbones-noclip-baseline` through GitHub branch search. **Actual source verified**, no assumption from docs:
- `Core/Inc/Soundengine/DSPAudio/RetriggerSmoother.h`, Git blob `2818d42043d604862004c10035fb6d5e3a6cefed`, lines 12–14: `LXR_RETRIGGER_FADE_SAMPLES = 132u`, annotated ~3 ms at `REAL_FS ~= 44140 Hz` (132/44140 = 2.99 ms); lines 16–28: correction/decrement, last sample, remaining and pending state; init/note-on/process API.
- `Core/Src/Soundengine/DSPAudio/RetriggerSmoother.c`, blob `b8b480d6c9eff062c5c44e83478ef285ed8f9b71`, lines 9–16 initialize; 18–24 only set pending for overlap; 33–60 process: when pending, offset `last_sample - audio[0]`, decrement by offset/131, 132-sample correction ramp; clamp to int16; retain last processed sample. This aligns new first sample to previous final sample without delaying the trigger.
- `Core/Src/Soundengine/DSPAudio/DrumVoice.c`, blob `6d276b5415466113c7951dd63269fb17e6e91345`, lines 124–133: `Drum_trigger` calls smoother note-on **before envelope retrigger**, overlap condition `oscVolEg.value > 0.015 && state != EG_STOPPED`; lines 323–326: `Drum_render` applies smoother after voice volume and before mixer decimation/routing; lines 68–69 initializes per-drum smoother under `LXR_RETRIGGER_SMOOTH`.
- Scope is **three primary drum voices**, not all six voices. `Snare.c` and `HiHat.c` do not contain this smoother call in the baseline. Guard is compile-time `LXR_RETRIGGER_SMOOTH`; verify its value on each candidate before claiming active.
- Important regression tests for A03: 132 exact samples across block boundaries; 1st corrected sample equals previous output unless saturated; correction ends on sample 132; no effect on nonoverlap attack; repeated retrigger during a running ramp; large ±32768 transitions and int16 saturation; all three drum voices independently; no impact on snare/hat.
- **A01 not DONE:** GitHub commit-message search for `RetriggerSmoother`, `retrigger smoothing`, `No Clip`, `noclip`, `click reduction` returned no introducing commit. Need file-history/blame or git log --follow for `RetriggerSmoother.c` to pin earliest pushed introducing SHA. No compilation, host audio test or ARM build performed.
