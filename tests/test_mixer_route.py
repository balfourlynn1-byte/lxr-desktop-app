#!/usr/bin/env python3
"""Static source audit of production native six-voice mixer routing.
Not a PCM/no-clip hardware test.
"""
from pathlib import Path
s=(Path(__file__).resolve().parents[1]/"Core/Src/Soundengine/DSPAudio/mixer.c").read_text(encoding="latin1")
for v in range(6):
    assert f"renderLayer({v}, sampleData," in s, f"missing layer voice {v}"
    assert f"mixer_decimateBlock({v},sampleData);" in s, f"missing decimation {v}"
    assert f"mixer_addDataToOutput(mixer_audioRouting[{v}]," in s, f"missing output routing {v}"
assert "FxBus_process(fxBusData," in s
assert "jb_audioDiagScan(output,output2,OUTPUT_DMA_SIZE);" in s
assert s.index("FxBus_process(fxBusData,") < s.index("jb_audioDiagScan(output,output2,OUTPUT_DMA_SIZE);")
print("PASS: six native voice routes, FX bus, post-FX diagnostic scan")
