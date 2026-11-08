#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host
${CC:-cc} -std=c11 -O2 -g -Wall -Wextra -Werror -fsanitize=undefined \
  -ICore/Inc tests/test_jb12_comb.c \
  Core/Src/Soundengine/DSPAudio/CombFilter.c \
  -o build/host/test_jb12_comb
build/host/test_jb12_comb
python3 tests/test_jb12_comb_source.py
