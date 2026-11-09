# JIMBONES experiments — 2026-11-09

## X01: Host-only relative editing, actually executed
Source: `tests/test_jb_shift_relative_host.c`.
Command executed locally: `cc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined /tmp/jb_relative.c -o /tmp/jb_relative && /tmp/jb_relative`, using identical source copied into the repository test file.
**Observed:** `PASS: relative offsets, exclusions, positive/negative deltas, saturation`.
Tested six initial values [20,45,60,35,75,90], +10 to all (max 100), then -15 excluding voice 1; tested bounds and larger deltas. Host-only algorithm proof; **no LXR DSP, parameter routing, kit persistence, emulator, ARM or hardware test**. Next run the checked-in file in CI or a repeatable host test job.

## X02: New LXR-specific implementation to investigate
The LXR QEONDA firmware listed at https://altstrument.org/devices/erica-synths-lxr-02 advertises **change-all knob edits**. Inspect its source and licensing for SHIFT gating, parameter compatibility, per-voice update logic and encoder acceleration. Do not copy unverified code or assume its feature has identical relative-delta semantics.

## X03: Independent file format validation lead
https://github.com/jhw/lxrpy documents LXR-02 .SND (255 bytes), .PAT (3663 bytes) and GLO.CFG (30 bytes), with per-voice parameters including flt_frq, flt_res, flt_drv, aeg_dec and osc_coa. **Important:** its docs describe BPM in GLO.CFG as project-wide, not per-kit; user's desired per-kit BPM requires an explicit compatibility-safe design, not an assumption that stock files store it in each kit. The library is MIT licensed. Future experiment: generate disposable fixture files, round-trip with lxrpy, compare unknown bytes and invalid/truncated handling; never operate on the user's real SD card without approval.

## X04: SD save risk lead
https://github.com/brendanclarke/LXR02Open/blob/prime/MEMORY.md documents async FatFs pitfalls: an async read returning zero may mean waiting, not EOF, and the filesystem can serialize only one operation. These are ideas to cross-check against our different FatFs implementation, not evidence of the same bugs.

## Next atomic experiments
1. Integrate and execute X01 checked-in C test in native CI; do not modify firmware.
2. Fetch and inspect QEONDA change-all implementation, then map differences to our SHIFT semantics.
3. Build lxrpy disposable fixture tests with explicit stock/custom-format compatibility boundaries.
4. Return to complete native DrumVoice wavetable ON/OFF PCM render before UI implementation.
5. Commit/push and checkpoint each step; **strict ARM hold**.
