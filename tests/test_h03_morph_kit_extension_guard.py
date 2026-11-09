#!/usr/bin/env python3
"""H03 source regression: morph saves must not overwrite native kit extension.

This checks source control flow, not a RAM/SD behavioral round trip.
"""
from pathlib import Path
import re

source = (Path(__file__).resolve().parents[1] /
          "Core/Src/Preset/presetManager.c").read_text(encoding="latin1")
start = source.index("void preset_saveDrumsetRam(")
end = source.index("FRESULT preset_saveDrumsetSD(", start)
body = source[start:end]
assert body.count("memcpy(jb_kits[presetNr],jb_parameters,JB_KIT_BYTES);") == 1
assert "if(isMorph)" in body
assert "mainParameters[presetNr][i] = value;" in body
assert re.search(
    r"\}\s*else\s*\{\s*"
    r"memcpy\(jb_kits\[presetNr\],jb_parameters,JB_KIT_BYTES\);"
    r"\s*//copy edit buffer to parameter array"
    r"\s*memcpy\(mainParameters\[presetNr\], activeParameters,",
    body
), "native kit extension must be written only on normal kit save"
print("PASS H03 source guard: morph save preserves existing native kit extension")
