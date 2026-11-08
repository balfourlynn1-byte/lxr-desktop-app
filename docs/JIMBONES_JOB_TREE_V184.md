# JIMBONES v1.8.04–1.8.06 — build job tree and Codex handover

**Updated:** 2026-11-08. **Purpose:** make jobs small and ready for Codex, with clear dependencies and test gates. 
**Status of this programme:** v1.8.04 has user-reported stable boot, ~62% DSP and no new audible crunch; new 1.8.05–1.8.06 changes remain **untested on hardware**. Exact PK/LATE/RAIL and four-pad stress on v1.8.04 are still unrecorded. CI does not prove realtime headroom. Branch roles in this board are **virtual assignments**, not autonomous active agents. CI passing is not a substitute for physical tests.

## Active dependency tree — 18:30 Codex handover

**Handoff ready:** [one-page Codex 18:30 instructions](https://github.com/balfourlynn1-byte/lxr-desktop-app/blob/agents/jb-28-full-integration-v186/docs/CODEX_1830_HANDOFF.md) · **Codex starting branch:** `agents/jb-28-full-integration-v186` (forked from JB-26, **Comb not yet transplanted**).

```text
BASE: known-working v1.8.04 + SD/kit rollback (do not overwrite)
 |
 +-- IMPLEMENTED, STACKED, CI PASS — review before integration:
 |   #8  JB-19A Step FX send gate
 |   #9  JB-19B dry + FX mixer
 |   #10 JB-21 16-button toggle + Step 1 LED
 |   #11 JB-20 tempo dub delay
 |   #12 JB-22 integration / Metallic FM disabled
 |   #13 JB-24 wavetable layer / wave-first OSC UI
 |   #14 JB-23 test pack
 |   #16 JB-25 five dub interval pads (v1.8.05)
 |   #17 JB-26 LCD fix + bouncing skull (v1.8.06)
 |          |
 |          +-- [CODEX 18:30 P0] JB-28A COMB INTEGRATION
 |          |    branch agents/jb-28-full-integration-v186
 |          |    take ONLY Comb changes from #15 (older branch)
 |          |    ninth FILTER Type; no loss of JB-25/JB-26
 |          |    full host + ARM + ITCM gate; v1.8.07 test image
 |          |          |
 |          |          +-- [P0] JB-28B combined regressions
 |          |          |    + physical 4-pad / Comb / LCD test
 |          |          |    + DSP/PK/LATE/RAIL and rollback
 |          |          |
 |          |          +-- [P1 conditional] JB-27 native
 |          |               oscillator waveform-ID/LFO scan
 |          |               + original waveshaping (licence/CPU)
 |          |
 |          +-- [PARALLEL SOURCE, NOT YET IN v1.8.06]
 |               #15 JB-12 Comb prototype, CI PASS on old branch
 |
 +-- AFTER HARDWARE HEADROOM: morph A/B/C; tape/tube;
 |   hi-hat; snare random; independent bass HPF/LPF;
 |   advanced 2/4/8-bar performance FX
 |
 +-- FEASIBILITY ONLY: USB 4-channel audio
 +-- HOLD: Metallic FM (CPU overload, little sonic benefit)
```

**Do not merge #15's entire old branch over #17.** Transplant only its Comb files and small patches into the new JB-28 branch, then rerun every gate. The two independent green builds do **not** establish a green combined build.

**Review order:** #8 → #9 → #10 → #11 → #12 → #13 → #14 → #16 → #17 → new JB-28A draft PR. All are stacked except #15, which is the source of the Comb patch. No GitHub Release or flash without hardware approval.

## Work queue and status

| Job | Priority | Status and proof | Next action | Owners |
|---|---|---|---|---|
| **JB-19A** FX send envelope | P0 | Source + host tests in [PR #8](https://github.com/balfourlynn1-byte/lxr-desktop-app/pull/8) | Independent integration review | DSP |
| **JB-19B** additive FX bus mixer | P0 | Host + ARM CI pass; [PR #9](https://github.com/balfourlynn1-byte/lxr-desktop-app/pull/9) | Review output-routing and race rules | DSP / Verification |
| **JB-21** Step FX toggles and LED | P0 | Host + ARM CI pass; [PR #10](https://github.com/balfourlynn1-byte/lxr-desktop-app/pull/10) | Actual physical Step 1 LED and 1–16 toggle test | Embedded / Verification |
| **JB-20** clocked dub delay | P1 | Host + ARM CI pass; [PR #11](https://github.com/balfourlynn1-byte/lxr-desktop-app/pull/11) | Test external clocks, physical FX output and wet tail | DSP / Verification |
| **JB-22** integration + FM-off | P0 | GCC/Clang, ARM + image validation PASS; **560 B free ITCM** in [run #42485178857](https://github.com/balfourlynn1-byte/lxr-desktop-app/actions/runs/42485178857); [PR #12](https://github.com/balfourlynn1-byte/lxr-desktop-app/pull/12) | Review the combined signal path, unchanged kit format and device risk | Integration / Verification |
| **JB-24** wave-first OSC UI | P1 | GCC/Clang + ARM + memory/image CI PASS in [run #42486888340](https://github.com/balfourlynn1-byte/lxr-desktop-app/actions/runs/42486888340); [PR #13](https://github.com/balfourlynn1-byte/lxr-desktop-app/pull/13) | Review 7 pages + interpolation on actual 16×2 display, preset legacy modes | Embedded / DSP |
| **JB-23** device test pack | P0 | **[Draft PR #14](https://github.com/balfourlynn1-byte/lxr-desktop-app/pull/14), [GCC/Clang + ARM/manifest checks GREEN](https://github.com/balfourlynn1-byte/lxr-desktop-app/actions/runs/42487592194)**; [UNVERIFIED artifact](https://github.com/balfourlynn1-byte/lxr-desktop-app/actions/runs/42487592194/artifacts/13620400602) published, 560 B ITCM free, SHA256 `0be1f7553f0fd86adbd8c49891b5de0c2f44a46348642e369b6223a76c741aae` | Independently review stacked PRs, verify working rollback image, then actual supervised hardware tests. No routine use approval. | Verification / Integration |
| **JB-03** post-FM-removal DSP verification | P0 | **Hardware report on v1.8.04:** ~62% DSP and the previous high readings/crunch appear gone; dub send sounds good. RAIL rises only with sequencer playing, not at idle, with no audible noise reported. **PK/LATE and controlled 2-tom/4-pad stress unreported.** | Log PK, LATE and loaded DSP/RAIL; treat v1.8.03 101% as historical. Do not write a speculative overload fix. | Verification (DSP conditional) |
| **JB-02** DSP/PK/LATE/RAIL meter | P0 | Present in experimental firmware and hardware readings on earlier build | Confirm semantics and reset behavior on new image; compare idle and loaded cases | Embedded / Verification |
| **JB-01** baseline parity and rollback | P0 | Source and CI regressions exist; user has booted prior experimental firmware | Verify six original voices, physical outputs, kits and recoverable rollback on device | Integration |
| **JB-09B** further wavetable integration | P1 conditional | Existing JB-24 UI and native tables work, but upstream LXR02Open's waveform-ID/LFO interpolation and user sample/loop system are **not fully ported**. | **JB-27** feasibility and independent implementation plan, CPU bench against ~62% baseline; noncommercial upstream code not copied into public GPL repo without separate permission. | DSP / Research |
| **JB-12** Comb filter / independent bass filter | P1, hardware-dependent | **[Draft PR #15](https://github.com/balfourlynn1-byte/lxr-desktop-app/pull/15) implemented Comb as ninth FILTER Type across six voices. [GCC/Clang host+ARM CI passed](https://github.com/balfourlynn1-byte/lxr-desktop-app/actions/runs/42495034632) with 528 B ITCM free.** Physical DSP/audio test still needed; separate bass HPF/LPF not implemented. | Verify review and hardware CPU/PK/LATE/RAIL on Comb versus native filter; do not release until stable | DSP / Verification |
| **JB-25** five dub timing buttons | P1 | [Draft PR #16](https://github.com/balfourlynn1-byte/lxr-desktop-app/pull/16), [host+ARM+image CI GREEN](https://github.com/balfourlynn1-byte/lxr-desktop-app/actions/runs/42498555769), v1.8.05. Step FX 1–5 quarter, eighth, triplet eighth, sixteenth, thirty-second; 6–16 originals. | Physical 5-button timing, LEDs, DSP/PK/LATE/RAIL and delay-buffer modes; v1.8.04 remains rollback. | DSP / Embedded |
| **JB-26** LCD corruption + animated skull | P0 | [Draft PR #17](https://github.com/balfourlynn1-byte/lxr-desktop-app/pull/17), v1.8.06; [GCC/Clang host, LCD state-machine simulation, ARM and SHA gates **PASS**](https://github.com/balfourlynn1-byte/lxr-desktop-app/actions/runs/42502110203). 560 B ITCM free. [**UNVERIFIED** test image](https://github.com/balfourlynn1-byte/lxr-desktop-app/actions/runs/42502110203/artifacts/13625738707), SHA256 `b959030387b89622470dfda288f1cf5096b4e0d41c39cf819f150bdce4091a49`. **Not yet device tested**. | Verify screenshot corruption gone after sleep/wake, actual skull glyphs, UI controls, 4-pad CPU; preserve v1.8.04 rollback. | Embedded / Verification |
| **JB-27** full upstream-style wavetable and waveshaping | P1 conditional | Requested with upstream [LXR02Open](https://github.com/brendanclarke/LXR02Open) + YouTube reference. Existing JIMBONES wave layer is only partial. Upstream **noncommercial restrictions conflict with GPL-only public distribution** if copied verbatim. | Obtain licence exception or independently implement original-oscillator waveform-ID interpolation/LFO scan, wavefold/shape, sample-domain evaluation, and test four-hit CPU cost; do not combine with skull bugfix. | DSP / Research |
| **JB-28A** one combined Comb+skull build | **P0 / Codex 18:30** | **HANDOFF READY, NOT YET IMPLEMENTED.** [Minimal Codex instructions](https://github.com/balfourlynn1-byte/lxr-desktop-app/blob/agents/jb-28-full-integration-v186/docs/CODEX_1830_HANDOFF.md), branch `agents/jb-28-full-integration-v186` from JB-26. Existing Comb PR #15 is CI green but separate. | Apply only Comb patch to current branch; host GCC/Clang, ARM, >=512 B ITCM, versioned SHA-pinned **v1.8.07 TEST ONLY**; draft PR. | Codex / Integration |
| **JB-28B** combined hardware validation | P0 / after JB-28A | **NOT STARTED.** Six native voices, kit compatibility, Comb, 5 dub timings, 16 Step FX toggles, wave UI, skull wake must coexist. | One-page physical test sheet, user records 4-hit DSP/PK/LATE/RAIL, no crunch, LCD corruption absent, verified rollback. | Verification / User |
| **JB-27** native oscillator wave-ID/LFO + waveshaping | P1 / after measured CPU | **NOT IMPLEMENTED.** JB-24 wave-first UI and native JIMBONES wave layer are partial, not full upstream feature parity. Upstream LXR02Open noncommercial licence limits source copying. | Original implementation or licence permission; isolate and benchmark four-hit cost. | DSP / Research |
| **JB-05** tape/tube saturation | P2 conditional | Backlog | Benchmark a cheap per-voice alternative, default off; do not add before live headroom proof | DSP |
| **JB-04** morph A/B/C | P2 conditional | Backlog | Preserve A and kick; spec edit/morph state and automation controls after stable build | DSP / Embedded |
| **JB-10/11/13** hi-hat/snare/visual polish | P2 | Backlog / partial diagnostics | Separate small PRs after core audio and Step FX validation | Embedded / DSP |
| **JB-06** 4-channel USB audio | P3 | Feasibility only | Hold until CPU, DMA, USB hardware budgets established | Research |
| **JB-08** Metallic FM | HOLD | **Compiled out of v1.8.04 DSP, and hidden from JB-24 UI** | No implementation; historical preset IDs preserved, not user-visible | — |

## What the wavetable branch does and does not do

- [JB-24](https://github.com/balfourlynn1-byte/lxr-desktop-app/pull/13) independently follows the **functional UI pattern** of [LXR02Open](https://github.com/brendanclarke/LXR02Open): waveform controls up front and a separate optional interpolation page.
- It does **not** paste or import LXR02Open's non-commercial source into this GPL-only public repository. The user's intended use is personal, but redistribution of code in public GitHub still requires a compatible licence or explicit permission. Existing native DSP wave tables remain.
- The actual LCD, knob sensitivity and audio result have **not** been tested on the physical synth. The CI checks verify source, host simulations, successful ARM link, a checksum and a >=512 B ITCM slack threshold; they do not measure audio ISR CPU utilisation.

## Next work: verify FM-free hardware headroom (JB-03 + JB-02 + JB-01)

**Goal:** verify the experimental package on the actual unit, using a separately verified known-working rollback image, and gather real DSP/PK/LATE/RAIL readings. JB-23 source and packaging are completed; the test is NOT.

**Read first:** [JB-23 rollback guide](https://github.com/balfourlynn1-byte/lxr-desktop-app/blob/agents/jb-23-from-waveui-v184/docs/agents/jb23_rollback_and_install.md), [hardware test sheet](https://github.com/balfourlynn1-byte/lxr-desktop-app/blob/agents/jb-23-from-waveui-v184/docs/agents/jb23_hardware_test_sheet.md), the exact source commit `3006145594c731ea842ca4301911ff116a99a045`, and all parent draft PRs.

1. Independently review parent PR chain (#8–#14), even though CI is green. A GitHub Actions download being available is not a flash recommendation.
2. Confirm the user retains their currently bootable image and backed-up SD kits **before** experimenting. Verify checksum of that rollback image and the new image.
3. User-supervised LXR-02 tests begin at low audio volume. Test original voices, output routes and Step 1 LED first; stop immediately on crashes, audible crunch, or increased late counters.
4. Record idle DSP/PK/LATE/RAIL after reset, 2 overlapping toms, 4 concurrent pad hits, wave interpolation, DUB SEND, clock disconnect, physical LEDs and output routing. v1.8.03 figures are historical reference only.
5. Treat v1.8.03 crunch measurements as historical, not a confirmed v1.8.04 defect. Metallic FM is disabled. JB-12 Comb now exists as **unmerged draft PR #15**, not a new hardware-approved image. Measure CPU load and 2-tom/four-pad stress on an explicitly backed-up candidate before merging/releasing Comb; defer new tape and oscillator engines until headroom is established.

**Release gate:** the user manually reports a stable boot with all six original voices, no new crackle or missed deadlines on four-hit stress, satisfactory headroom, and successful rollback. A Comb prototype exists in draft #15 but **has not met that hardware gate**; native oscillator waveform-ID/LFO interpolation is a separate JB-09B job.

**18:30 Codex launch:** Read [`docs/CODEX_1830_HANDOFF.md`](https://github.com/balfourlynn1-byte/lxr-desktop-app/blob/agents/jb-28-full-integration-v186/docs/CODEX_1830_HANDOFF.md). Start JB-28A only on `agents/jb-28-full-integration-v186`; transplant the existing Comb PR #15 changes into latest JB-26, run full gates and draft PR. Then stop. Earlier JB-19A instructions are completed and should **not** be restarted.

## What “fully functioning” means

**First integration milestone:** one candidate containing *all features currently coded* (parallel Step FX/dub, 5 timing buttons, wave-first layer, animated skull/LCD fix, and the Comb ninth FILTER Type) that builds and passes physical tests. This is **not** proof that all future requested ideas are implemented.

**Full feature-completion milestone (later):** separate tasks JB-27 native oscillator scan/waveshaping, JB-04 morph A/B/C, JB-05 tape/tube, JB-10 hi-hat, JB-11 snare random, JB-12B independent bass HPF/LPF, JB-07 advanced performance FX, and JB-06 USB feasibility. Each needs independent CPU/memory/physical gating; do not silently expand the 18:30 handoff to all of them.

