# JIMBONES — pre-ARM independent review gate

**Status: BLOCKED — do not produce/flash new ARM candidate.** 2026-11-08. Source inspection by assistant; **not independent agent peer reviews**. PR #7 currently has zero GitHub review submissions, zero review threads, and no PR-triggered workflow run returned for the latest reviewed commit. These checks are not a substitute for complete CI or physical tests.

## Review matrix
| Reviewer / workstream | Source finding | Gate |
|---|---|---|
| JB-13 UI | `Core/Src/menu/menu.c` extension router supports MOD/FM/CLICK/FILTER/MIX placeholders; OSC uses separate existing cycle. Most feature pages only say PENDING, not working parameter code. | BLOCK feature-complete claim |
| JB-14 CLICK | `jb_extPot` wires attack/sustain with [-64,64] clamp to `jb_setTransient`. Temporary cache reset depends on `jb_kitRevision`. Actual 16x2 physical interaction and reload behaviour untested. | NEED host + device |
| JB-18 LOFI | `jb_setSampleRateReduction` resets sampleHold.counter on factor changes. Risk of abrupt sample transition; click/pop-free automation is unproven. SRR intentional coloration is allowed only when selected. | P0 AUDIO REVIEW |
| JB-01 / storage | New UI controls not serialized into versioned kits; loading a kit clears DSP transient/SRR and increments revision. UI synchronization is present but unproven under real UI reload. | BLOCK saved-kit feature completion |
| JB-02 CPU | No current CI/ARM run returned for this review commit. Existing CPU telemetry cannot substitute for measured hardware audio deadline results. | NEED CI + physical |
| JB-03 No Clip | Prior user-observed two-or-more modulated voices crunch has no documented hardware PASS; no measured final DAC/output clipping evidence in this review. | **P0 BLOCK** |
| JB-04/05/07/08/09/10/11/12/15/16/17/19 | Agent branches contain work-order handoffs, not completed implementation PRs. Pending UI screens must not enable DSP. | BLOCK feature-complete claim |
| JB-20 integration | GitHub PR #7 has no recorded peer approvals. Host menu test extracts only `menu_switchSubPage` and stubs `jb_extSyncKit`; it cannot validate actual display/knob/kit integration. | **BLOCK ARM promotion** |

## Required fixes before building new ARM candidate
1. Run host tests against exact commit, including real production DSP and menu fixture; fix all compile/test failures.
2. Add end-to-end UI tests for native button reselect, repeated press cycling, voice switching, 16x2 labels, knob ranges and kit load reset. Remove stale/contradictory TEMP behaviour; either persist versioned controls or explicitly defer shipping.
3. JB-18 test abrupt factor changes during long tails, multi-voice SRR bypass transitions and output peaks; mitigate audible pops if found.
4. JB-03 reproduce and diagnose original overlapping modulation crunch with 2/3/4 concurrent pad voices and long decays; instrument pre/post FX and final mixer rails and CPU deadlines.
5. JB-13 review shared menu conflicts; JB-20 collect **actual** independent code review approvals and host/ARM CI evidence, then prepare hardware listening checklist. Only after source + CI gates may an ARM candidate be built, and only physical hardware tests may establish NO CLIP PASS.

**Current result:** source review conducted, no independent agent reviews, no green latest CI, **NO CLIP UNVERIFIED**. Existing working firmware remains untouched.

## Fix pass 1 — 2026-11-08
- **JB-18 live LOFI transition:** `Core/Src/Soundengine/DSPAudio/Jimbones.c` no longer forcibly zeroes the hold counter when SRR changes; each voice now blends between dry and held samples with a bounded 64-sample wet/dry ramp. This addresses one potential abrupt bypass discontinuity, but does **not** establish click-free factor changes or clean final output.
- **Host regression:** `tests/test_jimbones.c` now exercises active voice SRR 0 → 16 → 0 → 32 → 0 during rendering. This is a smoke test, not a quantified pop or No Clip assertion.
- **Kit-load UI mismatch:** existing revision counter and UI reset changes remain in place; must be verified by exact-commit host and device tests.
- **Outstanding blockers unchanged:** no independent peer approval, no verified latest host/ARM CI, no physical audio No Clip PASS, no persistence for temporary controls, no proven fix for the original overlapping modulated voice crunch. **DO NOT BUILD OR FLASH NEW ARM IMAGE YET.**
