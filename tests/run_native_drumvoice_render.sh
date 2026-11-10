#!/usr/bin/env bash
# Host-only real DSP renderer. Produces raw signed 16-bit native-endian PCM.
set -euo pipefail
cd "$(dirname "$0")/.."
CC="${CC:-clang}"
target="$("$CC" -dumpmachine)"
case "$target" in
  *arm*|*thumb*|*aarch64*|*none-eabi*)
    echo "FAIL: target compiler is prohibited: $target" >&2; exit 2 ;;
esac
out="${1:-/tmp/jb-drumvoice-render.pcm}"
build="$(mktemp -d "${TMPDIR:-/tmp}/jb-native-render.XXXXXX")"
flags=(-std=gnu11 -O0 -Werror=implicit-function-declaration
  -DSTM32F765xx -DUSE_HAL_DRIVER -DJB_HOST_PROBE
  -include stdint.h -include tests/host_dsp_intrinsics.h
  -ICore/Inc -ICore/Src -IDrivers/STM32F7xx_HAL_Driver/Inc
  -IDrivers/CMSIS/Device/ST/STM32F7xx/Include -IDrivers/CMSIS/Include)
units=(DrumVoice Oscillator Decay ResonantFilter snapEg SlopeEg2
  transientGenerator transientTables 1PoleLp distortion BufferTools
  RetriggerSmoother lfo modulationNode Jimbones random Samples dither
  squareRootLut JimbonesNativeWaveBank wavetable CombFilter)
objects=()
for unit in "${units[@]}"; do
  "$CC" "${flags[@]}" -c "Core/Src/Soundengine/DSPAudio/$unit.c" -o "$build/$unit.o"
  objects+=("$build/$unit.o")
done
"$CC" "${flags[@]}" -c tests/host_drumvoice_render.c -o "$build/renderer.o"
"$CC" "${objects[@]}" "$build/renderer.o" -lm -o "$build/renderer"
"$build/renderer" "$out"
"$build/renderer" "$out.repeat"
cmp "$out" "$out.repeat"
test "$(wc -c < "$out")" -eq 8192
shasum -a 256 "$out" "$out.repeat"
echo "PASS: real DrumVoice DSP linked, rendered non-silent PCM twice, and matched exactly."
