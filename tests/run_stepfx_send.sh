#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host
${CC:-cc} -std=c11 -O2 -g -Wall -Wextra -Werror -fsanitize=undefined \
  -ICore/Inc tests/test_stepfx_send.c Core/Src/Soundengine/StepFxSend.c \
  -o build/host/test_stepfx_send
build/host/test_stepfx_send
