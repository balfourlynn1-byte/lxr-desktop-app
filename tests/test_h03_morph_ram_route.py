#!/usr/bin/env python3
"""H03 source-level morph RAM-vs-SD routing regression, not a persistence test."""
from pathlib import Path
import re
src = (Path(__file__).resolve().parents[1] / "Core/Src/menu/menu.c").read_text(encoding="latin1")
save = src.split("case SAVE_TYPE_MORPH:", 1)[1].split("case SAVE_TYPE_GLO:", 1)[0]
load = src.split("case SAVE_TYPE_MORPH:", 2)[2].split("case SAVE_TYPE_SONG:", 1)[0]
assert re.search(r"preset_saveDrumsetRam\s*\(\s*menu_selectedPresetNr\[SAVE_TYPE_MORPH\]\s*,\s*1\s*\)", save)
assert "preset_saveDrumsetSD(" not in save
assert re.search(r"preset_loadDrumsetRam\s*\(\s*menu_selectedPresetNr\[menu_saveOptions.category\]\s*,\s*1\s*,\s*0\s*\)", load)
assert "menu_loadedPresetNr[CURRENT_MORPH]" in load
assert "activeParameters[PAR_MORPH_KIT_DEST]" in load
print("PASS H03 source: morph menu saves/loads RAM slot 1, no direct SD save in morph case")
