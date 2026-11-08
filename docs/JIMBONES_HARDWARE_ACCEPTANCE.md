# JIMBONES hardware acceptance checklist — one candidate at a time

**Candidate:** ______  **Source SHA:** ______  **Image SHA256:** ______  **Device booted:** YES / NO

**Status meanings:** CODE_READY = CI passed; BOOTED = firmware started; AUDIBLE_PASS = expected feature heard; STRESS_PASS = measured realtime stability; ACCEPTED = user confirms all relevant checks. Never infer one from another.

| Check | User observation | Pass/Fail/Not tested |
|---|---|---|
| Rollback image and kits backed up | | |
| Boot and original six voices sound as before | | |
| Existing Step FX effects still audible | | |
| Existing dub delay still audible | | |
| Step 1 LED and toggle on/off behave correctly | | |
| Comb selectable as ninth filter type | | |
| Comb clearly audible with Reso above zero | | |
| Comb Reso=0 bypass sounds unchanged | | |
| Comb works on snare, clap, toms, hats and kick individually | | |
| Five dub timing controls (if present in candidate) | | |
| Skull animation and LCD clean wake (if present) | | |
| Existing kits load/save correctly | | |
| No crackle, dropouts, clipping or stuck notes | | |

**Performance readings:** record `DSP / PK / LATE / RAIL` for idle, two overlapping toms, four simultaneous hits, four hits + Comb, four hits + dub. Include whether sequencer is running; RAIL may increment in that condition, so note the exact reproduction. Avoid assuming thresholds guarantee sound quality.

**Bug report (one issue each):** image SHA, exact page/button/knob actions, expected sound/display, observed result, repeatability, DSP/PK/LATE/RAIL, short audio/video if available. Do not bundle unrelated fixes into the same PR.

**Release decision:** NOT TESTED / REJECTED (bug IDs) / ACCEPTED BY USER (date and observations). Keep known-good image unchanged.
