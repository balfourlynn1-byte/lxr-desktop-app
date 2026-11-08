# JB-25 — Five live dub send timing pads (JIMBONES v1.8.05)

**Status:** EXPERIMENTAL. Built on v1.8.04 JB-23, keeping previous real-device build unchanged. Host, ARM and actual LXR-02 test results are separate gates.

## Why five pads?

User requested the first **four** Step FX buttons but named **five** timings. This implementation allocates **buttons 1–5** in the requested order so none is omitted, with all remaining effects starting at button 6. The intended triplet is the **eighth-note triplet** (three repeats per quarter-note beat pair, i.e. 1/3 of a quarter note).

| Physical Step FX button | Dub echo repeat time | Musical denominator | Internal division |
|---|---|---|---|
| **1** | Quarter note | 1/4 | 2 |
| **2** | Eighth note | 1/8 | 1 |
| **3** | Eighth-note triplet | 1/8T | 4 |
| **4** | Sixteenth note | 1/16 | 0 |
| **5** | Thirty-second note | 1/32 | 5 |
| **6–16** | Original trigger-based Step FX profiles | — | Original slots 5–15 |

These are **repeat intervals of the existing single global FX delay**, NOT independent new delay buffers and NOT changes to the sample snippet gate length. The Step FX menu continues to use **knob 2** for gate duration and **SHIFT + knob 2** as an advanced manual override of the echo interval; pressing a different dub timing pad reasserts its preset.

## Playing behaviour

- Enter Performance → Step FX page. Press **1–5** to activate that dub timing. The physical LED should track only the armed button. Same pad again = OFF; choosing a different pad switches without requiring an intermediate OFF press.
- Exactly **one** effect is active at a time, conserving CPU. Buttons **6–16** retain original Step FX repeat/stutter profiles and deactivate dub sends. No global Step 1 master-OFF semantics.
- Step FX pad 1–5 turns on the original JIMBONES **parallel dub send**: dry original voice remains on existing hardware route; only selected non-kick voices send snippets into the single existing global FX DELAY. Kick excluded by default; first-use send level conservative (64/127).
- The OLED shows the **focused** button and its timing even when knob browsing, and does not accidentally arm it. Active timing and status is marked by `*`. `OK` means currently supported; `LONG`, `SHORT`, `NOFX` and `CLK` suppress new sends, rather than wrapping invalid delay memory.
- At **120 BPM**, mono quarter note is 0.5 seconds, eighth 0.25, eighth-triplet ~0.167, sixteenth 0.125, thirty-second 0.0625. **At slower tempos or with ping-pong mode some divisions will not fit the original 32K delay buffer**: screen shows LONG and no new send is launched. At extremely high tempo 1/32 can be shorter than the delay engine's 20ms minimum: screen shows SHORT. No silently wrong timings.
- Changing pads while a delay crossfade is running queues the latest timing for the next free block. Does not create a second processor, kill existing echo buffer or force unsafe immediate ring index changes.
- The shared wet-only return retains existing 4-second max feedback-tail hold when switching OFF, existing capped dub feedback, tempo-clock safety and unchanged native six voices.

## Verification

- Production Step FX UI tested against production DubDelaySync control adapter (real 5-button mapping, press ON/OFF, exclusive slots 6–16, kick protection, dry-buffer parity).
- Production delay planner host test adds 1/32 mapping, unsupported high BPM, invalid divisions, and queued pad switches when Delay.c is crossfading.
- Full host GCC + Clang suites plus original triggered Step FX regression.
- ARM cross-link and **at least 512 bytes free ITCM**; correct v1.8.05 firmware image and checksum. CI `JB25-UNVERIFIED-TEST-ONLY` artifact only on green.
- Still requires LXR-02 testing: check actual Step 1–5 LEDs, tap between divisions while echo plays, 4-pad overlapping hits, DSP/PK/LATE/RAIL and speaker-safe levels. User reported v1.8.04 dub effects work well and Metallic-FM-era DSP crunch apparently gone; **neither claim is a v1.8.05 validation**.

**Rollback:** Keep the successfully booted previous `LXR_V2_V1.8.04.img` with its checksum in a separate location, back up SD card kits, and read `jb23_rollback_and_install.md`. Firmware package/CI does not certify physical-device safety.
