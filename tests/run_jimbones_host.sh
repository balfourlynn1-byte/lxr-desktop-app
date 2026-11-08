#!/usr/bin/env bash
set -euo pipefail
mkdir -p build/host
${CC:-cc} -std=gnu11 -O2 -Wall -Wextra \
  -Itests/host_include -ICore/Inc \
  tests/test_jimbones.c \
  Core/Src/Soundengine/DSPAudio/Jimbones.c \
  Core/Src/Soundengine/DSPAudio/wavetable.c \
  Core/Src/Preset/JimbonesPreset.c \
  -lm -o build/host/test_jimbones
build/host/test_jimbones

# Controller-only test: no claim that audio sends are routed yet.
${CC:-cc} -std=gnu11 -O2 -Wall -Wextra \
  -Itests/host_include -ICore/Inc \
  tests/test_stepfx_dub.c Core/Src/Soundengine/StepFX.c \
  -o build/host/test_stepfx_dub
build/host/test_stepfx_dub

# Compile the real Step FX Performance selector against the scheduler.
${CC:-cc} -std=gnu11 -O2 -Wall -Wextra \
  -Itests/host_include -ICore/Inc \
  tests/test_stepfx_ui.c Core/Src/menu/JimbonesPerfUI.c \
  Core/Src/Soundengine/StepFX.c -o build/host/test_stepfx_ui
build/host/test_stepfx_ui

python3 tests/test_dub_mixer_routing.py

python3 tests/test_mixer_route.py
