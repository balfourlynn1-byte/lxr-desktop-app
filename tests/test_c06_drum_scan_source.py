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
# Reinitializing the engine must clear all opt-in scan state before normal
# per-voice setup. This is a source guard, NOT a runtime/audio regression.
init=s[s.index("void initDrumVoice()"):s.index("void Drum_updatePan(")]
assert "for(i=0;i<NUM_DRUM_VOICES;i++)" in init
assert init.index("jbScanEnabled[i] = 0;") < init.index("initOnePole(&voiceArray[i].FaderVolumeFilter)")
for name, value in (
    ("jbScanEnabled", "0"),
    ("jbScanTables", "0"),
    ("jbScanNoteVelocity", "0"),
    ("jbScanBase", "0.f"),
    ("jbScanVelocityDepth", "0.f"),
    ("jbScanLfoDepth", "0.f"),
    ("jbScanLfoValue", "0.f"),
):
    reset = f"{name}[i] = {value};"
    assert init.count(reset) == 1, f"{name}: expected exactly one reset in initDrumVoice"
    # All resets must occur inside the per-voice loop and before any
    # pre-existing voice setup. An assignment elsewhere is not sufficient.
    assert init.index("for(i=0;i<NUM_DRUM_VOICES;i++)") < init.index(reset), name
    assert init.index(reset) < init.index("initOnePole(&voiceArray[i].FaderVolumeFilter)"), name
print("C06 source guard PASS: default OFF, captured velocity, native/fm fallback and smoother retained")
