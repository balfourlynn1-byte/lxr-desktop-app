#!/usr/bin/env python3
"""Pre-hardware review guards: source wiring, not a substitute for audio tests."""
from pathlib import Path
root=Path(__file__).resolve().parents[1]
def src(path):
    return (root / path).read_text(encoding="utf-8")
menu=src("Core/Src/menu/menu.c")
seq=src("Core/Src/Soundengine/Sequencer/sequencer.c")
engine=src("Core/Src/Soundengine/Soundengine.c")
jb=src("Core/Src/Soundengine/DSPAudio/Jimbones.c")
assert "pageNr != menu_activePage" in menu, "OSC extension resets on same-voice reselect"
assert "jb_selected=JB_MODE" in menu, "OSC extension must open on MOD/FML/WTL/DEC"
assert "if(!seq_running || !wasRunning)" in seq and "seq_rollState = 0;" in seq, "Stale roll start path"
assert "stepfx_audioBlock();" in engine and "stepfx_init();" in engine, "Step FX not integrated"
assert "jb_audioLoadPercent" in engine and "jb_audioOverruns++;" in engine, "Audio telemetry absent"
assert "if(!p[JB_MODE])" in jb and "void jb_preFilterNoise" in jb, "Mode 0 / pre-filter guard missing"
for path,func,native_filter in [
  ("Core/Src/Soundengine/DSPAudio/DrumVoice.c","jb_preFilterNoise(voiceNr,buf,size)","SVF_calcBlockZDF"),
  ("Core/Src/Soundengine/DSPAudio/Snare.c","jb_preFilterNoise(3,buf,size)","SVF_calcBlockZDF"),
  ("Core/Src/Soundengine/DSPAudio/HiHat.c","jb_preFilterNoise(5,buf,size)","SVF_calcBlockZDF"),
  ("Core/Src/Soundengine/DSPAudio/CymbalVoice.c","jb_preFilterNoise(4,buf,size)","SVF_calcBlockZDF"),
]:
  text=src(path)
  assert func in text, f"{path}: pre-filter injection missing"
  assert text.index(func)<text.index(native_filter), f"{path}: experimental noise is not pre-filter"
print("PASS source guards: OSC state, roll reset, engine/telemetry wiring, and four native-filter injections")
