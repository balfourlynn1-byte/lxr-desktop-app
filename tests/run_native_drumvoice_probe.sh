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
echo "HOST PROBE: $("$CC" --version | head -n 1)"
echo "SOURCE: $src"
"$CC" -std=gnu11 -fsyntax-only -Wall -Wextra \
  -Werror=implicit-function-declaration -DSTM32F765xx -include stdint.h \
  -ICore/Inc -ICore/Src \
  -IDrivers/STM32F7xx_HAL_Driver/Inc \
  -IDrivers/CMSIS/Device/ST/STM32F7xx/Include \
  -IDrivers/CMSIS/Include "$src"
echo "PASS: DrumVoice.c host syntax only; not link, PCM, DSP or ARM."
