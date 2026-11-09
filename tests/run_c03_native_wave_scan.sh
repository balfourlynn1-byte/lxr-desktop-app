#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host
${CC:-cc} -std=c11 -O2 -Wall -Wextra -Werror -fsanitize=undefined,float-cast-overflow -ICore/Inc tests/test_c03_native_wave_scan.c -lm -o build/host/test_c03_native_wave_scan
build/host/test_c03_native_wave_scan
