#!/usr/bin/env python3
"""H03 native MIDI source contract. Source test, not audio/firmware validation."""
from pathlib import Path
import re

root = Path(__file__).resolve().parents[1]
src = (root / "Core/Src/Soundengine/MIDI/MidiParser.c").read_text(encoding="latin1")
# The upstream mod-wheel morph implementation must not be presumed to exist
# in this native JIMBONES branch; flag a future semantic change for review.
assert re.search(r"case\s+CC_MOD_WHEEL:\s*break;", src), "Native mod-wheel contract changed: review morph semantics"
routes = re.search(
    r"case CC2_VELO_MOD_AMT_1:(.*?)"
    r"velocityModulators\[msg\.data1-CC2_VELO_MOD_AMT_1\]\.amount\s*=\s*value\.flt;",
    src, re.S,
)
assert routes, "Missing six-voice velocity amount dispatch"
for i in range(2, 7):
    assert f"case CC2_VELO_MOD_AMT_{i}:" in routes.group(1), f"Missing voice {i} amount route"
assert "value.itg/127.f" in src, "Normalized MIDI CC scaling changed"
print("PASS H03 native MIDI source contract: mod-wheel no-op; six velocity amount routes")
