# JIMBONES v1.8.03 — ARM hardware-test gate

**Experimental branch only. Do not overwrite v1.8.02.**

## What is committed

- Firmware version: 1.8.03
- Existing JIMBONES six-voice metallic/wavetable/Step FX code retained from v1.8.01 branch
- Global waveform interpolation ON/OFF function, ON by default
- Ninth JIMBONES OSC extension screen: WAVE INTERPOLATE
- First pot and encoder toggle the interpolation mode
- Ninth-page pot access guard prevents out-of-range parameter indexing
- Host regression for ninth page and waveform interpolation sound A/B
- Cloud workflow: host regression first; then ARM cross-compile, image validation, checksum, ELF section-size report

## Verified locally (not on hardware)

A source candidate based on the same DSP implementation passed:
- `CC=clang bash tests/run_host.sh`
- `CC=clang bash tests/run_waveshaping.sh`
- Interpolation audio A/B: ON and OFF differ; returning to ON reproduces the original waveform exactly
- Six-voice retrigger/stress tests, kit round-trip, legacy menu transition

The **GitHub Actions run and the ARM binary have NOT yet been independently verified**. Do not claim the .img is ready before the Actions workflow succeeds.

## Cloud verification

Open [Actions](https://github.com/balfourlynn1-byte/lxr-desktop-app/actions) and select **JIMBONES v1.8.03 ARM test build**. Choose branch `jimbones-v1.8.03-hardware-test` and click **Run workflow** if no automatic push run is available.

Both jobs must pass. Download `JIMBONES-v1.8.03-ARM-UNTESTED`. Verify the included `SHA256SUMS.txt` against `LXR_V2_V1.8.03.img`, and inspect the ELF section sizes. Check that memory usage is acceptable against v1.8.02 before installing.

## Physical test (only after cloud validation)

1. Keep the known-working `LXR_V2_V1.8.02.img` and an SD-card backup available; do not overwrite either.
2. Use the normal SD updater and the separately named v1.8.03 image; do not use any full-flash/bootloader update.
3. Confirm boot/version and default kit before enabling experimental layers.
4. Test all six stock voices, wavetable interpolation ON/OFF, metallic FM, Step FX, kit save/load and overlapping 3–4 drum-pad hits.
5. Listen for clicks/crunch and check CPU/memory behaviour, especially multi-voice modulation.
6. If the device fails to boot, **do not assume SD rollback is possible**; follow the device's documented recovery process rather than repeatedly flashing.

## Scope caveats

This is **not** a full LXR02Open port: it exposes interpolation in the existing JIMBONES wavetable engine, not LXR02Open's noise/sample interpolation. The toggle is not persistent across power cycles. The host tests do not establish physical hardware stability.
