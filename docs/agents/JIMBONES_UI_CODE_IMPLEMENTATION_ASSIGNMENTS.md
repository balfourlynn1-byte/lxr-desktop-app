# JIMBONES feature submenu implementation assignments — CODE REQUIRED

**2026-11-08.** These are tracked work orders, not autonomously running agents. Baseline: `agents/jb13-native-submenu-ui` / draft PR #7. Each agent must submit **working code**, tests and a draft PR, not only UI recommendations. Do not modify v1.8.02 or merge to hardware-test without review.

## Required deliverables for each feature
1. Implement real menu button/submenu navigation on native LXR-02 16x2 display and four knobs, following `Core/Src/menu/menu.c`, `Core/Inc/menu/menuPages.h`, and JB-13 router. Coordinate ownership to avoid conflicts in the shared menu.c file.
2. Show active voice, feature, four-or-fewer parameter names/values, bypass state, and a clear PENDING marker for DSP not implemented. No dummy parameter that silently changes sound.
3. Wire controls to **actual opt-in DSP** only where implemented; ensure bypass and legacy six-voice output unaffected. Add runtime state reset on kit load and versioned backward-compatible persistence or mark TEMP and block release.
4. Include deterministic host UI transitions, knob bounds, per-voice isolation, save/load, CPU/deadline and four-voice overlapping modulated audio regression. Explicit intentional coloration tests.
5. Provide a draft PR, exact commit and Actions host+ARM CI evidence. Independent JB-20 review required. **P0 NO CLIP:** any unintended clipping/crunch/click/pop/dropout is FAIL.

## Individual code assignments
| Agent | Native button / submenu | Deliverable |
|---|---|---|
| JB-04 | MOD / MORPH | immutable A; B/C edit and flashing target; kick exclude |
| JB-05 | MIX / TAPE | per-voice opt-in tape drive; clean bypass |
| JB-07 | PERFORMANCE / STEP FX | 16 independent toggle/flash; dub sends; no global off |
| JB-08 | OSC / METAL | FM layer controls and CPU-bounded enable |
| JB-09 | OSC / WAVE | wavetable interpolation and shape controls |
| JB-10 | MOD / HAT | FD-8 CC4 open/closed/choke controls |
| JB-11 | MOD / SNARE | seeded probability and backbeat lock |
| JB-12 | FILTER / BASS+COMB | safe HPF/LPF and comb bounded feedback |
| JB-14 | CLICK / TRANSIENT | attack/sustain, bypass and persistence |
| JB-15 | FM / FM DRUM | body/noise and mix |
| JB-16 | MOD / VELOCITY | target, depth, mask and kick exclude |
| JB-17 | FILTER / COMB ROUTE | polarity, pre/post, safe feedback |
| JB-18 | MIX / LOFI | bit/SRR controls, no clicks on changes |
| JB-19 | OSC / CLUSTER | detune, spread and CPU cap |
| JB-01 | GLOBAL / ABOUT | Read-only version, SHA, rollback status and legacy kit checks |
| JB-02 | PERFORMANCE / CPU | Actual deadline/peak telemetry and safe reset controls |
| JB-03 | PERFORMANCE / AUDIO | Pre/post mixer rail, clips, overruns diagnostics |
| JB-06 | NONE | USB audio deferred; no code or menu in this candidate |
| JB-13 | ALL | Merge collision-free shared menu router, display/encoder conventions and menu tests |
| JB-20 | PERFORMANCE / BUILD | Read-only build/qualification state, independent QA, never claim hardware pass from CI |

## Review sequence
JB-13 audits all UI diffs and consolidates shared routing; JB-02/JB-03 validate CPU/audio measurements; JB-20 runs independent regression and blocks any No Clip failure. PRs stay draft until reviewed. No auto-merge and no claim of autonomous work completion.

## Specialist reinforcement — JB-21 through JB-26 (2026-11-08)
Six independent specialist work-order branches have been created: `agents/jb-21-specialist` through `agents/jb-26-specialist`. Their tasks: JB-21 overlapping modulation/No Clip root cause, JB-22 CPU deadlines, JB-23 independent PCM/audio regression, JB-24 ARM build and memory, JB-25 Morph A/B/C and protected voices, JB-26 layered DSP optimisation. These are **assignments, not running autonomous agents or completed implementations**. Prioritize JB-21–24 before feature growth. Every candidate needs real independent PR review and JB-20 signoff; no unverified ARM release.
