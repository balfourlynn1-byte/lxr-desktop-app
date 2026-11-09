#!/usr/bin/env python3
from pathlib import Path
s=Path("Core/Src/Soundengine/DSPAudio/DrumVoice.c").read_text()
h=Path("Core/Inc/Soundengine/DSPAudio/DrumVoice.h").read_text()
assert "static uint8_t jbScanEnabled[NUM_DRUM_VOICES];" in s
assert "jbScanNoteVelocity[voiceNr] = vol;" in s
assert "void jb_drumScanConfigure(" in s and "void jb_drumScanConfigure(" in h
assert "jbScanEnabled[voice] = 0;" in s
start=s.index("void calcDrumVoiceSyncBlock(")
render=s[start:s.index("//calc transient sample",start)]
assert "if (jbScanEnabled[voiceNr] && jbScanTables[voiceNr])" in render
assert "jb_calcNativeWaveScanBlock(" in render
assert "calcNextOscSampleBlock(&voiceArray[voiceNr].osc,buf,size" in render
assert "calcNextOscSampleFmBlock(&voiceArray[voiceNr].osc,modBuf,buf,size" in render
assert "retrigger_smoother_process(&voiceArray[voiceNr].retriggerSmooth" in s
print("C06 source guard PASS: default OFF, captured velocity, native/fm fallback and smoother retained")
