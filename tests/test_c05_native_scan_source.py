#!/usr/bin/env python3
"""C05 source integration guard; does not compile or run the native engine."""
from pathlib import Path
source=Path("Core/Src/Soundengine/DSPAudio/Oscillator.c").read_text()
header=Path("Core/Inc/Soundengine/DSPAudio/Oscillator.h").read_text()
assert source.count("void jb_calcNativeWaveScanBlock(")==1
assert header.count("void jb_calcNativeWaveScanBlock(")==1
start=source.index("void calcNextOscSampleBlock(")
end=source.index("//-----------------------------------------------------------",start)
legacy=source[start:end]
assert "jb_calcNativeWaveScanBlock" not in legacy
assert "switch(osc->waveform)" in legacy
assert "osc->phase += osc->phaseInc;" in source[source.index("void jb_calcNativeWaveScanBlock("):start]
assert "jb_wave_velocity_position" in source
print("C05 source guard PASS: opt-in native renderer, legacy dispatch unchanged")
