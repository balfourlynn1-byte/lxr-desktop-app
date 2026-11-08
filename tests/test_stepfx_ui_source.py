#!/usr/bin/env python3
"""Source-level routing guard: not a hardware LED illumination test."""
from pathlib import Path
root = Path(__file__).resolve().parents[1]
def read(p): return (root / p).read_text(encoding="latin1")
button = read("Core/Src/buttonHandler.c")
led = read("Core/Src/ledHandler.c")
menu = read("Core/Src/menu/menu.c")
midi = read("Core/Src/Soundengine/MIDI/MidiVoiceControl.c")
assert "case 0:" in led and "ledNr = LED_STEP1;" in led
assert "menu_stepFxActive()<16u" in button
assert "led_setValue(1, led_idx2StepLed(menu_stepFxActive()))" in button
assert "led_setValue(1,led_idx2StepLed(menu_stepFxActive()))" in button
assert "menu_stepFxSelect(seqButtonPressed)" in button
assert "if(menu_stepFxPageActive() && jb_perfui_select(stepIndex))" in menu
assert "jb_perfui_focus(" in menu
assert "stepfx_send_set_mask(" in menu
assert "stepfx_send_adjust_division(" in menu
assert "stepfx_send_on_note(voice, vel, seq_getBpm())" in midi
assert "if(seqButtonPressed==0u && !buttonHandler_getShift())" in button
print("PASS JB-21 source guards: button 1 maps LED_STEP1, armed-only LEDs, send controls, ordinary MIDI/pad triggers, diagnostics reset separate")
