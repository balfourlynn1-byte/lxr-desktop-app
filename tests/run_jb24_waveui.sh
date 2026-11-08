#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host
${CC:-cc} -std=c11 -O2 -g -Wall -Wextra -Werror -fsanitize=undefined \
  -ICore/Inc tests/test_jb24_waveui.c \
  Core/Src/menu/JimbonesWaveUI.c -o build/host/test_jb24_waveui
build/host/test_jb24_waveui
python3 tests/test_jb24_waveui_source.py
