# JIMBONES v1.8.04 — LXR-02 hardware test record

**Firmware status: EXPERIMENTAL / HARDWARE UNVERIFIED.** Fill in only after creating a known-working rollback and backing up kits. Do not treat a CI green check as device proof.

## Identity and rollback record

| Field | Value |
|---|---|
| Source commit SHA | |
| Image SHA256 from MANIFEST.json | |
| Image SHA256 independently calculated on Mac | |
| Previously bootable working firmware file and version | |
| SHA256 of rollback image | |
| SD card backup folder on Mac | |
| Date / kit / MIDI pad mapping | |
| Headphone/monitor volume (start LOW) | |
| All 6 native voices played before update | Yes / No |
| Rollback path confirmed and image kept off the test SD | Yes / No |

**HARD STOP:** no existing bootable rollback image, missing backup of kits/SD data, checksum mismatch, wrong LXR-02 model or failed CI = do not install.

## Section A — baseline readings

Restart the device before each set. Use the same kit, pad velocities and long decay settings across measurements. Record figures (do not infer CPU numbers from memory free space).

| Test | DSP (%) | PK (%) | LATE | RAIL | Crunch/click? | Notes |
|---|---|---|---|---|---|---|
| Idle, effects OFF, wavetable OFF | | | | | | |
| Single original kick/snare/tom/hat hits | | | | | | |
| Wavetable ON, single tom | | | | | | |
| Two overlapping toms (same as v1.8.03 test) | | | | | | |
| Four overlapping pads with decays | | | | | | |
| FX DELAY selected, DUB SEND OFF | | | | | | |
| DUB SEND ON, one snare echo throw | | | | | | |
| Four overlapping hits with DUB SEND ON | | | | | | |
| Wavetable + dub send combined | | | | | | |

**Historical reference only (v1.8.03, NOT the new candidate):** idle DSP 62%, PK 66%, LATE 0, RAIL 0; two modulated toms produced crunch near DSP 94%; stress case DSP 101%, PK 105%, LATE 999, RAIL 240.

If DSP approaches the real-time deadline (near/over 100%), LATE increments, audible crackle appears, or unexpected high-level output occurs: immediately stop testing that combination and report the exact settings.

## Section B — controls, outputs, and clock

| Requirement | Pass / Fail / Not tested | Notes |
|---|---|---|
| Original six drum voices and both hi-hat articulations | | |
| Kits load/save and sound as expected | | |
| Metallic FM no longer appears in OSC extension | | |
| OSC Wav/Lvl/Pos/Mov page and seven wave pages | | |
| Smooth/stepped interpolation display and audible changes | | |
| Physical Step 1 LED illuminates when armed | | |
| Press Step 1 again turns off DUB SEND and clears LED | | |
| Steps 2–16 light and toggle consistently | | |
| Original sequencer Step 1 still works | | |
| CPU diagnostic Step 1 reset separate from Step FX | | |
| Kick protected from new send by default | | |
| Dry sound continues on original hardware output | | |
| FX bus echo audible without extra dry hit | | |
| One snare snippet enters delay; echo continues after gate off | | |
| Mono and ping-pong delay time warnings correct | | |
| Unsupported subdivisions show LONG/SHORT | | |
| External MIDI clock sync and clock stop/dropout safe | | |
| All four jack-output routings unchanged | | |
| No clicks when changing delay modes/tempo | | |
| Old kit compatibility: FM-only stored mode bypasses extra layer | | |

## Section C — conclusion

- Final DSP peak / PK / LATE / RAIL:
- Exact settings that produce any crunch or unwanted tails:
- All six voices, original sounds and rollback tested:
- Result: **PASS FOR FURTHER EXPERIMENTAL TESTING / FAIL / NOT TESTED**
- Screenshot, recording or notes:
- Any failure means **not approved for regular/live use** until fixed and retested.
