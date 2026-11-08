# JIMBONES combined hardware-test release — implementation queue and progress
Updated 2026-11-08. **All percentages are engineering estimates of end-to-end readiness (code + host test + ARM build + integration + physical validation), NOT automatically measured completion.** Device verification is 0% for the new combined candidate. Assignments are GitHub work orders for virtual implementer/reviewer roles, not running autonomous agents.

## Mandatory release blockers — execute first
| Work order | Implementer deliverable | Independent reviewer acceptance |
|---|---|---|
| JB-01 Baseline & release | Lock v1.8.02 rollback image/hash; produce exact candidate SHA, binary size, map and recovery notes | Verify six original voices, old kits, no bootloader changes |
| JB-02 CPU monitor | Show render cycles, peak and deadline misses in performance UI; instrument realistic ISR budget | Reproduce 1–4 simultaneous hits, long tails, CPU wrap/reset tests |
| JB-03 Crunch | Trace pre/post FX clipping and final output with four-voice overlapping mod; eliminate cause | Deterministic final mixer stress, user-recorded A/B hardware test |
| JB-13 UI integration | Wire real buttons, LEDs, CLICK submenu, morph edit indicators, CPU page | Verify debounce, navigation, state persistence and device behavior |
| JB-20 Integration & QA | Stage reviewed feature branches one by one, host+ARM CI on exact commit, release checklist and manifest | Reject untested combinations, stale artifacts and claims of hardware pass |

## Implementation teams — deliver code, not only research
| ID | Scope | Implementer goal | Reviewer test |
|---|---|---|---|
| JB-04 | Morph A/B/C | Protected A, edit B/C, LFO+velocity, per-voice mask | Old kit compatibility, kick exclusion, zipper noise |
| JB-05 | Tape/tube | Per-voice low-end-safe drive, velocity routing | Bypass, spectrum, four-voice CPU |
| JB-07 | Step FX & dub sends | Independent button toggles, flashing LEDs, 2/4/8-bar events, delay/reverb send throws | Clock drift, multi-effect bounds, kick exclusion, feedback stability |
| JB-08 | Metallic FM | Optional layer alongside native engines | Headroom, CPU, pitch/aliasing |
| JB-09 | Wavetable + waveshaping | Interpolation, fold/phase warp, optional click sample research | Bandlimiting, memory and kit regression |
| JB-10 | FD-8 hi-hat | CC4 open/closed, choke and dynamic velocity | Real pedal replay and closed-tail behavior |
| JB-11 | Snare variation | Per-hit seeded choice, backbeat protection | Repeatability and no mid-tail switching |
| JB-12 | Bass/comb filters | Independent HPF/LPF and tunable stable comb | Feedback runaway and CPU |
| JB-14 | Transient Designer in CLICK | Finish CLICK menu wiring, optional attack/sustain + click layer and save/load design; DSP already in isolated branch | Default-off parity, old kits, four-voice headroom, physical menu |
| JB-15 | FM Drum body/noise | Optional pitch/body/noise engine, reuse existing FM | No duplication, bounded cycles |
| JB-16 | Velocity matrix | Routes to transient, FM, morph, filter and tape | Protected A, kick opt-out, repeated-hit safety |
| JB-17 | Comb+/− routing | Filter polarity and pre/post order | Feedback, DC, memory |
| JB-18 | Bit/SRR | Per-voice sample-rate/bit reduction | Alias, level, CPU |
| JB-19 | Oscillator cluster | CPU-bounded detuned texture; defer if expensive | 4-voice deadline |

