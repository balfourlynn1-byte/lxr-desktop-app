#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host
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

# JB-03 deterministic four-voice modulation fixture
${CC:-cc} -std=c11 -O2 -g -Wall -Wextra -Werror -fsanitize=address,undefined,float-cast-overflow \
 -Itests/host -ICore/Inc tests/test_four_voice_modulation.c \
 Core/Src/Soundengine/DSPAudio/Jimbones.c Core/Src/Soundengine/DSPAudio/wavetable.c \
 -lm -o build/host/test_four_voice_modulation
build/host/test_four_voice_modulation

# JB-03 isolated summed PCM headroom diagnostic
${CC:-cc} -std=c11 -O2 -g -Wall -Wextra -Werror -fsanitize=address,undefined,float-cast-overflow \
 -Itests/host -ICore/Inc tests/test_four_voice_sum.c \
 Core/Src/Soundengine/DSPAudio/Jimbones.c Core/Src/Soundengine/DSPAudio/wavetable.c \
 -lm -o build/host/test_four_voice_sum
build/host/test_four_voice_sum

# JB-02 wrap-safe CPU render telemetry regression
${CC:-cc} -std=c11 -O2 -g -Wall -Wextra -Werror -fsanitize=address,undefined \
 -ICore/Inc tests/test_audio_telemetry.c -o build/host/test_audio_telemetry
build/host/test_audio_telemetry

# JB-14 transient shaper isolated DSP regression
${CC:-cc} -std=c11 -O2 -g -Wall -Wextra -Werror -fsanitize=address,undefined \
 -ICore/Inc tests/test_transient_shaper.c -lm -o build/host/test_transient_shaper
build/host/test_transient_shaper
