#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host
${CC:-cc} -std=c11 -O2 -g -Wall -Wextra -Werror -fsanitize=undefined,float-cast-overflow \
 -ICore/Inc tests/test_jb27_native_wave.c \
 Core/Src/Soundengine/DSPAudio/JimbonesNativeWave.c -lm \
 -o build/host/test_jb27_native_wave
build/host/test_jb27_native_wave
python3 tests/test_jb27_native_wave_source.py
