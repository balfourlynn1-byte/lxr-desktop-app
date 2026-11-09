#!/usr/bin/env python3
"""Source wiring guard only; physical LED behavior remains unverified."""
from pathlib import Path
r = Path(__file__).resolve().parents[1]
button = (r / 'Core/Src/buttonHandler.c').read_text(encoding='latin1')
led = (r / 'Core/Src/ledHandler.c').read_text(encoding='latin1')
assert 'case 0:' in led and 'ledNr = LED_STEP1;' in led
assert 'seq_toggleMainStep(trackNr, seqButtonPressed + 16*menu_activeBar, patternNr);' in button
assert 'led_setValue(seq_isMainStepActive(trackNr,' in button
assert 'seqButtonPressed + 16*menu_activeBar, patternNr) ? 1u : 0u,' in button
assert 'led_idx2StepLed(seqButtonPressed));' in button
print('PASS source guard: sequencer LED mirrors armed and disarmed state including step 0')
