#!/usr/bin/env python3
"""LCD screen ownership source checks; does not simulate physical OLED."""
from pathlib import Path
r=Path(__file__).resolve().parents[1]
lcd=(r/"Core/Src/menu/menu.c").read_text(encoding="latin1")
saver=(r/"Core/Src/menu/screensaver.c").read_text(encoding="latin1")
assert "refresh_latched_char=(uint8_t)editDisplayBuffer[refresh_i][refresh_j]" in lcd
assert "lcd_data_nonBlock(refresh_latched_char)" in lcd
assert "currentDisplayBuffer[refresh_i][refresh_j]=(char)refresh_latched_char" in lcd
assert "lcd_data_nonBlock(editDisplayBuffer[refresh_i][refresh_j])" not in lcd
assert "if(screensaver_Active)return;" in lcd
assert "SKULL_DRAIN_MENU" in saver
assert "sendDisplayBuffer()==0u" in saver
assert "lcd_generatechar_nonBlock((uint8_t)(4u+glyph_index)" in saver
assert "SKULL_EXIT_CLEAR" in saver
assert "menu_repaintAll()" in saver
assert "while(sendDisplayBuffer()" not in saver
assert "while(screensaver_process()" not in saver
assert "lcd_data_nonBlock(4u)" in saver
assert "lcd_data_nonBlock(7u)" in saver
assert "Src/menu/JimbonesSkull.c" in (r/"Core/CMakeLists.txt").read_text()
print("PASS JB-26 source: stable LCD nibbles, non-blocking skull, dedicated LCD owner, clean wake repaint, no blocking loop")
