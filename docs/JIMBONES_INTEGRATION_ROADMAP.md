# JIMBONES staged integration roadmap

Goal: one future flashable build with P0/P1/P2 features, preserving all six voices and recoverability. This is a dependency plan, not a promise that every proposed feature will fit the STM32F765 CPU/RAM/USB budget.

## Integration gates
- **Gate A — diagnostic baseline**: establish parity with the user's known-working v1.8.02; host and ARM checks; CPU budget and 1–4 voice modulation regression. No new effects.
- **Gate B — sound reliability**: fix reproducible multi-voice crunch and retrigger artifacts; measure full-mix saturation and deadline overruns; run pad hardware tests.
- **Gate C — expressive controls**: morph A/B/C, LFO, velocity curves, hi-hat state and per-hit randomisation; protect kick and legacy kits.
- **Gate D — synthesis and processing**: FM/wavetable improvements, tape saturation and filters; only enable each when CPU/memory limits are measured.
- **Gate E — performance**: step FX, tempo sync and UI performance controls; bounded delay memory and no blocking ISR.
- **Gate F — connectivity**: USB multitrack only if hardware/driver feasibility and latency are demonstrated; otherwise keep this optional and do not block core release.
- **Gate G — combined release candidate**: all accepted feature branches integrated, host regression, ARM build, image checksum, rollback and complete on-device pad test.

## Merge protocol
1. Feature branch → focused draft PR to experimental integration branch.
2. Independent verification review: host tests, ARM compiler, memory budget and audio evidence.
3. Merge only accepted and tested changes; compile the full integration branch after each merge.
4. Tag candidate with exact commit, artifact SHA256 and known limitations.
5. Do not overwrite the known-working v1.8.02 image. Never auto-flash hardware.

## Current state
- v1.8.03 experimental ARM compilation and artifact validation previously passed; on-device behaviour remains unverified.
- JB-03 PR #2 introduces deterministic 1–4 modulated voice host fixture; tests not yet confirmed in GitHub CI.
- JB-02 CPU monitoring still requires identifying the full audio callback and instrumenting DWT cycles.
- Other feature branches contain research briefs, not implemented or independently running agents.

## Hardware-test priorities
1. Boot/recovery and kit compatibility.
2. 3–4 simultaneous pad hits with long decay, modulation crunch and kick integrity.
3. Peak CPU/deadline diagnostics under worst-case synthesis and effects.
4. Morph and expressive controls; then sound processing and step FX.
5. USB multitrack only after independent feasibility gate.
