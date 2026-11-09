#!/bin/sh
# A03 only. Never invokes ARM toolchains or packages firmware.
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host
"${CC:-cc}" -std=c11 -O2 -g -Wall -Wextra -Werror \
  -fsanitize=address,undefined -fno-omit-frame-pointer \
  -ICore/Inc tests/test_noclip_132.c \
  Core/Src/Soundengine/DSPAudio/RetriggerSmoother.c \
  -o build/host/test_noclip_132
build/host/test_noclip_132
