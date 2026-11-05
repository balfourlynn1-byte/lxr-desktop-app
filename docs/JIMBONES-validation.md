# v1.7.91 validation record

Date: 5 November 2026. Target: STM32F765 desktop LXR-02. Status: experimental image ready for physical evaluation, not hardware validated.

## Source and build

Implementation commit: `f31a852aabed9cd489e7dcc175f4b86dc5fef323` in the user's fork, branch `jimbones-v1.7.91`. Its Git tree (`5ac20093622e0a89e81cc3ae1468862779c04f46`) exactly matches the local implementation tree. Subsequent test/documentation commits do not change the embedded implementation.

The source starts at upstream `a43cd4fcda23ec3d577dc425261c3eb28bd5abff`. A baseline v1.7 build completed before changes. Both baseline and custom build used ARM GCC 16.1.0, the repository's release preset, and the existing ITCM `-O2` setting. The baseline already produces an unused `foobar` variable warning in main.c; this was not introduced by JIMBONES. The host storage fixture also reports Apple's deprecation warning for the existing `sprintf("Empty")` call. Neither is a test failure.

No upstream writes, bootloader programming, physical flashing or replacement of prior release images was performed.

| Linker region | Baseline bytes | v1.7.91 bytes | Capacity | v1.7.91 use |
|---|---:|---:|---:|---:|
| ITCM | 15,128 | 15,328 | 16,384 | 93.55% |
| DTCM | 107,392 | 107,392 | 131,072 | 81.93% |
| RAM | 271,040 | 282,576 | 393,216 | 71.86% |
| FLASH region | 223,724 | 231,948 | 1,015,808 | 22.83% |

These are linker region reports, not measured stack high-water marks or CPU load. The image includes load data for RAM sections as well as the FLASH region.

## Automated results

- Production DSP and existing waveform tables compiled on the Intel Mac with address, undefined-behavior and float-cast sanitizers.
- Original-only mode leaves an input sample buffer bit-identical for every voice. This proves the added renderer's bypass; it is not a complete original-engine audio regression comparison.
- All six voices × all three enabled modes produce output; other voices remain silent. Zero layer levels, amplitude velocity response and open-to-closed layer-tail replacement pass.
- Deterministic seeded render and disable/re-enable without stale tail pass.
- 11,520,000 stress samples with all voices, rapid retriggers, varied pitches/velocities and randomized/minimum/maximum settings pass. No sanitizer failures and zero int16 clipping events in this isolated-layer test. This does not establish that sums with original voices/FX cannot clip.
- Kit codec round trip passes. Every possible truncation and each single-byte corruption of a 180-byte record defaults safely to disabled layers. Out-of-range parameter values are rejected.
- Actual kit/RAM/autosave functions, compiled with an in-memory FatFs fixture, pass save/reload, preserved original payload offsets, legacy-file handling, corrupt-extension handling and invalid temporary-slot checks.
- Actual menu transition function passes first-press original / second-press macros, every added screen, exit to original pages, both hat selection pages, and legacy second-page/global behavior.

A waveform indexing fault found during the first stress run was fixed before this build. Bounded phase wrapping now handles tiny negative values that round up to exactly 1.0 when wrapped; the reproducing randomized stress run passes.

## Firmware container

`LXR_V2_V1.7.91.img`:

- Total size: 284,288 bytes.
- Application payload: 284,272 bytes.
- Magic: `LXRV2IMG`.
- Payload checksum byte: `DB`.
- Initial stack: `0x20080000`.
- Reset vector: `0x08013e4d` (Thumb entry within the application image).
- Payload matches the linked build's raw `.bin` exactly.
- SHA-256: `9e514ec6cfeef72d595f433824269c9a95201cd8e0e634304a07507d8e495844`.

The independently supplied fallback `LXR_V2_V1.7.90.img` also passes container checks. Its preserved copy matches the Downloads original, SHA-256 `57ce6e6a6ad729d4e7a4c1d657fce10e39c0c94f94a24abc078442afe74b266c`. Binary validation cannot recover or establish its source modifications.

## Not yet verified

Physical boot, actual LCD/button behavior, real SD-card save/reload, full original-engine audible regression, real simultaneous MIDI/USB/DIN behavior, pad/pedal performance, actual interrupt safety under load, worst-case stack usage, CPU deadline margin, end-to-end latency and listening quality are unverified. On-device cycle telemetry is included for measuring render cost, but no physical measurements have been made.

The new engines are bounded experimental DSP, with a four-wave interpolated bank and no nonlinear oversampling. Listen for aliasing/clicks at extreme settings. The existing morph implementation does not interpolate additional parameters. Future roadmap features are deferred as listed in the usage guide.

See `JIMBONES-v1.7.91.md` for controls, installation/rollback and the required hardware checklist. See `JIMBONES-roadmap.md` for the supplied master architecture.
