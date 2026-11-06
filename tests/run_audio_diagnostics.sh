#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host
${CC:-cc} -std=c11 -O2 -Wall -Wextra -Werror -fsanitize=undefined \
  -DLXR_EMU=1 -ICore/Inc tests/test_audio_diagnostics.c \
  Core/Src/Soundengine/AudioDiagnostics.c \
  -o build/host/test_audio_diagnostics
build/host/test_audio_diagnostics
