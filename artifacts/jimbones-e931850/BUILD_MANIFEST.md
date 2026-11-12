# JIMBONES EXPERIMENTAL — HOST VERIFIED, HARDWARE UNTESTED

Build date: 2026-11-12 07:16 UTC  
Source commit: `e93185028632b8044c214dc8500b1f62203239e2`  
Branch: `agents/jb-wavetable-host-experiment`  
Target: LXR V2 desktop, STM32F765VG, Cortex-M7  
Toolchain: arm-none-eabi-gcc 16.1.0, macOS x86_64

## Image

- Update file: `LXR_V2_V1.8.07.img`
- Image size: 306,672 bytes
- SHA-256: `8698c941e14d8192011399a88172f6f28cbd962772d88fd6f486ae1bbc3dfbab`
- Header: `LXRV2IMG`, little-endian payload length 306,656, checksum byte `0x38`; payload matches the generated .bin byte for byte and satisfies the bootloader checksum.
- Local build outputs: `build/release/LXR_V2_STM32F765.elf` (548,028 bytes, SHA-256 `743c57589fc500cd0063acbb695462f0ac9eeed3397cbe6591f533ac782493eb`) and `build/release/LXR_V2_STM32F765.bin` (306,656 bytes, SHA-256 `ad114a1f4bb53cbaa9ba659aada1e7903dc26ffbd729f3f51f4901e65b444287`).

## Verification

- `cmake -S tests/production/emu -B /tmp/jb-host-emu -DFW="$PWD"`, `cmake --build /tmp/jb-host-emu -j2`, `ctest --test-dir /tmp/jb-host-emu --output-on-failure`: exit 0, six of six tests PASS.
- Project-save emulator: 62 injected failed saves, including measured third-save backup cleanup, write/sync/rename boundaries, SD-full/removal and successful retries. Cold starts recovered internally consistent old or new generations.
- Globals emulator: 24 fault/recovery cases plus four successful post-failure retries; CRC-backed backup and legacy globals checks PASS.
- `sh tests/run_stepfx_ui.sh` and `sh tests/run_shift_relative_menu_host.sh`: exit 0.
- `bash tests/run_off_path_pcm_compare.sh`: exit 0; 12/12 scenarios, zero sample mismatches against integration commit `f797ec786fc57d008afeaabad964906f35a43177`. Overlap host-sum has three clipped samples in both reference and experiment.
- `cmake --preset release`, `cmake --build --preset release --clean-first`: exit 0. ELF is ARM 32-bit little-endian, EABI5 hard-float. Symbols include `preset_saveProject`, `preset_loadProject`, and the project fingerprint routine.
- Linker: ITCM 15,624/16,384 B (95.36%); DTCM 110,560/131,072 B (84.35%); RAM 288,024/393,216 B (73.25%); application flash 254,036/1,015,808 B (25.01%). Against pinned integration with the same toolchain: +136 B ITCM, +0 DTCM, +3,368 B RAM, +8,528 B flash.
- Five ARM compiler warnings: one pre-existing unused variable, three pre-existing menu string truncation warnings, one unused preset helper. No compiler or linker error.

## Boundaries and rollback

No physical LXR-02 test, SD power-loss test, or target CPU-load measurement has been run. Native fault injection does not reproduce torn FAT sectors. Use only with a backed-up test SD card and preserve a known-working firmware image and project backup for rollback. Do not use this build as a live-performance release. No hardware flashing or production merge was performed.
