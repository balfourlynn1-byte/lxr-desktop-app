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
echo 'NOTE: preflight only; DrumVoice native compilation and audio NOT RUN'
