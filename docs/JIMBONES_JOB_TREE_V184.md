# JIMBONES v1.8.04–1.8.06 — build job tree and Codex handover

**Updated:** 2026-11-08. **Purpose:** make jobs small and ready for Codex, with clear dependencies and test gates. 
**Status of this programme:** v1.8.04 has user-reported stable boot, ~62% DSP and no new audible crunch; new 1.8.05–1.8.06 changes remain **untested on hardware**. Exact PK/LATE/RAIL and four-pad stress on v1.8.04 are still unrecorded. CI does not prove realtime headroom. Branch roles in this board are **virtual assignments**, not autonomous active agents. CI passing is not a substitute for physical tests.

## Active dependency tree

```text
BASE: known-good LXR-02 firmware (keep untouched + backed up)
  |
  +-- [CI PASS / REVIEW] JB-19A Step FX gated send state      draft PR #8
      |
      +-- [CI PASS / REVIEW] JB-19B parallel dry + FX mixer  draft PR #9
          |
          +-- [CI PASS / REVIEW] JB-21 16-button ON/OFF UI,
              |                Step 1 DUB SEND and LEDs      draft PR #10
              |
              +-- [CI PASS / REVIEW] JB-20 tempo dub delay  draft PR #11
                  |
                  +-- [CI PASS / REVIEW] JB-22 integrated
                      |    v1.8.04; Metallic FM disabled;
                      |    image format and ITCM gate        draft PR #12
                      |
                      +-- [CI PASS / REVIEW] JB-24 wave-first
                          |    OSC menu, FM controls hidden   draft PR #13
                          |
                          +-- [CI PASS / REVIEW] JB-23 experimental
                              |   test pack, rollback + checksum   draft PR #14
                              |       |
                              |       +-- JB-03 actual DSP stress
                              |       +-- JB-02 meter verification
                              |       +-- JB-01 baseline/rollback
                              |
                              +-- [CI PASS / REVIEW] JB-25 5 dub
                              |   interval pads v1.8.05         draft PR #16
                              |       |
                              |       +-- [CI PASS / REVIEW] JB-26
                              |           LCD byte-latch + bouncing skull
                              |           v1.8.06               draft PR #17
                              |           |
                              |           +-- [NEXT / CONDITIONAL] JB-27
                              |               upstream-style waveform-ID
                              |               scanning + shaping DSP
                              |               (licence + CPU review first)
                              |
                              +-- [AFTER CPU PASS] JB-09B native
                              |   oscillator wavetable / LFO
                              |   scan feasibility + profile
                              +-- [CI PASS / REVIEW] JB-12 Comb
                              |   FILTER Type prototype         draft PR #15
                              +-- [DEFERRED] other sound engines
```

**Review order for the current test-image chain:** #8 → #9 → #10 → #11 → #12 → #13 → #14 → #16 → #17. These PRs are **stacked**, not independently mergeable into `main`. **Comb PR #15 is a separate branch and is NOT included in the v1.8.06 skull candidate.** Never merge or flash just because CI is green.

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
| **JB-26** LCD corruption + animated skull | P0 | [Draft PR #17](https://github.com/balfourlynn1-byte/lxr-desktop-app/pull/17), v1.8.06: guarded LCD ownership, latched nibble writes, 10×12 skull bouncing vertically, host UI simulation + ARM gate. **Hardware test not yet performed**. | Verify screenshot corruption is gone after sleep/wake, actual skull glyphs, UI controls and CPU. | Embedded / Verification |
| **JB-27** full upstream-style wavetable and waveshaping | P1 conditional | Requested with upstream [LXR02Open](https://github.com/brendanclarke/LXR02Open) + YouTube reference. Existing JIMBONES wave layer is only partial. Upstream **noncommercial restrictions conflict with GPL-only public distribution** if copied verbatim. | Obtain licence exception or independently implement original-oscillator waveform-ID interpolation/LFO scan, wavefold/shape, sample-domain evaluation, and test four-hit CPU cost; do not combine with skull bugfix. | DSP / Research |
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

**Codex next-start prompt:** `Read docs/JIMBONES_JOB_TREE_V184.md and PR #14. Use the existing JB-23 checksum-pinned artifact and device checklist. Don't assume hardware tests have been done; when actual device readings arrive, compare them to historical v1.8.03 baseline, identify DSP bottlenecks, and make separate minimal reviewable PRs. Preserve backup/rollback.`
