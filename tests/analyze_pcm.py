#!/usr/bin/env python3
"""Host-only PCM diagnostics: raw little-endian signed 16-bit mono."""
import argparse
from array import array
import hashlib
import json
import math
from pathlib import Path
import sys

def analyze(path, threshold=32760):
    data = Path(path).read_bytes()
    if not data or len(data) % 2:
        raise ValueError(f"{path}: empty or odd-length 16-bit PCM")
    samples = array("h")
    samples.frombytes(data)
    if sys.byteorder != "little":
        samples.byteswap()
    peak = max(abs(int(x)) for x in samples)
    near = sum(abs(int(x)) >= threshold for x in samples)
    endpoints = sum(x in (-32768, 32767) for x in samples)
    run = longest = 0
    for x in samples:
        if abs(int(x)) >= threshold:
            run += 1
            longest = max(longest, run)
        else:
            run = 0
    return {
        "file": str(path), "samples": len(samples),
        "sha256": hashlib.sha256(data).hexdigest(),
        "peak_absolute": peak,
        "peak_dbfs": round(20 * math.log10(peak / 32768), 4) if peak else None,
        "rms": round((sum(int(x)*int(x) for x in samples)/len(samples))**0.5, 4),
        "nonzero": sum(x != 0 for x in samples),
        "near_rail_samples": near, "near_rail_longest_run": longest,
        "exact_endpoint_samples": endpoints
    }

def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("pcm", nargs="+", type=Path)
    p.add_argument("--near-rail", type=int, default=32760)
    p.add_argument("--fail-on-rail", action="store_true")
    args = p.parse_args()
    if not 1 <= args.near_rail <= 32768:
        p.error("near-rail must be 1..32768")
    try:
        results = [analyze(path, args.near_rail) for path in args.pcm]
    except (OSError, ValueError) as exc:
        p.exit(2, f"ERROR: {exc}\n")
    print(json.dumps({"near_rail_threshold": args.near_rail, "results": results,
                      "caveat": "Near-rail samples are not proof of clipping; inspect pre-clamp accumulation."}, indent=2))
    return int(args.fail_on_rail and any(r["near_rail_samples"] for r in results))

if __name__ == "__main__":
    sys.exit(main())
