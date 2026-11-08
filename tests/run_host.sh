#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host
sh tests/run_stepfx_send.sh
sh tests/run_stepfx_send_mix.sh
sh tests/run_stepfx_ui.sh
sh tests/run_dub_delay_sync.sh
python3 tests/test_dub_delay_source.py
sh tests/run_jb22_metallic_off.sh
python3 tests/test_jb22_integration.py
sh tests/run_jb24_waveui.sh
sh tests/run_jb12_comb.sh
${CC:-cc} -std=c11 -O2 -g -Wall -Wextra -Werror -fsanitize=address,undefined,float-cast-overflow \
 -Itests/host -ICore/Inc tests/test_jimbones.c \
 Core/Src/Soundengine/DSPAudio/Jimbones.c Core/Src/Soundengine/DSPAudio/wavetable.c \
 Core/Src/Preset/JimbonesPreset.c -lm -o build/host/test_jimbones
build/host/test_jimbones build/host/layers-demo.s16
python3 tests/test_menu.py
python3 tests/test_storage_paths.py
${CC:-cc} -std=c11 -O2 -g -Wall -Wextra -Werror -fsanitize=address,undefined,float-cast-overflow \
 -Itests/host -ICore/Inc tests/test_retrigger.c \
 Core/Src/Soundengine/DSPAudio/Jimbones.c Core/Src/Soundengine/DSPAudio/wavetable.c \
 -lm -o build/host/test_retrigger
build/host/test_retrigger
${CC:-cc} -std=c11 -O2 -g -Wall -Wextra -Werror -fsanitize=address,undefined,float-cast-overflow \
 -Itests/host -ICore/Inc tests/test_texture_noise.c \
 Core/Src/Soundengine/DSPAudio/Jimbones.c Core/Src/Soundengine/DSPAudio/wavetable.c \
 -lm -o build/host/test_texture_noise
build/host/test_texture_noise
