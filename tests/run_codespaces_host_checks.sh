#!/usr/bin/env bash
# JIMBONES remote Linux host-only checks. No ARM, link, firmware image, or flash.
set -euo pipefail
cd "$(dirname "$0")/.."
if [[ ! -f Core/Src/Soundengine/DSPAudio/DrumVoice.c ]]; then
  echo 'FAIL: run from a complete JIMBONES checkout' >&2
  exit 2
fi
if ! command -v gcc >/dev/null 2>&1; then
  echo 'FAIL: install native GCC in this Linux environment' >&2
  exit 2
fi
echo "COMMIT: $(git rev-parse HEAD)"
echo "BRANCH: $(git branch --show-current)"
echo "GCC: $(gcc --version | head -n 1)"
echo '--- Preflight (fixture; NOT native compile) ---'
bash tests/test_native_probe_preflight.sh
echo '--- Real DrumVoice.c host syntax compile ---'
CC=gcc bash tests/run_native_drumvoice_probe.sh
echo 'PASS: native GCC syntax check; NOT linked, PCM tested, ARM compiled or hardware ready'
