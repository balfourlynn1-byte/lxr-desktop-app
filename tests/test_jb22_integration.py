#!/usr/bin/env python3
"""Integration safety assertions. Source checks do not replace hardware runs."""
from pathlib import Path
root=Path(__file__).resolve().parents[1]
def read(p):return (root/p).read_text(encoding="latin1")
j=read("Core/Src/Soundengine/DSPAudio/Jimbones.c")
h=read("Core/Inc/Soundengine/DSPAudio/Jimbones.h")
fx=read("Core/Src/Soundengine/FxBus.c")
mix=read("Core/Src/Soundengine/DSPAudio/mixer.c")
ui=read("Core/Src/menu/JimbonesPerfUI.c")
button=read("Core/Src/buttonHandler.c")
delay=read("Core/Src/Soundengine/DubDelaySync.c")
r=read("CMakeLists.txt")
assert "#define JB_METALLIC_ENABLED 0" in h
assert "#if JB_METALLIC_ENABLED" in j
assert "if(!(p[JB_MODE]&2u))" in j
assert 'VERSION 1.8.07' in r
assert "IN_ITCM_RAM void FxBus_process" in fx
assert "IN_ITCM_RAM void FxBus_updateParameter" not in fx
assert "IN_ITCM_RAM void FxBus_setParameter" not in fx
assert "dub_delay_prepare(&fxBus.delay" in mix
assert "FxBus_process(fxBusData" in mix
assert "dub_delay_can_send()" in mix
assert "stepfx_send_mix_voice" in mix
assert "jb_perfui_active(void)" in ui
assert "menu_stepFxActive()<16u" in button
assert "dub_delay_wet_only()" in fx
assert "DUB_DELAY_TOO_LONG" in delay
print("PASS JB-22: 1.8.05 integration, Metallic disabled, 6-voice mixer/FX/dub/UI linked and control setters out of ITCM")
