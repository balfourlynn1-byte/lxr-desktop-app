#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host
for test in test_stepfx test_perfui; do
 cc -std=c11 -O2 -g -Wall -Wextra -Werror -fsanitize=address,undefined -Itests/host -ICore/Inc tests/$test.c Core/Src/Soundengine/StepFX.c Core/Src/menu/JimbonesPerfUI.c -o build/host/$test
 build/host/$test
done
sh tests/retrigger/run_tests_mac.sh
