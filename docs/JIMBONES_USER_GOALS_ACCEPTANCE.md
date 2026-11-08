# JIMBONES — non-negotiable user goals and hardware-test acceptance
2026-11-08. This document is the authoritative goal check for the experimental integration branch; it does not imply the features below are implemented.

## Non-negotiable sound and compatibility
- Preserve all **six original LXR-02 voices**, their original sound, existing kits, and normal performance. No overclocking.
- Protect working v1.8.02 and maintain SD-card firmware-only recovery; no bootloader changes.
- Fix reported crunch when **two or more modulated sounds** overlap, including long decays and 3–4 simultaneous pad strikes.
- Keep audio timing and measurable CPU headroom central. Label render-only telemetry honestly; monitor actual deadlines.

## Playing setup and musical behavior
- Alesis Performance Pad struck with sticks, Roland KT-10 kick, Roland FD-8 hi-hat pedal, MIDI via Ableton on Intel Mac.
- FD-8 open/closed and choke with expressive velocity; future Sensory Percussion zones are deferred.
- Morph A/B/C: **A protected**, edit B/C without altering A; flashing B edit indicator; morph-position edit targeting; LFO-to-morph, velocity-to-morph, per-voice inclusion mask and kick exclusion.
- Per-hit seeded snare variation with probability control and backbeat protection; no mid-tail engine switching.

## Sound design and performance
- Metallic FM and wavetable oscillators **alongside** original engines; optional layering and velocity blend.
- Wavetable waveshaping and click-transient user samples where feasible without breaking compatibility.
- Per-voice tape/tube-like saturation with low-end preservation and velocity drive; do not substitute a global effect.
- Optional second bass HPF/LPF and comb filter.
- Tempo-synced step performance FX: 2/4/8-bar repeat patterns, step 1 all off, step 16 morph LFO, selectable kick exclusion.
- Straightforward double-press menu navigation, useful performance page and CPU monitor; visual waveform is lower priority than real-time audio safety.

## Candidate release gates
1. A deterministic host test for final mixer/post-FX peak and deadline monitoring, plus four overlapping voices.
2. Six-voice legacy behavior and old-kit load/save parity; no unwanted kick morph or saturation.
3. Host regression + ARM compilation for **the exact combined commit**; image size, memory and SHA verified.
4. Actual LXR-02 test: sticks, KT-10, FD-8, long tails, morph, saturation, repeat effects; no claim of bug-free before that.
5. Feature may be explicitly deferred if hardware budget or testing fails; do not silently change the user's target.

## Explicit scope exclusion
Four-channel USB multitrack audio is deferred from this candidate. Do not implement USB audio descriptors or routing.

## Engineering state
- Diagnostic fix commit d0bfc25e85bb069426d09d02d50475334d37c148: GitHub Actions run 42457959314 completed **host-regression success and firmware success**.
- DBG-01 verification branch agents/debug-telemetry-reset tracks CPU telemetry reset regression; no independent background agent is executing.
- Research branches and work orders are designs, not proof of finished code or device verification.
