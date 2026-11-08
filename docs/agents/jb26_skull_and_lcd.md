# JB-26 — Fix LCD wake corruption and add a bouncing skull screensaver

**Status:** JIMBONES v1.8.06 EXPERIMENTAL, branch `agents/jb-26-skull-screensaver-v185` stacked on JB-25 v1.8.05. Firmware only, **no changes in the DMA audio renderer, DSP voice engines or delay buffer**. The displayed device photo is a **bug report** showing corrupt LCD characters after screensaver activity. **It is not artwork used for this skull.**

## Actual hardware and symptom

- Erica Synths LXR-02 panel has a **16×2 character OLED** with only eight programmable 5×8 custom CGRAM character slots, NOT a pixel-addressable 128×64 display. A scrolling raster GIF or full-screen bitmap would be inappropriate.
- User's device displayed scrambled symbols/letters across the top character row after going through screensaver. The screenshot demonstrates the visual symptom, not its precise root cause. LCD command/data byte corruption and uncontrolled overlap between menu refresh and screensaver were plausible from source.
- The old screensaver performed repeatedly blocking `while(sendDisplayBuffer()!=0)`, direct LCD clears, or non-blocking writes that could run while a pending normal menu operation still held the bus. The old wake handler also busy-waited with `soundengine_process()`.
- The low-level non-blocking data/command writers re-read their *input arguments* at both 4-bit nibbles of a byte, even if the menu changed the character/source buffer between calls. JB-26 now latches the pending byte at transfer start and uses that SAME byte for the low nibble.

## Change set

1. **Skull pixel model:** `Core/Src/menu/JimbonesSkull.c` and `Core/Inc/menu/JimbonesSkull.h` define original (newly created) 10×12 black-and-white pixel skull art, spread across four 5×8 programmable LCD characters. An eight-frame loop shifts the 10×12 skull from vertical y=0 to y=4 and back.
2. **Display-exclusive animation:** `Core/Src/menu/screensaver.c` runs a non-blocking state machine on the normal UI/main loop every ~170ms. The first state drains any unfinished normal-menu transfer; only then does it clear the screen, program CGRAM slots **4–7**, and draw the 2×2-character skull. It updates only the CGRAM pattern during vertical motion; occasional slow horizontal drift avoids stationary OLED burn. CGRAM slot **0** remains empty for legacy `\0` handling.
3. **Wake-up:** user input only schedules a clean exit. Once the current LCD transaction has completed, the screensaver clears the display, yields sole ownership and requests `menu_repaintAll()` so the full real menu is redrawn. **No blocking wait from button handlers** and no audio render callback allocations.
4. **Menu refresh:** `menu_tick()` yields the LCD while screensaver is active. Normal `sendDisplayBuffer()` captures the selected character once and updates the cached character with the transmitted byte, instead of possibly writing the new value into the cache after an old byte is sent.
5. **Driver nibble atomicity:** `lcd_data_nonBlock` and `lcd_command_nonblock` latch the entire pending byte when the first nibble is sent, guarding against intervening UI refresh changing the second nibble.
6. All existing waveform/Step FX/dub functionality is unchanged. Bump to distinct firmware **1.8.06**; retain user-tested **1.8.04** known-good image for rollback. JB-25 v1.8.05 is still experimental.

## Host and firmware validation

- `sh tests/run_jb26_skull.sh`: tests 8 animation frames, bounded CGRAM rows and loop, source-level LCD ownership and code path.
- `sh tests/run_jb26_screensaver_state.sh`: compiles the **production screensaver state machine**, simulates menu transfer, CGRAM installation, animation, wake-up and reentry with a mock LCD, checks no writes before bus is drained and exactly one menu redraw on exit.
- `sh tests/run_host.sh`: all existing host/GCC and Clang audio, menu, Step FX, firmware tests remain.
- `.github/workflows/jimbones-jb26-skull.yml`: ARM cross-compile, >=512 bytes free ITCM, updater-image integrity/size/checksum, SHA-pinned experimental artifact (only after CI fully passes).
- **Physical LXR-02 regression STILL REQUIRED**: let it idle until saver activates, look for a recognisable skull moving UP/DOWN; press encoder or pad mid-animation several times; watch screen for corruption; verify original menu content and controls, verify no new audio underruns while seq and dub are active, ensure DSP/PK/LATE/RAIL under 3–4 simultaneous pad hits remain acceptable. Return to v1.8.04 if display malfunctions.

## Separate wavetable-engine work: JB-27, not bundled here

The user's other repository/video is understood to be [Brendan Clarke's LXR02Open](https://github.com/brendanclarke/LXR02Open) on `prime`; its code adds oscillator waveform-ID modulation with optional linear interpolation across adjacent wave types and sample/loop IDs. **JB-24 provided an independently written wave-first UI and existing JIMBONES wavetable DSP; it did NOT fully port upstream oscillator code.**

The upstream `LICENSE` forbids commercial use and demands source redistribution. That restriction is **not compatible with redistributing a combined project as GPL-3.0-only**. User intends personal use, but a public GitHub branch distributing copied code must still satisfy all licences. **Do not copy third-party source into the public GPL-only project unchanged.** JB-27 should either obtain a compatible licence exception/permission or implement its functionality independently, with host tests and a measured DSP budget. The upstream block-morph can require rendering two oscillator buffers, so safeguard the 62% baseline and 4-hit headroom. Do not combine the FM engine, extra sample loader or new USB audio with this UI bugfix.

**Status rule:** Green CI means source/ARM tests passed, NOT physical screen/audio verification.
