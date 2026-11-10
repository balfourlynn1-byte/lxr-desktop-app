#!/usr/bin/env bash
# Verify host-only probe fails safely before touching firmware or invoking a compiler.
set -euo pipefail
repo_root="$(cd "$(dirname "$0")/.." && pwd)"
tmp="$(mktemp -d)"
trap 'rm -rf "$tmp"' EXIT
mkdir -p "$tmp/tests" "$tmp/Core/Src/Soundengine/DSPAudio"
cp "$repo_root/tests/run_native_drumvoice_probe.sh" "$tmp/tests/"
touch "$tmp/Core/Src/Soundengine/DSPAudio/DrumVoice.c"
cat > "$tmp/fake-arm-cc" <<'EOF'
#!/usr/bin/env bash
if [[ "${1:-}" == "-dumpmachine" ]]; then
  printf '%s\n' 'arm-none-eabi'
  exit 0
fi
echo 'ERROR: compiler unexpectedly invoked beyond target detection' >&2
exit 99
EOF
chmod +x "$tmp/fake-arm-cc"
set +e
output="$(cd "$tmp" && CC="$tmp/fake-arm-cc" bash tests/run_native_drumvoice_probe.sh 2>&1)"
status=$?
set -e
if [[ "$status" != 2 || "$output" != *"is prohibited in host-only probe"* ]]; then
  printf 'FAIL: ARM compiler rejection: exit=%s output=%s\n' "$status" "$output" >&2
  exit 1
fi
echo 'PASS: ARM cross-compiler rejected before compilation (exit 2)'
set +e
output="$(cd "$tmp" && CC=__missing_host_compiler__ bash tests/run_native_drumvoice_probe.sh 2>&1)"
status=$?
set -e
if [[ "$status" != 2 || "$output" != *"not found"* ]]; then
  printf 'FAIL: missing compiler rejection: exit=%s output=%s\n' "$status" "$output" >&2
  exit 1
fi
echo 'PASS: missing host compiler rejected (exit 2)'
# Host-only fake compiler records the actual syntax command without compiling firmware.
mkdir -p "$tmp/Drivers/CMSIS/Device/ST/STM32F7xx/Include" "$tmp/Drivers/CMSIS/Include"
touch "$tmp/Drivers/CMSIS/Device/ST/STM32F7xx/Include/stm32f7xx.h" "$tmp/Drivers/CMSIS/Include/core_cm7.h"
cat > "$tmp/fake-host-cc" <<'HOST'
#!/usr/bin/env bash
if [[ "${1:-}" == "-dumpmachine" ]]; then printf '%s\n' 'x86_64-linux-gnu'; exit 0; fi
printf '%s\n' "$@" > "$(dirname "$0")/captured-args"
HOST
chmod +x "$tmp/fake-host-cc"
output="$(cd "$tmp" && CC="$tmp/fake-host-cc" bash tests/run_native_drumvoice_probe.sh 2>&1)"
for flag in '-fsyntax-only' '-DSTM32F765xx' '-DUSE_HAL_DRIVER' '-include' 'stdint.h' 'Core/Src/Soundengine/DSPAudio/DrumVoice.c'; do
  if ! grep -Fxq -- "$flag" "$tmp/captured-args"; then
    printf 'FAIL: required host compile argument missing: %s\n' "$flag" >&2; exit 1
  fi
done
if [[ "$output" != *"PASS: DrumVoice.c host syntax only"* ]]; then
  echo 'FAIL: host probe did not finish' >&2; exit 1
fi
echo 'PASS: fake host compiler received syntax-only, STM32F765xx, USE_HAL_DRIVER, stdint and DrumVoice source arguments'
echo 'NOTE: fixture/preflight only; actual DrumVoice native compilation and audio NOT RUN'
