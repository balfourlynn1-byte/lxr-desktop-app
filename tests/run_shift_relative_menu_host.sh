#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host
case "$(uname -s)" in
  Darwin) gc_flag=-Wl,-dead_strip ;;
  Linux) gc_flag=-Wl,--gc-sections ;;
  *) echo "Unsupported host linker" >&2; exit 2 ;;
esac
"${CC:-cc}" -std=gnu11 -O0 -ffunction-sections -fdata-sections \
  -Werror=implicit-function-declaration -Werror=fortify-source \
  -fsanitize=undefined -DSTM32F765xx -DUSE_HAL_DRIVER -DJB_HOST_PROBE \
  -include stdint.h -include tests/host_dsp_intrinsics.h \
  -ICore/Inc -ICore/Src -IFATFS/App -IFATFS/Target \
  -IMiddlewares/Third_Party/FatFs/src \
  -IDrivers/STM32F7xx_HAL_Driver/Inc \
  -IDrivers/CMSIS/Device/ST/STM32F7xx/Include -IDrivers/CMSIS/Include \
  tests/test_shift_relative_menu_host.c "$gc_flag" \
  -o build/host/test_shift_relative_menu_host
build/host/test_shift_relative_menu_host
