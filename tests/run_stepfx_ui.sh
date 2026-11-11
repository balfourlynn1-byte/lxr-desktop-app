#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host
${CC:-cc} -std=c11 -O2 -g -Wall -Wextra -Werror -fsanitize=undefined \
  -Itests/host -ICore/Inc tests/test_perfui.c \
  Core/Src/Soundengine/StepFX.c \
  Core/Src/Soundengine/StepFxSend.c \
  Core/Src/Soundengine/StepFxSendMix.c \
  Core/Src/Soundengine/DubDelaySync.c \
  Core/Src/menu/JimbonesPerfUI.c \
  -o build/host/test_perfui
build/host/test_perfui
${CC:-cc} -std=gnu11 -O0 -g -Wall -Wextra -Werror=implicit-function-declaration -fsanitize=undefined \
  -DSTM32F765xx -DUSE_HAL_DRIVER -DJB_HOST_PROBE -include stdint.h \
  -ICore/Inc -ICore/Src -IDrivers/STM32F7xx_HAL_Driver/Inc \
  -IDrivers/CMSIS/Device/ST/STM32F7xx/Include -IDrivers/CMSIS/Include \
  tests/test_led_handler_host.c Core/Src/ledHandler.c \
  -o build/host/test_led_handler_host
build/host/test_led_handler_host
python3 tests/test_stepfx_ui_source.py
