# JIMBONES — cross-agent research synthesis (USB audio excluded)
2026-11-08. Research is a design handoff, not evidence of implemented features or independently executing agents.

## External source assessment
- Official Erica Synths firmware (https://github.com/erica-synths/lxr-desktop-app) establishes STM32F765 216 MHz, 16 KB ITCM, 128 KB DTCM, 384 KB RAM, 992 KB app flash, explicit CMake source lists, and SD firmware-only update. Preserve bootloader and original six-voice behavior.
- LXR02Open (https://github.com/brendanclarke/LXR02Open) documents asynchronous hardware queues, CPU monitor and oscillator interpolation as **design references only**. Its licensing differs from the official GPL tree: do not copy code until compatibility and permission are confirmed.
- Community feature inventory (https://altstrument.org/devices/erica-synths-lxr-02) identifies existing beat-repeat, DJ filter and CPU-meter approaches. Compare behavior, not unverified performance.

## Pair deliverables and why they help the combined build
| Workstream | Researched solution | Contribution | Release gate |
|---|---|---|---|
| JB-01 baseline | Freeze working v1.8.02 image/source and hashes, kit backups, rollback; compare exact app layout | Recoverability and compatibility | User boot and kit test |
| JB-02 CPU | DWT render-only timing, existing AudioDiagnostics, deadline counters, queue only with verified signal | Performance budget for all effects | Integrated CI + 4-pad benchmark |
| JB-03 crunch | Compare isolated layer, final sum and post-FX rail hits vs deadline misses | Identify clipping vs CPU overload before changing sound | Before/after audio and hardware |
| JB-04 morph | Immutable A, separate B/C, per-voice masks, smoothed continuous params and trigger-boundary discrete changes | Safe expressive morph with kick exclusion | Kit roundtrip + click tests |
| JB-05 tape | Optional per-voice bounded asymmetric saturation with DC blocker and equal-loudness mix | Analog-like color without global distortion | Spectrum, CPU, bass response |
| JB-07 step FX | Tempo-clocked retrigger first, then small bounded delay buffer; kick excluded | Lower-RAM beat repeat and performance steps | Clock, feedback, CPU and RAM |
| JB-08 metallic FM | Profile sample loop, cache trigger coefficients, preserve existing modes | More simultaneous FM voices | CPU and alias benchmarks |
| JB-09 wavetable | Selectable interpolation/bandlimiting, no large new banks initially | Reduced aliasing with controllable CPU cost | License, spectrum, memory |
| JB-10 hi-hat | CC4/note state machine, short choke release, configurable mapping | FD-8 expression | MIDI replay + hardware |
| JB-11 snare random | Seeded per-hit PRNG, no mid-tail changes, backbeat guard | Repeatable organic snare | Seed and edge-case tests |
| JB-12 filters | Stable opt-in bass biquad, bounded comb only if memory allows | Additional sound shaping | Stability, CPU, memory |
| JB-13 UI | Snapshot render-load/peak/overruns outside audio ISR | Usable CPU feedback | Menu tests and device display |

## Integration order and dependency graph
1. **Gate A/B**: JB-01 + JB-02 + JB-03, fix any proven multi-voice regression. No feature merge before measured baseline.
2. **Gate C**: JB-10 (MIDI choke), JB-04 (morph storage and smoothing), JB-11 (snare PRNG), JB-13 (diagnostic UI).
3. **Gate D**: JB-08 + JB-09 CPU optimisation before JB-05 saturation and JB-12 filters.
4. **Gate E**: JB-07 step FX only after timing budget and RAM are measured.
5. **Combined candidate**: one PR at a time, run host and ARM after each, compare map and code size, image checksum, test hardware and rollback.
6. **Explicitly excluded**: JB-06 four-channel USB audio, its descriptors, routing and related feature work.

## Risks and controls
- CPU headroom: preserve all six voices and 3–4 simultaneous pad hits, worst-case long decays. Gate new modes opt-in.
- Audio clipping: summed int16 test alone is not final hardware mixer; measure after FX.
- Parameter jumps: avoid interpolating enums, use trigger-boundary swaps and smoothing for continuous parameters.
- RAM: avoid multi-bar audio buffers until quantified; use event retrigger where possible.
- Storage: version and preserve old kits; test roundtrip.
- License: do not transplant non-GPL code from external firmware without review.
- Safety: SD card application updates only; no bootloader flashing.

## Current engineering status
- Combined diagnostic integration at commit 1790aec passed GitHub Actions run 42457405607 (host + ARM).
- Agent branches contain proposed research solutions, not newly implemented effects. Existing code in the integration branch may already contain partial versions of effects; audit actual symbols before writing replacements.
- Research briefs are updated per agent branch; they are not autonomous agents or independently validated external reports.

## Immediate code tickets
A. JB-03: add final mixer/post-FX peak and deadline fixture.
B. JB-02/JB-13: expose snapshot counters in menu with truthful render-only label.
C. JB-10: implement host-tested CC4 choke state machine with default disabled.
D. JB-04: implement version-safe A/B/C snapshot storage with kick mask, then LFO.
E. JB-08/09: measure baseline cycles before sound engine optimisations.
