#!/usr/bin/env python3
from pathlib import Path
cm=Path("Core/CMakeLists.txt").read_text()
drum=Path("Core/Src/Soundengine/DSPAudio/DrumVoice.c").read_text()
hdr=Path("Core/Inc/Soundengine/DSPAudio/DrumVoice.h").read_text()
assert "Src/Soundengine/DSPAudio/JimbonesNativeWaveBank.c" in cm
assert "Src/Soundengine/DSPAudio/DrumVoice.c" in cm
assert "Src/Soundengine/DSPAudio/Oscillator.c" in cm
assert '#include "Soundengine/DSPAudio/JimbonesNativeWaveBank.h"' in drum
assert "jb_drumScanConfigure(jb_builtinWaveBank, voice, enabled," in drum
assert "void jb_drumScanConfigureBuiltin(" in hdr
print("PASS C11: ROM bank registered in desktop CMake and opt-in voice API connected (source check)")
