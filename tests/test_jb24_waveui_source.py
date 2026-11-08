#!/usr/bin/env python3
"""Source guards for OSC page wiring, not actual device display validation."""
from pathlib import Path
root=Path(__file__).resolve().parents[1]
def read(p):return (root/p).read_text(encoding="latin1")
ui=read("Core/Src/menu/menu.c")
wave=read("Core/Src/menu/JimbonesWaveUI.c")
assert "jb_waveui_param(jb_selected)" in ui
assert "jb_waveui_col(bank,(uint8_t)col)" in ui
assert "jb_waveui_next(jb_selected,inc)" in ui
assert "jb_waveui_col((uint8_t)(jb_selected/4u),potNr)" in ui
assert "jb_selected==JB_WAVEUI_INTERP" in ui
assert "jb_selected=JB_WAVEUI_INTERP" in ui
assert "jb_selected==JB_PARAMS" not in ui
assert "jb_uiAdjust(jb_selected,inc)" in ui
assert "jb_setControl(jb_uiVoice(),JB_MODE,diff>0?2u:0u)" in ui
assert 'snprintf(b,sizeof(b),"V%u WAVE %s"' in ui
assert "jb_waveui_shape_name(" in ui
assert "#include \"menu/JimbonesWaveUI.h\"" in ui
assert "Src/menu/JimbonesWaveUI.c" in read("Core/CMakeLists.txt")
for fm in ["JB_METAL_LEVEL","JB_METAL","JB_VMETAL","JB_RATIO","JB_DEPTH","JB_FEEDBACK"]:
    assert fm not in wave, fm
for label in ['"Metal level"','"FM ratio"','"FM depth"','"FM feedback"','"Velocity > Metal"']:
    assert label not in ui, label
print("PASS JB-24 source: wave-first UI, no metallic controls, safe legacy modes, oscillator interpolation and kit IDs")