## Estimated completion toward *hardware-tested combined build*
| Feature / workstream | Estimate | Evidence / principal gap |
|---|---:|---|
| Original six-voice preservation & kit regression | 70% | Host fixtures exist; full device A/B still needed |
| CPU diagnostics / monitor | 60% | DWT render telemetry and CI exist; full deadline/UI hardware validation incomplete |
| Multi-voice crunch fix | 35% | Stress/headroom diagnostics exist; user still heard crunch, final root cause unconfirmed |
| Metallic FM experimental layer | 65% | DSP and host regression exist; performance hardware validation pending |
| Wavetable interpolation/waveshaping | 65% | DSP and host regression exist; sound/CPU hardware validation pending |
| Transient Designer / CLICK submenu | 40% | Prototype + engine wiring + host/ARM CI on draft PR; CLICK UI and persistent storage absent |
| Step FX tempo retrigger / phrase scenes | 55% | Engine and tests exist; actual hardware and effects validation pending |
| Step FX 16-button toggles & blinking LEDs | 20% | Isolated UI state and test; engine/hardware routing absent |
| Dub-style Step FX sends / returns | 5% | Requirement specified; no proven audio send/return DSP |
| Morph A/B/C protected editing | 15% | Goals and work order; no demonstrated complete integration |
| Per-voice tape/tube saturation | 15% | Research/planning; existing drive not proof of requested tape design |
| FD-8 articulation and choking | 20% | Some existing closed-hat behavior; full CC4 device path unverified |
| Seeded snare variation | 10% | Assignment; full trigger-only engine switching unverified |
| Bass HPF/LPF + Octatrack comb | 10% | Design; stable integrated implementation unverified |
| FM Drum body/noise | 10% | Implementation branch/work order; no validated new engine |
| Velocity modulation matrix | 15% | Bounded velocity-route prototype and host fixture on JB-16 branch; not yet in live DSP |
| Comb+/− and routing | 5% | Assignment; unimplemented |
| Per-voice bit/SRR | 55% | JB_BITS plus opt-in SRR wired into experimental engine in draft PR #5; UI/kit persistence and hardware tests pending |
| Detuned oscillator cluster | 5% | Assignment; unimplemented |
| CLICK custom transient samples | 5% | Requirement only; sample format/storage unresolved |
| UI double-press / performance page | 20% | UI work order; full menu hardware validation absent |
| Combined firmware build and release qualification | 25% | Previous integration CI passed; new feature branches not merged/qualified as one exact candidate |
| Four-channel USB multitrack | Deferred | Excluded from this candidate by design |

**These are rough readiness estimates, not percent of lines coded or pass probabilities.** Prioritize stability and working test image over attempting all features at once. Every feature needs implementation, independent review, CI, ARM build, hardware test; defer costly features explicitly rather than silently drop them. The new JB-14 draft PR is https://github.com/balfourlynn1-byte/lxr-desktop-app/pull/4 ; its CI run 42459381928 host-regression and firmware jobs passed, but CLICK UI is not connected and no device test occurred.

## Implementation update — 2026-11-08
- JB-14: additional four-voice/long-tail/rail regression committed on feature branch; latest commit `406712daea264a7d2aff0b54cd41828ad07945a2`. Prior CI success predates this new test; reverify before merge.
- JB-16: `JimbonesVelocityMatrix.h` and `test_velocity_matrix.c` plus host runner, latest commit `70cafd6db9e38e8d2dacf260077c14dd798fac1a`. Isolated primitive only, no live DSP routing.
- JB-18: `JimbonesSampleRateReducer.h` and `test_sample_rate_reducer.c` plus host runner, latest commit `ff5a63cf03a93c3e6ad7731a258af19406f013ea`. Isolated primitive only, no live DSP routing.
- Percentages changed conservatively. CI for latest commits and device verification must be checked separately.

## Implementation update — 2026-11-08 (continued)
- JB-14 latest four-voice stress fixture commit `406712da`: GitHub Actions run `42459662194` **host-regression success, firmware success**. CLICK submenu and persistent controls still absent.
- JB-18 SRR opt-in audio path and regression in draft PR #5: https://github.com/balfourlynn1-byte/lxr-desktop-app/pull/5. Controls are runtime-only, bypass default, no kit serialization. Latest host/ARM CI not yet verified; reviewer must inspect reset/trigger edge cases and switching clicks.
- Next integration gates: locate native CLICK submenu code, implement menu control mapping with backwards-compatible persistence; compare four-voice DSP load and output rail statistics for JB-14 and JB-18 together before merging either.
