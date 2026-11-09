#!/usr/bin/env python3
"""Compare the factory OFF main-oscillator call and FM path with integration baseline.
Source-level equivalence only: this does NOT prove sample-identical audio.
"""
import pathlib
import re
import subprocess

path = "Core/Src/Soundengine/DSPAudio/DrumVoice.c"
current = pathlib.Path(path).read_text()
baseline = subprocess.check_output(["git", "show", "refs/remotes/origin/agents/jb-28-full-integration-v186:" + path], text=True)
def calls(source, fn):
    return [re.sub(r"\\s+", "", x) for x in re.findall(r"\\b" + fn + r"\\s*\\([^;]*?\\);", source, flags=re.S)]
for fn in ("calcNextOscSampleBlock", "calcNextOscSampleFmBlock", "bufferTool_addBuffersSaturating", "transient_calcBlock"):
    old = calls(baseline, fn)
    new = calls(current, fn)
    assert old == new, f"{fn}: factory call differs: baseline={old}, candidate={new}"
assert "if (jbScanEnabled[voiceNr] && jbScanTables)" in current
assert "else\n            calcNextOscSampleBlock" in current
assert "jbScanEnabled[NUM_DRUM_VOICES]" in current
print("PASS: legacy oscillator/FM/mix/transient calls preserved; guarded factory OFF path (SOURCE ONLY)")
print("NOT RUN: native compiled engine or PCM sample-identity comparison")
