# JIMBONES v1.8.04 — build job tree and Codex handover

**Updated:** 2026-11-08. **Purpose:** make jobs small and ready for Codex, with clear dependencies and test gates. 
**Status of this programme:** experimental source; **no approved hardware build or confirmed DSP headroom**. Branch roles in this board are **virtual assignments**, not autonomous active agents. CI passing is not a substitute for physical tests.

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
                              +-- [AFTER CPU PASS] JB-09B native
                              |   oscillator wavetable / LFO
                              |   scan feasibility + profile
                              +-- [AFTER CPU PASS] JB-12 comb
                              |   filter prototype and budget
                              +-- [DEFERRED] other sound engines
```

**Review order:** #8 → #9 → #10 → #11 → #12 → #13 → #14. These PRs are stacked **on one another**, not six independently mergeable changes into `main`. Never merge or flash just because CI is green.

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
| **JB-03** post-FM-removal DSP verification | P0 | **Historical issue on metallic-enabled v1.8.03 only** (DSP up to 101%, LATE 999). Metallic FM render code is compiled out in v1.8.04; there is **no evidence yet** of this crunch on the new build. | Record like-for-like v1.8.04 idle/2-tom/4-pad DSP, PK, LATE and RAIL. **Only open an optimisation/fix job if it recurs.** | Verification (DSP conditional) |
| **JB-02** DSP/PK/LATE/RAIL meter | P0 | Present in experimental firmware and hardware readings on earlier build | Confirm semantics and reset behavior on new image; compare idle and loaded cases | Embedded / Verification |
| **JB-01** baseline parity and rollback | P0 | Source and CI regressions exist; user has booted prior experimental firmware | Verify six original voices, physical outputs, kits and recoverable rollback on device | Integration |
| **JB-09B** further wavetable integration | P1 conditional | JB-24 uses existing JIMBONES wave tables/interpolation, **not** upstream sample/loop engine or its LFO wave-ID scan | Research independent LFO-to-waveform modulation and CPU usage; design separate minimal PR **after measurements** | DSP / Research |
| **JB-12** comb / independent bass filter | P1 conditional | Initial feasibility backlog, no integrated comb implementation in current candidate | Audit short-delay memory and stability; then bounded prototype if CPU allows | DSP / Research |
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
5. Treat v1.8.03 crunch measurements as historical, not a confirmed v1.8.04 defect. Metallic FM processing is removed. File a new DSP optimisation/fix work order only if fresh firmware readings reproduce overload; hold expensive comb/tape/new oscillators until headroom is established.

**Release gate:** the user manually reports a stable boot with all six original voices, no new crackle or missed deadlines on four-hit stress, satisfactory headroom, and successful rollback. Only then reconsider comb/wavetable expansion.

**Codex next-start prompt:** `Read docs/JIMBONES_JOB_TREE_V184.md and PR #14. Use the existing JB-23 checksum-pinned artifact and device checklist. Don't assume hardware tests have been done; when actual device readings arrive, compare them to historical v1.8.03 baseline, identify DSP bottlenecks, and make separate minimal reviewable PRs. Preserve backup/rollback.`
