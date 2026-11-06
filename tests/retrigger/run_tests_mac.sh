#!/bin/sh
set -eu
ROOT=$(CDPATH='' cd -- "$(dirname -- "$0")/../.." && pwd)
cd "$ROOT"
OUT="${TMPDIR:-/tmp}/lxr_retrigger_host_test_$$"
trap 'rm -f "$OUT"' EXIT HUP INT TERM
cc -std=c11 -O2 -Wall -Wextra -Werror -I Core/Inc \
  Core/Src/Soundengine/DSPAudio/RetriggerSmoother.c \
  tests/retrigger/test_retrigger_smoother.c -o "$OUT"
"$OUT"
