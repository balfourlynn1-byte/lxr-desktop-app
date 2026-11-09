#!/usr/bin/env python3
"""H03 SD extension serialization contract; static source guard, not SD emulation."""
from pathlib import Path

source = (Path(__file__).resolve().parents[1] / "Core/Src/Preset/presetManager.c").read_text(encoding="latin1")

def body(start: str, end: str) -> str:
    assert source.count(start) == 1, f"ambiguous start: {start}"
    return source.split(start, 1)[1].split(end, 1)[0]

save = body("FRESULT preset_saveDrumsetSD(uint8_t presetNr, uint8_t isMorph)", "static void preset_writeGlobalData()")
load = body("FRESULT preset_loadDrumsetSD(uint8_t presetNr, uint8_t isMorph)", "void preset_morph(")
ram = body("void preset_saveDrumsetRam(uint8_t presetNr, uint8_t isMorph)", "FRESULT preset_saveDrumsetSD(")
assert "if(isMorph)" in ram and "else" in ram
assert "memcpy(jb_kits[presetNr],jb_parameters,JB_KIT_BYTES);" in ram.rsplit("else", 1)[1]
assert "jb_pack(record,isMorph?&jb_parameters[0][0]:jb_kits[presetNr]);" in save
assert "res=f_write((FIL*)&SDFile,record,sizeof(record),&bytesWritten);" in save
assert "if(res==FR_OK && !isMorph)" in load
assert "jb_unpack(jb_kits[presetNr],record,got);" in load
assert "preset_saveDrumsetSD(i, 0)" in source
assert "preset_loadDrumsetSD(i, 0)" in source
print("PASS H03 source contract: normal SD kit extension round-trip is wired; morph SD extension is written but not loaded")
