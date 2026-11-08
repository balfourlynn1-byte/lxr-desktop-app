#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host
${CC:-cc} -std=c11 -O2 -g -Wall -Wextra -Werror -fsanitize=undefined \
 -ICore/Inc tests/test_jb26_skull.c Core/Src/menu/JimbonesSkull.c \
 -o build/host/test_jb26_skull
build/host/test_jb26_skull
python3 tests/test_jb26_screensaver_source.py
sh tests/run_jb26_screensaver_state.sh
