# CODEX 18:30 UK — JIMBONES full-build handoff (2026-11-08)

**Read this file first.** Minimal context. The user wants **one fully functioning LXR-02 firmware build** containing the already implemented features **including the Comb filter**, not another disconnected prototype. Prioritise completing, verifying and packaging existing work; do not restart old jobs or rewrite code that has already passed CI. No automatic flash/merge.

## Exact starting point

- Repo: `balfourlynn1-byte/lxr-desktop-app`
- **Work branch:** `agents/jb-28-full-integration-v186` — created from the **latest skull/LCD** branch `agents/jb-26-skull-screensaver-v185`, head `21d7c7e74a89909c122d3edb8cc37030f4c8473a`, [draft PR #17](https://github.com/balfourlynn1-byte/lxr-desktop-app/pull/17).
- The current source branch already includes PRs **#8 → #9 → #10 → #11 → #12 → #13 → #14 → #16 → #17**: parallel dry+FX send, Step FX toggles/LED, tempo dub delay, Metallic FM disabled, JIMBONES wavetable layer + wave-first UI, test pack, 5 dub timings, animated skull + LCD ownership/byte fixes.
- v1.8.06 skull [host GCC/Clang + ARM + updater checksum PASS](https://github.com/balfourlynn1-byte/lxr-desktop-app/actions/runs/42502110203), **560 B ITCM free**; [UNVERIFIED 1.8.06 test artifact](https://github.com/balfourlynn1-byte/lxr-desktop-app/actions/runs/42502110203/artifacts/13625738707). Do not claim actual LCD or audio testing.
- **Comb exists already** in [JB-12 PR #15](https://github.com/balfourlynn1-byte/lxr-desktop-app/pull/15), branch `agents/jb-12-comb-filter-v184`, commit `cb27f684a108a3fe257ac4e4dfe437a041fbe40b`, based on older JB-23, **not yet in JB-26**. [Its host/ARM CI passed](https://github.com/balfourlynn1-byte/lxr-desktop-app/actions/runs/42495034632), **528 B ITCM free on that OLD branch**. Combined branch memory and sound are **unknown**.
- Historical user v1.8.04 device report: boots, ~62% DSP, previous crunch no longer heard; dub echo sounds good. RAIL increments only while sequencer plays, not idle. No equivalent v1.8.06 or comb physical readings. Old Metallic FM v1.8.03 reached 94–101% DSP and crunched; **keep Metallic FM OFF**.
- User has Mac Intel, Alesis pad, Roland kick and hi-hat, and typically plays **3–4 simultaneous hits**. Six native voices, factory presets and dry routing must remain untouched with new features OFF.

## 18:30 handoff — ONLY ONE IMPLEMENTATION JOB FIRST

**JB-28A — integrate the existing Comb into the latest skull branch.**

1. On `agents/jb-28-full-integration-v186`, transplant the **Comb-specific changes only** from PR #15; don't merge its older branch wholesale (that would risk overwriting JB-25/JB-26). First inspect `git diff agents/jb-23-from-waveui-v184..agents/jb-12-comb-filter-v184 --` and the exact 12 changed paths in PR #15. Copy/adapt: `CombFilter.c/.h`, `ResonantFilter.c/.h`, `MenuText.h` ninth FILTER Type, `Core/CMakeLists.txt`, `tests/run_host.sh`, `tests/run_jb12_comb.sh`, `tests/test_jb12_comb.c`, `tests/test_jb12_comb_source.py`. Keep the JB-26 LCD code, the JB-25 button mapping and the 1.8.06 test packaging untouched except the next version bump.
2. Retain filter IDs **0–7** exactly, append **Comb as index 8**; use existing FILTER Freq/Reso/Drive, six independent 256-sample rings, no allocations or new global delay; Reso=0 exact bypass. Native factory FM parameters are **not** the shelved Metallic FM and must stay.
3. Add `sh tests/run_jb12_comb.sh` to current `tests/run_host.sh`; add source to `Core/CMakeLists.txt`. Build GCC/Clang host, Step FX, ARM and updater image. **Do not weaken** >=512 B free ITCM or any test. New combined ITCM may fall below the limit: optimise/move *control-rate* functions to flash, never remove protection or overwrite audio DSP kernels to force a green test.
4. Version candidate **v1.8.07**, distinct from the prior 1.8.04 known-working rollback and 1.8.06 experimental test image. Update all version-specific references in the new workflow and package script, not just the filename. Publish **SHA-pinned, hardware-unverified TEST ONLY** image only after complete CI passes.
5. Open **one draft PR** for JB-28A based on `agents/jb-26-skull-screensaver-v185`; report commit, changed paths, tests, RAM/ITCM, artifact checksum, residual hardware risks. Stop if any regression, memory gate or integrity test fails. Do not claim physical CPU improvement.

**Avoid reading the entire repo.** Required files: [Comb design](https://github.com/balfourlynn1-byte/lxr-desktop-app/blob/agents/jb-12-comb-filter-v184/docs/agents/jb12_comb_filter.md), [LCD/skull design](https://github.com/balfourlynn1-byte/lxr-desktop-app/blob/agents/jb-26-skull-screensaver-v185/docs/agents/jb26_skull_and_lcd.md), current `Core/CMakeLists.txt`, `tests/run_host.sh`, `Core/Src/Soundengine/DSPAudio/ResonantFilter.c`, `Core/Inc/Soundengine/DSPAudio/ResonantFilter.h`, `Core/Inc/menu/MenuText.h`, and PR #15 patch. Read further only to resolve a concrete failure.

## Approved follow-on UI job — JB-29 (DO NOT MIX INTO JB-28A)

The user confirms existing Step FX effects work; they lack musical usefulness due to nonselective pitch up/down and no per-voice routing. **Preserve working Step FX and dub audio engines.** Full specification: [JB-29 four-knob UI and voice-group selector](https://github.com/balfourlynn1-byte/lxr-desktop-app/blob/agents/wavetable-research/docs/JIMBONES_JOB_TREE_V184.md#jb-29-ui-specification--approved-design-2026-11-08).

- **Knob 1 VOICE:** OFF, SNR (snare+clap), TOM (tom1+tom2), HAT, S+T, S+H, T+H, ALL. Kick protected/excluded. SHIFT+Knob1 advanced six-bit per-voice mask for snare-only, clap-only, any combination. Confirm actual voice IDs from source.
- **Knob 2 RATE**, **Knob 3 DEPTH**, **Knob 4 CHAR** (pitch range / dub feedback / retrigger variation according to slot). Four jobs remain consistent across all effects.
- Each of **16 Step FX slots** recalls its own voice mask and knob settings; avoid value jumps based on actual encoder/pot hardware. Keep backward kit compatibility, existing five dub timings, existing dry audio and LEDs.
- **JB-29A:** minimal selector+mask+send/trigger gating only, with eight preset + advanced mask + per-slot recall + dry/kick + 4-hit tests. **JB-29B:** effect-specific RATE/DEPTH/CHAR and improved bounded pitch-up/down controls only after JB-29A validation. Do not label accepted without user device confirmation.
- **Do not implement this in the current Comb integration job**; it is next after the combined candidate is available, to keep the Codex workload small and regressions isolatable.

## AFTER JB-28A green — separate small tasks

**JB-28B — full candidate regression + hardware-test sheet (P0, no new sound engine).** Combine six original voices, default-off dry parity, Step FX 1–16 on/off + LED1, 5 dub timing buttons, global FX wet tails, Comb on/off + all six voices, native wavetable, skull wake, kit save/load and rollback. Run GCC+Clang, 4-hit host stress, ARM, memory and image checksum. Produce a 10-minute **physical test checklist** and safe rollback instructions; user performs the actual LXR-02 test and supplies DSP/PK/LATE/RAIL at idle, two toms, four simultaneous pads, Comb active and echo active. **Do not declare fully functioning until physical pass.**

**JB-27 — genuine native oscillator waveform-ID/LFO wavetable and waveshaping (P1, independent DSP PR AFTER measured headroom).** JB-24 already provides a wave-first UI and a native JIMBONES wavetable layer, but **does not fully port** upstream [LXR02Open](https://github.com/brendanclarke/LXR02Open) native oscillator modulation or sample/loop engine. The upstream licence has **non-commercial terms**, so do not copy upstream code into GPL-only public JIMBONES without explicit permission; write an original, independently tested implementation or secure permission. Preserve original six voice modes and saved kit IDs, default OFF; quantify CPU and audio output for 4 simultaneous hits. Avoid adding new wave-table banks if current `wavetable.c` suffices. This is **not** required to complete JB-28A.

**Remaining backlog — do not quietly claim complete:** JB-04 morph A/B/C (A protected; per-voice/kick exclusion; LFO and velocity), JB-05 per-voice tape/tube, JB-10 hi-hat articulation, JB-11 snare per-hit random, JB-12B independent bass HPF/LPF, JB-07 advanced 2/4/8-bar performance Step FX, JB-06 USB four-channel feasibility. Each is a **separate feature**, not yet implemented and not a prerequisite for the existing feature integration candidate. Only schedule them after actual DSP headroom is measured; USB may be physically infeasible. Never describe all requested features as implemented before those jobs are completed and verified.

## Acceptance / stop conditions

- Default bypass preserves original audio/6 voices/kit data, dry kick protected; no duplicated DSP FX, no dynamic allocation in ISR.
- Existing Comb and skull fixes **both present in the same image**; physical FILTER shows 9 types, with Comb ninth.
- Full host+ARM CI green, ITCM free >=512 B, verified checksum, image SHA and matching source SHA, explicit `HARDWARE_UNVERIFIED` label.
- Hardware pass requires audible Comb on all six voices, safe feedback, four-pad stress, no new crunch or late counters, 5 dub timing pads, proper LCD wake and recoverable rollback.
- No merges to `main`, GitHub Releases, device flashes or unsupported claims. Do not modify historical good image.

## Copy-paste Codex launch prompt

> Continue JIMBONES in `balfourlynn1-byte/lxr-desktop-app` on `agents/jb-28-full-integration-v186`. Read ONLY `docs/CODEX_1830_HANDOFF.md` and the JB-12 comb design it links. Do **JB-28A only**: transplant the already-tested Comb filter from PR #15 into the latest JB-26 skull/LCD source, retaining the five dub timing buttons and all existing fixes. Append Comb as ninth FILTER Type without changing old preset IDs. Integrate tests, build GCC/Clang host + ARM, require >=512 B ITCM free, validate a v1.8.07 test-only image, open a draft PR and report exact SHA, checksum, memory and risks. Preserve rollback and do not claim hardware validation. Then stop for review.
