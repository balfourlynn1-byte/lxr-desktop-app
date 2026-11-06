#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host
# The original test_stepfx.c assumes all voices are armed by default.
# The new stepfx engine safely defaults to snare and hi-hat, so exercise
# its current grid/phrase/voice-mask regression instead.
bash tests/run_combined_stepfx.sh
${CC:-cc} -std=c11 -O2 -Wall -Wextra -Werror -fsanitize=undefined \
  -Itests/host -ICore/Inc tests/test_perfui.c \
  Core/Src/Soundengine/StepFX.c Core/Src/menu/JimbonesPerfUI.c \
  -o build/host/test_perfui
build/host/test_perfui
sh tests/retrigger/run_tests_mac.sh
