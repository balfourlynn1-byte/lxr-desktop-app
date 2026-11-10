#!/usr/bin/env bash
# P3 baseline only: measure real native DSP output without modifying firmware.
# Not a CPU timing test or a production hardware mixer simulation.
set -euo pipefail
cd "$(dirname "$0")/.."
if [[ "$(git branch --show-current)" != "agents/jb-wavetable-host-experiment" ]]; then
  echo "FAIL: experimental branch only" >&2; exit 2
fi
target="${CC:-clang}"
case "$("$target" -dumpmachine)" in
  *arm*|*thumb*|*aarch64*|*none-eabi*) echo "FAIL: host compiler required" >&2; exit 2 ;;
esac
out="${1:-$(mktemp -d "${TMPDIR:-/tmp}/jb-p3-baseline.XXXXXX")}"
mkdir -p "$out"
out="$(cd "$out" && pwd)"
echo "P3 host baseline: $(git rev-parse HEAD)"
echo "Output: $out"
for scenario in kick tom1 tom2 fm retrigger long overlap snare clap cymbal hihat openhat; do
  status=0
  bash tests/run_native_drumvoice_render.sh "$out/$scenario.pcm" "$scenario" > "$out/$scenario.log" 2>&1 || status=$?
  expected=0
  [[ "$scenario" == "overlap" ]] && expected=1
  if [[ "$status" -ne "$expected" ]]; then
    echo "FAIL $scenario: renderer exit $status, expected $expected" >&2
    cat "$out/$scenario.log" >&2
    exit 1
  fi
  python3 tests/analyze_pcm.py "$out/$scenario.pcm" > "$out/$scenario.json"
  echo "Recorded $scenario (renderer exit $status)"
done
python3 - "$out" <<'PY'
import json
from pathlib import Path
import sys
out = Path(sys.argv[1])
rows = []
for file in sorted(out.glob("*.json")):
    r = json.loads(file.read_text())["results"][0]
    rows.append((file.stem, r["peak_absolute"], r["near_rail_samples"],
                 r["exact_endpoint_samples"], r["near_rail_longest_run"]))
lines = ["scenario,peak_absolute,near_rail_samples,exact_endpoint_samples,near_rail_longest_run"]
lines.extend(",".join(map(str, row)) for row in rows)
(out / "summary.csv").write_text("\n".join(lines) + "\n")
print("\n".join(lines))
PY
echo "P3 baseline complete; compare with real mixer and hardware before inferring clipping."
