#!/usr/bin/env python3
"""C08 source guard: no shared wavetable-bank pointer across drum voices."""
from pathlib import Path
s=Path("Core/Src/Soundengine/DSPAudio/DrumVoice.c").read_text()
assert "static const int16_t (*jbScanTables[NUM_DRUM_VOICES])[256];" in s
assert "jbScanTables[voice] = 0;" in s
assert "jbScanTables[voice] = tables;" in s
assert "jbScanEnabled[voiceNr] && jbScanTables[voiceNr]" in s
assert "jb_calcNativeWaveScanBlock(&voiceArray[voiceNr].osc, jbScanTables[voiceNr]," in s
assert "static const int16_t (*jbScanTables)[256]" not in s
print("PASS C08: independent per-voice bank references and disable clears reference (SOURCE ONLY)")
