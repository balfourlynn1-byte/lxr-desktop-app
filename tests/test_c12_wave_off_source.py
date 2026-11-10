#!/usr/bin/env python3
"""C12 structural OFF-path guard, NOT an audio identity test."""
from pathlib import Path
import re
src=Path("Core/Src/Soundengine/DSPAudio/DrumVoice.c").read_text()
# Default zero-initialized static array; no initializer silently enables the path.
assert re.search(r"static\s+uint8_t\s+jbScanEnabled\s*\[\s*NUM_DRUM_VOICES\s*\]\s*;",src)
# Explicit enable requires a non-null table, and disable clears both state and pointer.
configure=src.split("void jb_drumScanConfigure(",1)[1].split("void jb_drumScanConfigureBuiltin(",1)[0]
# Reject out-of-range voice indices before any scan-state array access.
assert re.search(r"if\\s*\\(voice\\s*>=\\s*NUM_DRUM_VOICES\\)\\s*return\\s*;",configure)
assert configure.index("if (voice >= NUM_DRUM_VOICES) return;") < configure.index("jbScanEnabled[voice] = 0;")
assert re.search(r"jbScanEnabled\[voice\]\s*=\s*0\s*;",configure)
assert re.search(r"jbScanTables\[voice\]\s*=\s*0\s*;",configure)
assert re.search(r"if\s*\(!tables\s*\|\|\s*!enabled\)\s*return\s*;",configure)
assert re.search(r"jbScanEnabled\[voice\]\s*=\s*1\s*;",configure)
# Existing native oscillator call must remain the OFF branch.
assert re.search(r"if\s*\(jbScanEnabled\[voiceNr\]\s*&&\s*jbScanTables\[voiceNr\]\).*?jb_calcNativeWaveScanBlock\(.*?else\s+calcNextOscSampleBlock\(&voiceArray\[voiceNr\]\.osc,buf,size,",src,re.S)
# No implicit enable on init/trigger.
for function in ("initDrumVoice","Drum_trigger"):
    body=src.split("void "+function+"(",1)[1]
    body=body[:body.find("\n}\n")+2] if "\n}\n" in body else body
    assert "jb_drumScanConfigureBuiltin(" not in body
    assert "jbScanEnabled[" not in body
print("PASS C12: default OFF and legacy oscillator fallback structurally retained; PCM identity NOT TESTED")
