#!/usr/bin/env python3
"""JB-12 source integration guard, not actual device performance proof."""
from pathlib import Path
r=Path(__file__).resolve().parents[1]
def read(p):return (r/p).read_text(encoding="latin1")
h=read("Core/Inc/Soundengine/DSPAudio/ResonantFilter.h")
c=read("Core/Src/Soundengine/DSPAudio/ResonantFilter.c")
ui=read("Core/Inc/menu/MenuText.h")
cm=read("Core/CMakeLists.txt")
assert "FILTER_NAIVE_2_POLE,\n\tFILTER_OFF,\n\tFILTER_COMB" in h.replace("\r\n","\n")
assert "JbCombFilter comb;" in h
assert c.count("if(type== FILTER_COMB)") == 2
assert c.count("jb_comb_process(&filter->comb") == 2
assert "jb_comb_init(&filter->comb);" in c
assert "jb_comb_reset(&filter->comb);" in c
assert 'const char filterTypes[][8]' in ui
start=ui.index('const char filterTypes[][8]')
section=ui[start:start+500]
assert '{9}' in section
assert section.index('{"LP"}') < section.index('{"off"}') < section.index('{"Comb"}')
assert "Src/Soundengine/DSPAudio/CombFilter.c" in cm
# Legacy type numeric IDs are unchanged, and filter page reuses FREQ and RESO.
pages=read("Core/Inc/menu/menuPages.h")
assert "PAR_FILTER_FREQ_1" in pages and "PAR_RESO_1" in pages and "PAR_FILTER_TYPE_1" in pages
print("PASS JB-12 source: 9 FILTER types, legacy OFF preserved, comb integrated across original/cheap paths, existing filter controls reused")
