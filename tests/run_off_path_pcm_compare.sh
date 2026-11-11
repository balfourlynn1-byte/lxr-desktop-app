#!/usr/bin/env bash
# Compare real original-voice PCM against the pinned, read-only integration tree.
# The reference archive is adapted only for native host compilation in /tmp.
set -euo pipefail
cd "$(dirname "$0")/.."
reference=f797ec786fc57d008afeaabad964906f35a43177
git cat-file -e "$reference^{commit}"
if [[ "$(git branch --show-current)" != agents/jb-wavetable-host-experiment ]]; then
  echo "FAIL: run from experimental branch only" >&2; exit 2
fi
work="$(mktemp -d "${TMPDIR:-/tmp}/jb-off-path-compare.XXXXXX")"
mkdir "$work/reference"
git archive "$reference" | tar -x -C "$work/reference"
cp tests/host_dsp_intrinsics.h tests/host_drumvoice_render.c \
  tests/run_native_drumvoice_render.sh "$work/reference/tests/"
patch -s -p1 -d "$work/reference" < tests/reference_host_adaptations.patch
echo "EXPERIMENTAL: $(git rev-parse HEAD)"
echo "INTEGRATION: $reference"
echo "HOST TARGET: $("${CC:-clang}" -dumpmachine)"
echo "EVIDENCE DIR: $work"
run_renderer() {
  local log=$1
  shift
  python3 - "$log" "$@" <<'PY'
import subprocess
import sys

log, *command = sys.argv[1:]
with open(log, "w") as output:
    try:
        result = subprocess.run(command, stdout=output,
                                stderr=subprocess.STDOUT, timeout=120)
    except subprocess.TimeoutExpired:
        output.write("TIMEOUT: renderer exceeded 120 seconds\n")
        raise SystemExit(124)
raise SystemExit(result.returncode)
PY
}
case_index=0
for scenario in kick tom1 tom2 fm retrigger long overlap snare clap cymbal hihat openhat; do
  case_index=$((case_index + 1))
  case_start=$SECONDS
  echo "CASE $case_index/12 START $scenario"
  experimental_status=0
  reference_status=0
  run_renderer "$work/experimental-$scenario.log" bash \
    tests/run_native_drumvoice_render.sh "$work/experimental-$scenario.pcm" "$scenario" \
    || experimental_status=$?
  (cd "$work/reference" && run_renderer "$work/reference-$scenario.log" bash \
    tests/run_native_drumvoice_render.sh "$work/reference-$scenario.pcm" "$scenario") \
    || reference_status=$?
  if [[ "$experimental_status" -eq 124 || "$reference_status" -eq 124 ]]; then
    echo "FAIL $scenario: renderer timeout; logs in $work" >&2; exit 1
  fi
  if [[ "$experimental_status" -ne "$reference_status" ]]; then
    echo "FAIL $scenario: renderer exits $experimental_status vs $reference_status" >&2; exit 1
  fi
  if [[ "$scenario" == overlap ]]; then
    if [[ "$experimental_status" -ne 1 ]] || \
       ! grep -Eq 'clipped=[1-9][0-9]*' "$work/experimental-$scenario.log"; then
      echo "FAIL overlap: expected observed host-sum clipping" >&2; exit 1
    fi
  elif [[ "$experimental_status" -ne 0 ]]; then
    echo "FAIL $scenario: renderer exit $experimental_status" >&2; exit 1
  fi
  python3 - "$scenario" "$work/experimental-$scenario.pcm" "$work/reference-$scenario.pcm" <<'PY'
from array import array
from pathlib import Path
import sys
if sys.byteorder != "little":
    raise SystemExit("FAIL: PCM comparator requires little-endian x86 host")
case, experimental, reference = sys.argv[1:]
a, b = array("h"), array("h")
a.frombytes(Path(experimental).read_bytes())
b.frombytes(Path(reference).read_bytes())
if len(a) != 4096 or len(b) != 4096:
    raise SystemExit(f"FAIL {case}: sample counts {len(a)} vs {len(b)}")
differences = [(i, abs(x - y)) for i, (x, y) in enumerate(zip(a, b)) if x != y]
print(f"{case}: samples=4096 mismatches={len(differences)} "
      f"first={differences[0][0] if differences else 'none'} "
      f"max_abs={max((value for _, value in differences), default=0)}")
if differences:
    raise SystemExit(1)
PY
  echo "CASE $case_index/12 DONE $scenario elapsed=$((SECONDS - case_start))s"
done
echo "PASS: 12 original OFF-path scenarios, zero PCM mismatches; overlap host-sum clipping documented."
