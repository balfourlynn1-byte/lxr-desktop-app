#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host
${CC:-cc} -std=c11 -O2 -g -Wall -Wextra -Werror -fsanitize=undefined \
  -Itests/host -ICore/Inc \
  tests/test_dub_delay_sync.c \
  Core/Src/Soundengine/DubDelaySync.c \
  Core/Src/Soundengine/StepFxSend.c \
  -o build/host/test_dub_delay_sync
build/host/test_dub_delay_sync
