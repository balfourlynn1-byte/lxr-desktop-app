#!/usr/bin/env python3
"""Static integration guard for six dub sends in the production native mixer."""
from pathlib import Path
s=(Path(__file__).resolve().parents[1]/"Core/Src/Soundengine/DSPAudio/mixer.c").read_text(encoding="latin1")
for voice in range(6):
    gate=f"fxBus.fxType==FX_DELAY && fxBus.dryWet>=0.999f && mixer_audioRouting[{voice}]!=MIXER_ROUTING_FX_BUS"
    tap=f"stepfx_dubTap({voice},sampleData,fxBusData,OUTPUT_DMA_SIZE);"
    assert gate in s, f"missing FX wet-only guard for voice {voice}"
    assert tap in s, f"missing dub tap for voice {voice}"
assert s.count("fxBus.dryWet>=0.999f")==6
print("PASS: six native dub taps gated on fully wet delay and no FX-only double routing")
