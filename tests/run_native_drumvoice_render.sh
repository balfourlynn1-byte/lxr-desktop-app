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
scenario="${2:-kick}"
build="$(mktemp -d "${TMPDIR:-/tmp}/jb-native-render.XXXXXX")"
flags=(-std=gnu11 -O0 -Werror=implicit-function-declaration
  -DSTM32F765xx -DUSE_HAL_DRIVER -DJB_HOST_PROBE
  -include stdint.h -include tests/host_dsp_intrinsics.h
  -ICore/Inc -ICore/Src -IDrivers/STM32F7xx_HAL_Driver/Inc
  -IDrivers/CMSIS/Device/ST/STM32F7xx/Include -IDrivers/CMSIS/Include)
units=(DrumVoice Snare CymbalVoice HiHat Oscillator Decay ResonantFilter snapEg SlopeEg2
  transientGenerator transientTables 1PoleLp distortion BufferTools
  RetriggerSmoother lfo modulationNode Jimbones random Samples dither
  squareRootLut wavetable CombFilter)
if [[ -f Core/Src/Soundengine/DSPAudio/JimbonesNativeWaveBank.c ]]; then
  units+=(JimbonesNativeWaveBank)
  flags+=(-DJB_EXPERIMENTAL_WAVE)
fi
objects=()
for unit in "${units[@]}"; do
  "$CC" "${flags[@]}" -c "Core/Src/Soundengine/DSPAudio/$unit.c" -o "$build/$unit.o"
  objects+=("$build/$unit.o")
done
"$CC" "${flags[@]}" -c tests/host_drumvoice_render.c -o "$build/renderer.o"
"$CC" "${objects[@]}" "$build/renderer.o" -lm -o "$build/renderer"
first_status=0
second_status=0
"$build/renderer" "$out" "$scenario" || first_status=$?
"$build/renderer" "$out.repeat" "$scenario" || second_status=$?
if [[ "$first_status" -ne "$second_status" ]]; then
  echo "FAIL: repeat render exit statuses differ: $first_status vs $second_status" >&2
  exit 1
fi
cmp "$out" "$out.repeat"
test "$(wc -c < "$out")" -eq 8192
shasum -a 256 "$out" "$out.repeat"
if [[ "$first_status" -ne 0 ]]; then
  echo "FAIL: real DSP rendered deterministic PCM with clipping or silence (exit $first_status)." >&2
  exit "$first_status"
fi
echo "PASS: real DrumVoice DSP linked, rendered non-silent PCM twice, and matched exactly."
