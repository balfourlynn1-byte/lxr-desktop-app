#!/usr/bin/env python3
"""H03 native morph safety/preset routing source contract (not audio simulation)."""
from pathlib import Path
import re

root = Path(__file__).resolve().parents[1]
menu = (root / "Core/Src/menu/menu.c").read_text(encoding="latin1")
def must(pattern):
    assert re.search(pattern, menu, re.S), f"Missing native morph contract: {pattern}"

# Morph loads into the alternate kit buffer, not the current source kit.
must(r"case SAVE_TYPE_MORPH:\s*#if 1\s*preset_loadDrumsetRam\([^;]*,1,\s*0\);")
must(r"activeParameters\[PAR_MORPH_KIT_DEST\]\s*=\s*menu_selectedPresetNr")
must(r"menu_loadedPresetNr\[CURRENT_MORPH\]\s*=\s*menu_selectedPresetNr")
# The menu delegates morph position to the preset engine.
must(r"case PAR_MORPH:\s*\{[^}]*preset_morph\(value\);")
# Routing must remain on source kit even while sound parameters morph.
must(r"if\(\(paramNr >= PAR_AUDIO_OUT1\).*?paramNr <= PAR_AUDIO_OUT6\).*?\{\s*morph = 0;")
must(r"morph = activeParameters\[PAR_MORPH\];\s*//morph = preset_morphValue;\s*dtypeValue = preset_getMorphValue\(paramNr, morph\);")
# Six existing velocity destinations remain connected to native modulation nodes.
must(r"modNode_setDestination\(&velocityModulators\[\(uint8_t\)\(paramNr - PAR_VEL_DEST_1\)\], value\);")
print("PASS H03 source contract: morph buffer, routing isolation, six velocity nodes")
