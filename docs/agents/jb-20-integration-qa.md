# JB-20 — Combined hardware-test firmware integration and QA
**Status:** Assigned, not executing autonomously. **Target:** jimbones-v1.8.03-hardware-test; do not touch working v1.8.02.

Implementer: triage JB-01..JB-19 branches/PRs, stage one reviewed feature at a time, preserve six original voices, old kits, no bootloader edits. Run host regression, exact ARM build, memory map, SHA, verify artifact is downloadable. Never silently remove feature requirements; defer explicitly if CPU budget or compatibility fails. Highest priority JB-03 crunch, JB-02 CPU and JB-14 CLICK UI. Require Step FX sends, toggles, flashing LEDs, morph and FD-8 acceptance before claiming complete candidate.

Independent reviewer: compare before/after old-kit sound and data, inspect DSP headroom and real deadline misses for four simultaneous modulated hits, repeat long tails and clock stress. Ensure all changes are optional/bypassable, check power-on/SD recovery. Hardware operator must test Alesis pads, Roland KT-10, FD-8, CLICK menu and Step FX LEDs; remote CI cannot certify these.

Gates: G0 rollback, G1 crunch and CPU, G2 combined host/ARM, G3 hardware. Release only as **UNTESTED hardware-test candidate** until device report is received.

Progress dashboard: docs/JIMBONES_COMBINED_BUILD_IMPLEMENTATION_TRACKER.md.
