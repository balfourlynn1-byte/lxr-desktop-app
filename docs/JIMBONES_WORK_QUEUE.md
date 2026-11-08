# JIMBONES project tree — active path to next full hardware candidate

Updated 2026-11-08. **User is testing v1.8.06 and finds it far from ready. v1.8.08 is a proposed feature-rich target, not a guaranteed release. ARM builds and image packaging are on HOLD until user explicitly approves a feature-frozen candidate.**

## Foundation decision
- **Primary research reference:** [Brendan Clarke's LXR02Open](https://github.com/brendanclarke/LXR02Open) (Sonic Potions 0.37 port); study its oscillator, waveform interpolation and modulation.
- **Current implementation branch:** `agents/jb-28-full-integration-v186`; **experimental native wave branch:** `agents/jb-27-native-wave-scan-v187` (not integrated or hardware verified).
- **Preserve:** original six voices; original 3 ms No Clip fade/retrigger continuity; reliable velocity response; current known-working v1.8.06 image and rollback; **v1.8.06 bouncing skull screensaver** (user reports it is very good), including its animation, wake behaviour and display interaction. Do not redesign it; if changing firmware foundation, port and re-test it.
- **Licence gate:** upstream non-commercial redistribution conditions vs public JIMBONES GPL. No unreviewed upstream source copied into public branch. Decide between independently authored compatible port or separate appropriately licensed noncommercial fork before implementation.
- **Scope rule:** do not reintroduce experimental Metallic FM or sacrifice CPU stability for effects. All optional DSP should bypass cleanly.

## Project tree / dependencies

| ID | Work package | State | Exit criterion / next action |
|---|---|---|---|
| JB-00 | Baseline and rollback | REQUIRED / OPEN | Pin exact v1.8.06 image SHA, source commit, kit/SD backup and reproducible hardware symptoms; retain known-good original firmware |
| JB-01 | No Clip 3 ms fade preservation | PRIORITY 1 / OPEN | Locate first working code, trace note-on/retrigger/overlap path, create audio regression and compare against v1.8.06 |
| JB-02 | Upstream oscillator audit | IN PROGRESS | Pin upstream commit; map waveform dispatch, two-wave block blend, FM/noise/sample transitions, oscillator state and CPU/memory exposure. See [audit plan](JB_WAVETABLE_UPSTREAM_PORT_PLAN.md) |
| JB-03 | Upstream modulation + velocity audit | OPEN | Map waveform-ID modulation destination, fractional LFO state, velocity routing and persistence; specify useful velocity destinations/curves |
| JB-04 | Licence/architecture decision | BLOCKING / OPEN | Confirm lawful reuse path and whether native Erica adaptation or private upstream-derived fork preserves essential native features, No Clip and kit compatibility |
| JB-05 | Host wavetable prototype | BLOCKED BY 01–04 | Implement native waveform scanning and velocity modulation in isolated branch, no unreviewed code copy; keep stock path bit-identical when OFF |
| JB-06 | Audio quality and CPU host validation | BLOCKED BY 05 | Compare stepped vs smooth sound, all waveform classes, 3–4 overlapping hits, retriggers, tail clicks, peak/rail, deterministic CPU/memory bounds |
| JB-07 | Step FX / performance essentials | QUEUED / SCOPE REVIEW | Confirm per-slot voice targeting and 4-knob controls; preserve functioning dub timing, Step 1 LED, original voice routing; avoid speculative extra features |
| JB-08 | Comb filter integration | PATCH PUSHED / PARTIALLY VERIFIED | PR #18 focused Comb test PASS in CI run 42511490948; full host regression stopped on stale version assertion, fixed commit 442d8be. Rerun GCC/Clang and review interactions; no ARM |
| JB-09 | Integrated feature freeze | BLOCKED BY 00–08 | User reviews exact included features; all focused and full host tests green; no known audible click/crunch regression; fix list and test sheet signed off |
| JB-10 | ARM build + memory + image validation | **ON HOLD — USER APPROVAL REQUIRED** | Only after JB-09 and explicit approval: cross-compile selected frozen SHA, verify ITCM >=512 B, flash/RAM, image header/CRC, manifest/SHA256 and rollback pack |
| JB-11 | Physical hardware acceptance | BLOCKED BY 10 | User tests LXR-02 with Alesis pads, KT-10, FD-8, overlapping toms; report DSP/PK/LATE/RAIL, wave sweep, velocity and No Clip; iterate only after observations |
| JB-12 | Candidate release / documentation | BLOCKED BY 11 | Release only accepted, checksum-pinned test image with honest version and hardware status; preserve earlier images |

## Existing jobs — mapping
- **JB-28A** Comb filter: maps to JB-08; current draft [PR #18](https://github.com/balfourlynn1-byte/lxr-desktop-app/pull/18).
- **JB-28B** integrated physical regression: deferred to JB-11 after feature freeze and ARM approval.
- **JB-29A** per-slot Step FX voice targeting: JB-07.
- **JB-29B** four-knob Step FX parameter UI: JB-07, after voice-target behavior.
- **JB-27** four-periodic-wave native interpolation: experimental donor/research, not a substitute for upstream parity.

## Immediate next three jobs (host-only)
1. **JB-01:** locate exact original No Clip 3 ms implementation, preserve tests and behaviour.
2. **JB-03 + JB-02:** inspect upstream `modNode_updateValue`, interpolation render and compare against Erica structures; document compatibility and licence decision.
3. **JB-05:** create isolated native wavetable/velocity prototype and audible host comparison, then full GCC/Clang gates.

**Explicit stop:** no ARM compilation, no .img packaging, no merge to main, no hardware-ready claim before the user approves the frozen candidate.

Historical planning tree: [JIMBONES_JOB_TREE_V184.md](https://github.com/balfourlynn1-byte/lxr-desktop-app/blob/agents/wavetable-research/docs/JIMBONES_JOB_TREE_V184.md).

## Autonomous execution board
The [atomic task checklist](JIMBONES_ATOMIC_TASKS.md) is the **canonical one-task-at-a-time execution queue**, with A01–E07, dependencies, evidence gates, per-task completion messages and explicit user-gated ARM hold. First actionable job: **A01 — locate the original 3 ms No Clip fade source and commit**. Future scheduled runs should complete one small task, push and report, not claim unverified progress.

### 2026-11-08 atomic B01 checkpoint
Upstream `brendanclarke/LXR02Open` `prime` HEAD pinned to `c896bfd0efe86a4037418bf9f46f1fcc39255958`; six file blob hashes and licence restrictions verified in `docs/JB_WAVETABLE_UPSTREAM_PORT_PLAN.md` (commit `a5c51dc`). Research-only; licence gate still blocking. Next priority A01 original No Clip source trace; then B04/B07.
