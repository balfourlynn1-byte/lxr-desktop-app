#!/usr/bin/env bash
# Host-only DrumVoice syntax probe. Never invokes ARM toolchain or builds firmware.
set -euo pipefail
cd "$(dirname "$0")/.."
CC="${CC:-gcc}"
src=Core/Src/Soundengine/DSPAudio/DrumVoice.c
if [[ ! -f "$src" ]]; then
  echo "FAIL: missing $src" >&2
  exit 2
fi
if ! command -v "$CC" >/dev/null 2>&1; then
  echo "FAIL: host compiler '$CC' not found (set CC=gcc or CC=clang)" >&2
  exit 2
fi
# Refuse target cross-compilers even when CC is overridden: this probe is host-only.
compiler_target="$("$CC" -dumpmachine 2>/dev/null)" || {
  echo "FAIL: cannot identify compiler target for '$CC'" >&2
  exit 2
}
case "$compiler_target" in
  *arm*|*thumb*|*aarch64*|*none-eabi*)
    echo "FAIL: target compiler '$CC' ($compiler_target) is prohibited in host-only probe" >&2
    exit 2 ;;
esac
echo "HOST TARGET: $compiler_target"
for header in \
  Drivers/CMSIS/Device/ST/STM32F7xx/Include/stm32f7xx.h \
  Drivers/CMSIS/Include/core_cm7.h; do
  if [[ ! -f "$header" ]]; then
    echo "FAIL: missing host-probe dependency $header" >&2
    exit 2
  fi
done
echo "HOST PROBE: $CC"
echo "SOURCE: $src"
"$CC" -std=gnu11 -fsyntax-only -Wall -Wextra \
  -Werror=implicit-function-declaration -DSTM32F765xx -DUSE_HAL_DRIVER -DJB_HOST_PROBE \
  -include stdint.h -include tests/host_dsp_intrinsics.h \
  -ICore/Inc -ICore/Src \
  -IDrivers/STM32F7xx_HAL_Driver/Inc \
  -IDrivers/CMSIS/Device/ST/STM32F7xx/Include \
  -IDrivers/CMSIS/Include "$src"
echo "PASS: DrumVoice.c host syntax only; not link, PCM, DSP or ARM."
