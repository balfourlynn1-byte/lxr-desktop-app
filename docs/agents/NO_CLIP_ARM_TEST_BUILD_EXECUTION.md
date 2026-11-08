# JIMBONES No Clip — testable ARM candidate execution plan

**Goal:** reproduce and eliminate unwanted crunch with 2–4 simultaneous modulated voices, including long decays and retriggers. Preserve all six native voices, dry bypass, kits and bootloader. No overclocking.

**Evidence baseline:** GitHub Actions run 42462536809 passed host-menu, host-dsp and arm-compile. This is not a physical No Clip pass.

**Workstream model:** JB-31–42 are task owners/work orders, **not independently executing autonomous agents**. A work order alone is 0% implementation. Each change must be coded, tested and reviewed.

## Ordered gates

- **Gate A — Baseline:** exact SHA, existing clean six-voice audio, original crunch reproducible and recorded.
- **Gate B — Instrument:** layer/native/FX/output peak & rail telemetry plus CPU deadline measurements; no audio behaviour changes.
- **Gate C — Reproduce:** deterministic 2/3/4-voice fixtures with velocity, long tails, modulation and retriggers; fail before fix where applicable.
- **Gate D — Correct:** smallest measured DSP/gain fix, no new limiter by default; bypass remains bit-exact and CPU headroom adequate.
- **Gate E — Verify:** native menu, production DSP host tests, ARM compile/link, memory checks, independent review.
- **Gate F — Hardware:** backup existing working firmware/kits, load engineering image by SD only with explicit user approval, record 2/3/4 voice Alesis performance, check pops/crunch and CPU, rollback on fault.

## Atomic work orders

| ID | Work | Dependencies | Review | Definition of done |
|---|---|---|---|---|
| JB-31 | Native mixer trace | Gate A | JB-21, JB-27 | Committed source/tests, reproducible evidence, independent review |
| JB-32 | Rail and peak counters | Gate A | JB-27 | Committed source/tests, reproducible evidence, independent review |
| JB-33 | CPU deadline probe | Gate A | JB-22, JB-28 | Committed source/tests, reproducible evidence, independent review |
| JB-34 | Two-voice reproducer | Gate B | JB-23, JB-29 | Committed source/tests, reproducible evidence, independent review |
| JB-35 | Three/four-voice stress | Gate B | JB-23, JB-29 | Committed source/tests, reproducible evidence, independent review |
| JB-36 | Transition click tests | Gate B | JB-18, JB-29 | Committed source/tests, reproducible evidence, independent review |
| JB-37 | Gain-stage fix | Gate C | JB-03, JB-21 | Committed source/tests, reproducible evidence, independent review |
| JB-38 | Performance optimization | Gate C | JB-26, JB-28 | Committed source/tests, reproducible evidence, independent review |
| JB-39 | Host regression gate | Gate D | JB-29, JB-30 | Committed source/tests, reproducible evidence, independent review |
| JB-40 | ARM memory and build | Gate D | JB-24, JB-30 | Committed source/tests, reproducible evidence, independent review |
| JB-41 | Hardware test protocol | Gate E | JB-20, JB-23 | Committed source/tests, reproducible evidence, independent review |
| JB-42 | Release decision | Gate E | JB-20, JB-30 | Committed source/tests, reproducible evidence, independent review |

## Stop conditions
Any unexplained full-scale rail, discontinuity, missed audio deadline, broken dry bypass, unexpected preset mutation, ARM link overflow or independent review blocker stops the test release. Never claim a physical pass from host CI alone.

## First implementation slice
1. JB-31 map real native audio pipeline and locate output conversion.
2. JB-32 extend diagnostics to the real mixer path without altering sound.
3. JB-34 create a two-voice regression that reproduces the issue before attempting a correction.
4. JB-33 add deadline telemetry; only then decide whether the cause is gain or CPU.

## Parallel Step FX implementation (must not delay No Clip)

**Verified source state (2026-11-08):** `Core/Src/Soundengine/StepFX.c` schedules bounded MIDI retriggers; `Core/Src/menu/JimbonesPerfUI.c` selects 16 slots; `Core/Src/menu/menu.c` exposes the page and encoder edits. No dedicated dub-delay audio-send/throw DSP path was identified in these modules. A step-button press-to-toggle path was not verified. Do not mark these as implemented.

- **JB-43 Step-button event trace:** identify actual hardware step-key dispatch and confirm precedence with sequencer; produce event-flow evidence.
- **JB-44 Toggle UI state:** one press enables selected slot, second disables it; slot 1 is all off, never confuse index 0 with physical key 1; verify LED state, page transitions and kit switching.
- **JB-45 Dub-delay DSP design:** bounded tempo-synced delay buffer, feedback, send level, return level and CPU/RAM budget; distinguish MIDI retriggers from audio delay.
- **JB-46 Send/throw routing:** per-voice send gates, kick exclusion by default, step-key throw state and feedback tails that survive send-off; guard against feedback runaway.
- **JB-47 Step FX tests:** host key-toggle/LED, bypass parity, clock sync, delay tail, feedback ceiling, 2–4 voice No Clip stress and ARM memory checks.

**Ordering:** JB-43 → JB-44; JB-45 → JB-46 → JB-47. Do not merge audio delay into No Clip test build before its own CPU, memory, regression and review gates pass. All are work orders, not autonomous workers.
