# JB-23 — Mac / LXR-02 experimental image preparation and rollback

**Important:** The GitHub Actions artifact is named **UNVERIFIED** on purpose. It is a firmware candidate for a user-supervised hardware experiment, **not a stable release, not a boot guarantee**, and does not change the updater/bootloader. No remote process can install this on the actual LXR.

## Before you even copy an experimental image

1. **Keep your currently bootable firmware's original .img file**, copied to a different Mac folder or drive. Firmware installed on the machine cannot be recreated simply by copying the SD card. If you do not have a known-working rollback image, obtain or rebuild one first and **do not test** the experimental candidate yet.
2. Copy the entire existing SD card contents to a Mac folder, including kits and patterns. Do not overwrite the originals; keep another copy if the SD contains unique work. Eject cleanly.
3. Confirm the device is the **desktop Erica Synths LXR-02** and the old image boots. Do not use the Eurorack firmware. Verify the old image and label its version and checksum.
4. On the Mac, check the candidate image SHA256 against the included `SHA256SUMS.txt` / `MANIFEST.json`:
   ```sh
   shasum -a 256 LXR_V2_V1.8.04.img
   ```
   The 64-character output must exactly match the manifest. A mismatch is a hard stop.
5. Keep the candidate's source commit SHA, workflow run URL and checksum with the hardware test sheet. Check the GitHub draft PR chain (#8 → #9 → #10 → #11 → #12 → #13 → #14) has been independently reviewed before installation.

## Test card and update (only after all preflight checks pass)

1. Use a separate **FAT32** SD card where possible. Do not format the only card containing your kits. On Mac Disk Utility, use MS-DOS (FAT) and MBR for compatibility. The target is the normal bootloader SD update, **not** the debug-probe `flash_full` target.
2. Put the correctly checksummed `LXR_V2_V1.8.04.img` at the SD card root. **There must be only one `LXR*.img` updater on the card**; the LXR loader selects the first matching update file. Do not include previous images or debug/full-flash binary in the root.
3. With the unit OFF and output levels low, insert the card. Hold the main encoder button while powering up. Look for **LXR Updater**. Follow the updater screen; when it says **Update OK!**, it restarts.
4. If the screen reports checksum, size or other update errors, stop; re-check the card and the image. Do not keep power-cycling during a flash write. If in doubt, use the known-working recovery updater first.
5. Start with an unmodified native kit, six voices and low volume. Record idle DSP/PK/LATE/RAIL, then follow `jb23_hardware_test_sheet.md` before enabling wavetable or dub send.

## How to roll back if the experiment fails

1. Switch the LXR OFF. On the Mac, remove the experimental `LXR*.img` from the test SD root and copy your verified **known-working** image there, with **no second updater image** in the root.
2. Power up with the encoder held to enter the built-in updater, as above, and load the known-working image. The updater is separate from the application and is not modified by this image, but **recovery is not guaranteed** if something unexpected happens.
3. Restore saved kits/patterns from the SD backup only if needed; do not copy experimental/legacy kit formats over a healthy backup without checking.
4. If the bootloader cannot read the SD card, shows persistent flash errors or the hardware behaves abnormally, stop. Do not try a full-chip flash or bootloader replacement without appropriate hardware and expert recovery.

## Safety / scope reminders

- Source-level host tests and ARM compilation do not prove no sound glitches or timing misses on four simultaneous live pads.
- The prior DSP 101%, LATE 999 case belongs to **v1.8.03**, not v1.8.04. Measure again.
- First sound checks at a low level to protect speakers/headphones. Use the physical output route you know, never assume the global FX bus returns to a particular jack.
- A validated image file says its bytes are coherent and targeted correctly; it does not certify that it will run safely on the unit. Keep all previous images and preset backups until tests have passed.
