#!/usr/bin/env python3
"""H03 source guard for morph slot save/load separation; not a preset round trip."""
from pathlib import Path
import re
src=(Path(__file__).resolve().parents[1]/"Core/Src/menu/menu.c").read_text(encoding="latin1")
def branch(label, following):
    start=src.index(label)
    end=src.index(following,start)
    return src[start:end]
save_start=src.rfind("case SAVE_TYPE_MORPH:", 0, src.index("preset_saveDrumsetRam(menu_selectedPresetNr[SAVE_TYPE_MORPH],1)"))
save=src[save_start:src.index("case SAVE_TYPE_GLO:",save_start)]
assert re.search(r"preset_saveDrumsetRam\s*\(\s*menu_selectedPresetNr\[SAVE_TYPE_MORPH\]\s*,\s*1\s*\)",save)
load=src[src.index("case SAVE_TYPE_MORPH:",src.index("preset_loadDrumset(menu_selectedPresetNr")):]
load=load[:load.index("case SAVE_TYPE_SONG:")]
assert "preset_loadDrumsetRam(menu_selectedPresetNr[menu_saveOptions.category],1, 0)" in load
assert "activeParameters[PAR_MORPH_KIT_DEST] = menu_selectedPresetNr[menu_saveOptions.category]" in load
assert "menu_loadedPresetNr[CURRENT_MORPH] = menu_selectedPresetNr[menu_saveOptions.category]" in load
assert "preset_getMorphValue(paramNr, morph)" in src
assert "if((paramNr >= PAR_AUDIO_OUT1) && (paramNr <= PAR_AUDIO_OUT6))" in src
print("PASS H03 source contract: morph save/load use buffer 1 and preserve audio routing")
