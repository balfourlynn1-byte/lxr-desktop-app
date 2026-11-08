#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host
${CC:-cc} -std=c11 -O2 -g -Wall -Wextra -Werror -fsanitize=undefined \
  -Itests/jb26_stubs -ICore/Inc \
  tests/test_jb26_screensaver_state.c \
  Core/Src/menu/screensaver.c Core/Src/menu/JimbonesSkull.c \
  -o build/host/test_jb26_screensaver_state
build/host/test_jb26_screensaver_state
