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
                          +-- [NEXT / BLOCKED] JB-23 device-ready
                              |   test pack, rollback + checksum
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

**Review order:** #8 → #9 → #10 → #11 → #12 → #13. These PRs are stacked **on one another**, not six independently mergeable changes into `main`. Never merge or flash just because CI is green.

## Work queue and status

| Job | Priority | Status and proof | Next action | Owners |
|---|---|---|---|---|
| **JB-19A** FX send envelope | P0 | Source + host tests in [PR #8](https://github.com/balfourlynn1-byte/lxr-desktop-app/pull/8) | Independent integration review | DSP |
| **JB-19B** additive FX bus mixer | P0 | Host + ARM CI pass; [PR #9](https://github.com/balfourlynn1-byte/lxr-desktop-app/pull/9) | Review output-routing and race rules | DSP / Verification |
| **JB-21** Step FX toggles and LED | P0 | Host + ARM CI pass; [PR #10](https://github.com/balfourlynn1-byte/lxr-desktop-app/pull/10) | Actual physical Step 1 LED and 1–16 toggle test | Embedded / Verification |
| **JB-20** clocked dub delay | P1 | Host + ARM CI pass; [PR #11](https://github.com/balfourlynn1-byte/lxr-desktop-app/pull/11) | Test external clocks, physical FX output and wet tail | DSP / Verification |
| **JB-22** integration + FM-off | P0 | GCC/Clang, ARM + image validation PASS; **560 B free ITCM** in [run #42485178857](https://github.com/balfourlynn1-byte/lxr-desktop-app/actions/runs/42485178857); [PR #12](https://github.com/balfourlynn1-byte/lxr-desktop-app/pull/12) | Review the combined signal path, unchanged kit format and device risk | Integration / Verification |
| **JB-24** wave-first OSC UI | P1 | GCC/Clang + ARM + memory/image CI PASS in [run #42486888340](https://github.com/balfourlynn1-byte/lxr-desktop-app/actions/runs/42486888340); [PR #13](https://github.com/balfourlynn1-byte/lxr-desktop-app/pull/13) | Review 7 pages + interpolation on actual 16×2 display, preset legacy modes | Embedded / DSP |
| **JB-23** device test pack | P0 | Not implemented; no user-distributed v1.8.04 image | Prepare a checksum-pinned experimental artifact and rollback instructions **only after code review**; do not call it safe | Verification / Integration |
| **JB-03** overlapping-tom crunch | P0 | Reproduced on v1.8.03 device (DSP up to 101%, LATE 999); root cause not confirmed; Metallic DSP removed in later source | Test same kit and velocities on reviewed v1.8.04 device build, 2 toms and 4 pads | DSP / Verification |
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

## Ready-to-execute next Codex job — JB-23

**Goal:** a reviewable, rollback-safe **testing pack**, not a claim of stable firmware.

**Read only first:** this tree; `docs/agents/jb22_release_gate.md`; `docs/agents/jb24_waveui.md`; `tests/validate_image.py`; `.github/workflows/jimbones-jb24-waveui.yml`; the exact head of PR #13.

1. Verify and independently review complete parent PR chain (#8–#13). Resolve any unreviewed integration surprises before making an image public.
2. Create a test-only workflow that records exact git SHA, ELF/ITCM, firmware version, and SHA256, and uploads a distinctly labelled `UNVERIFIED` experimental image only when build and preflight gates are passing. Never overwrite the last bootable image.
3. Prepare an explicit device rollback checklist: backup kits and current working image, SD card steps, how to return to working firmware, and checksum comparison. All device claims are **pending hardware tests**.
4. Prepare a fillable hardware log: idle DSP/PK/LATE/RAIL after reset; 2 overlapping toms; 4 concurrent pad hits; wave interpolation OFF/ON; Step 1 dub throw; mono/pingpong; external clock disconnect; physical LEDs, output routing and feedback tails. Include original v1.8.03 numbers as **historical reference only**.
5. Stop at a draft PR for human review. **Do not automatically flash, merge main or call hardware stable.**

**Release gate:** the user manually reports a stable boot with all six original voices, no new crackle or missed deadlines on four-hit stress, satisfactory headroom, and successful rollback. Only then reconsider comb/wavetable expansion.

**Codex start prompt:** `Read docs/JIMBONES_JOB_TREE_V184.md and docs/agents/jb22_release_gate.md. Start JB-23 on a separate branch from PR #13. Add a checksum-pinned, rollback-safe experimental hardware test pack and no automatic release. Produce a draft PR with CI proof and list any device tests that remain outstanding.`
