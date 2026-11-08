#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host
${CC:-cc} -std=c11 -O2 -g -Wall -Wextra -Werror -fsanitize=undefined \
  -ICore/Inc tests/test_stepfx_send_mix.c \
  Core/Src/Soundengine/StepFxSend.c Core/Src/Soundengine/StepFxSendMix.c \
  -o build/host/test_stepfx_send_mix
build/host/test_stepfx_send_mix
