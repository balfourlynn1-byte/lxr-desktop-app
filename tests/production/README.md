# JIMBONES production DSP test bench
Run `python3 run.py /path/to/noclip-baseline /path/to/noclip-stepfx /path/to/noclip-metal-wt --out results`.
Requires CMake, a native Clang C compiler, Python 3 and three source checkouts. Each process boots once with seed 123. WAVs are 16-bit stereo, 44,140 Hz. `events.txt` is frame-index, MIDI status, note, velocity. It combines long tom tails, 50 ms alternating hits, alternating pitches/velocities, all voices together, open/closed hats and events just before/on/after 64-sample boundaries. The last two seconds allow tails to decay. Calls enter the real MIDI parser and production DSP/mixer. Delivery retains the emulator's normal block quantisation.

`emu/` adapts bg1eym/lxr-desktop-app's emu branch (GPL-3.0-only). Attribution is retained. Production sources are supplied by `-DFW`, not duplicated DSP approximations. `prepare.py` removes target memory-section placement, guards a null-address modulation read that Cortex firmware tolerates, and renames a duplicate weak FatFs timestamp fallback for Mach-O linking. HAL/SD/UI/timer peripherals are substitutes; native float conversion uses the emulator's saturating-cast option. Native results do NOT establish ARM bit identity, ISR deadlines, electrical input latency or physical peripheral operation.

The gregsadetsky/lxr02-recomp project was inspected, including its QEMU/recompiled comparison workflow. Its separate toolchain/QEMU/Emscripten setup was not run for these builds. No Cortex-M7 cycle-accurate result is claimed.

Mode 4 in this harness means a maximum-fader headroom stress, not a fifth firmware mode. Baseline mode 4 is original voices; metal mode 4 enables both layers. Main comparison modes use fixed 0.4 faders. Full-scale stress clips on both the stock baseline and the layered build. Listen at a suitable level; these files are test renders, not hardware recordings.

Equivalent raw MIDI replay is deterministic. This models either pedal-generated or manually generated Ableton chain switching; no actual FD-8, Ableton or physical LXR was connected to this test bench. Those acceptance tests remain with the hardware operator.
