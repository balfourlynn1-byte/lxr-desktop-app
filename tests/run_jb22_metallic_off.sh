#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host
${CC:-cc} -std=c11 -O2 -g -Wall -Wextra -Werror -fsanitize=undefined,float-cast-overflow \
  -Itests/host -ICore/Inc tests/test_jb22_metallic_off.c \
  Core/Src/Soundengine/DSPAudio/Jimbones.c \
  Core/Src/Soundengine/DSPAudio/wavetable.c -lm \
  -o build/host/test_jb22_metallic_off
build/host/test_jb22_metallic_off
