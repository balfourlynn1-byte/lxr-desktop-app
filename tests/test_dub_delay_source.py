#!/usr/bin/env python3
"""Source mapping for delay connection, not a device audio simulation."""
from pathlib import Path
r=Path(__file__).resolve().parents[1]
def read(path):return (r/path).read_text(encoding="latin1")
m=read("Core/Src/Soundengine/DSPAudio/mixer.c")
f=read("Core/Src/Soundengine/FxBus.c")
ui=read("Core/Src/menu/menu.c")
d=read("Core/Src/Soundengine/DubDelaySync.c")
assert "dub_delay_prepare(&fxBus.delay" in m
assert "dub_delay_can_send()," in m
assert m.index("dub_delay_prepare(&fxBus.delay") < m.index("mixer_stepfx_send(0,")
assert "dub_delay_wet_only()?1.f:fxBus.dryWet" in f
assert "dub_delay_mark_dirty();" in f
assert "Delay_setRange(d,1.f)" in d
assert "Delay_setDelayTime(d,normalized)" in d
assert "dub_delay_status_name()" in ui
assert "dub_delay_adjust_division(diff)" in ui
assert "if(buttonHandler_getShift())" in ui
assert "Delay_setFeedback(d,feedback)" in d
assert "if(fx_is_delay)" in d
print("PASS JB-20 source wiring: delay before send; wet-only return, status UI, explicit sample bounds and feedback safeguards")
