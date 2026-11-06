#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host
cc -std=c11 -O2 -Wall -Wextra -Werror -fsanitize=undefined \
    -Itests/host -ICore/Inc \
    tests/test_combined_stepfx.c Core/Src/Soundengine/StepFX.c \
    -o build/host/test_combined_stepfx
build/host/test_combined_stepfx
