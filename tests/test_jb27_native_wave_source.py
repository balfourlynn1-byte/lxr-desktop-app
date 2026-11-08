#!/usr/bin/env python3
"""JB-27 real firmware integration guard (not an ARM CPU benchmark)."""
from pathlib import Path
r=Path(__file__).resolve().parents[1]
def read(p):return (r/p).read_text(encoding="latin1")
osc=read("Core/Src/Soundengine/DSPAudio/Oscillator.c")
h=read("Core/Inc/Soundengine/DSPAudio/Oscillator.h")
mod=read("Core/Src/Soundengine/DSPAudio/modulationNode.c")
native=read("Core/Src/Soundengine/DSPAudio/JimbonesNativeWave.c")
assert "uint32_t waveMorphState;" in h
assert "jb_native_periodic_morph_block(osc,buf,size,gain,morph)" in osc
assert "__atomic_load_n(&osc->waveMorphState" in osc
assert "if(morph&JB_NATIVE_WAVE_MORPH_ON)" in osc
assert "switch(osc->waveform)" in osc
assert "calcWavetableOscBlock(osc,sawTable,buf,size, gain)" in osc
assert "jb_native_wave_reset_all();" in mod
assert "jb_native_wave_pack(newValue,jb_getWaveInterpolation())" in mod
assert "for(uint8_t i=0u;i<3u;i++)" in mod
for selector in ("PAR_OSC_WAVE_DRUM1","PAR_OSC_WAVE_DRUM2","PAR_OSC_WAVE_DRUM3",
                 "PAR_OSC_WAVE_SNARE","PAR_WAVE1_CYM","PAR_WAVE1_HH",
                 "PAR_MOD_WAVE_DRUM1","PAR_MOD_WAVE_DRUM2","PAR_MOD_WAVE_DRUM3",
                 "PAR_WAVE2_CYM","PAR_WAVE3_CYM","PAR_WAVE2_HH","PAR_WAVE3_HH"):
    assert selector in mod,selector
assert "const float pos=normalized*6.f" in native
assert "if(!(pos<3.f))return 0u" in native
assert "Src/Soundengine/DSPAudio/JimbonesNativeWave.c" in read("Core/CMakeLists.txt")
assert "#define JB_METALLIC_ENABLED 0" in read("Core/Inc/Soundengine/DSPAudio/Jimbones.h")
print("PASS JB-27 native LFO wave scan across six original voice destinations; no FM layer or extra waveform ROM")
