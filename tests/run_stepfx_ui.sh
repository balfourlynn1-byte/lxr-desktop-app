#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host
${CC:-cc} -std=c11 -O2 -g -Wall -Wextra -Werror -fsanitize=undefined \
  -Itests/host -ICore/Inc tests/test_perfui.c \
  Core/Src/Soundengine/StepFX.c \
  Core/Src/Soundengine/StepFxSend.c \
  Core/Src/Soundengine/StepFxSendMix.c \
  Core/Src/menu/JimbonesPerfUI.c \
  -o build/host/test_perfui
build/host/test_perfui
python3 tests/test_stepfx_ui_source.py
