# JIMBONES — LXR-specific ecosystem research (2026-11-09)

Research only. No firmware edits, builds, flashing, or third-party code imports. Strict ARM hold.

## Sources and why they matter

1. **LXR-02 community tools and firmware directory** — https://altstrument.org/devices/erica-synths-lxr-02
   - Lists the unofficial browser emulator from bg1eym, independent lxr02-recomp browser emulator, lxrpy file utility, web kit editor, and other firmware forks.
   - **Task:** investigate whether either emulator can execute *our changed native sources* rather than just a stock WASM build; document unsupported hardware interactions. Do not claim an uploaded .img runs in WASM without a compatible build.

2. **lxrpy** — find linked project via directory above.
   - Python tools for LXR-02 kit/pattern/song/global files.
   - **Task:** establish an independent read-only fixture/round-trip verifier for saved .SND/kit and globals, especially BPM/kit state and SD rollback. Do not write user SD media until test fixtures prove safety.

3. **Brendan Clarke's LXR02Open** — https://github.com/brendanclarke/LXR02Open
   - Contains DSP_AUDIT.md, OSC_INTERP_AUDIT.md, MEMORY.md, hardware map and alternate firmware.
   - **Task:** compare algorithmic strategies for wavetable interpolation, per-voice memory, and audio timing. This project is a different port/architecture and its custom licence includes noncommercial conditions; do not copy code into GPL project without legal review.

4. **ARM CMSIS-DSP interpolation** — https://arm-software.github.io/CMSIS-DSP/v1.12.0/group__BilinearInterpolate.html
   - Supports fixed-point Q7/Q15/Q31 and floating-point bilinear interpolation. Could serve as a reference for phase+scan interpolation across adjacent wavetable frames.
   - **Task:** benchmark current two-axis interpolation vs small inline implementation and CMSIS reference using deterministic PCM and aliasing/DC/click metrics; account for out-of-range behavior (CMSIS returns zero) and wrap-around at phase boundaries. Do not replace current path without measurable sound quality and CPU benefit.

5. **Erica Synths official source and safety notice** — https://www.ericasynths.lv/news/post/lxropen/ and https://github.com/erica-synths/lxr-desktop-app
   - Original firmware is GPL-3.0, modified firmware unsupported by manufacturer. Keep all changes isolated and preserve known-good bootloader; never run full flash target.

## Priority experiments
- E01: Verify whether bg1eym and gregsadetsky emulators can load a rebuilt experimental branch or expose audio render hooks; record limits and reproducible commands.
- E02: Explore lxrpy parser for kit round-trip checks and corrupted/truncated save recovery; fixture-only.
- E03: Review LXR02Open DSP/interpolation audits for test ideas; do not copy source.
- E04: Evaluate CMSIS interpolation as a **measured candidate** after end-to-end host PCM harness exists.
- E05: Keep existing strict ARM hold; update work queue/checkpoint and push each atomic change.

This is a research plan, **not a report of tests executed**.
