# JIMBONES — CURRENT STATUS

TASK: P0 full-project and globals save integrity
STATUS: IN PROGRESS; same-length globals corruption fixed, corrupt-primary resave edge under investigation
BRANCH: agents/jb-wavetable-host-experiment
CURRENT COMMIT: 023b47a4c9708e240c915d3ff3090f0fc2c39607 (pre-checkpoint)
LAST SOURCE COMMIT: 023b47a4c9708e240c915d3ff3090f0fc2c39607
TESTED COMMIT: 023b47a4c9708e240c915d3ff3090f0fc2c39607
CHANGED FILES: Core/Src/Preset/presetManager.c; tests/production/emu/bpm_persist.c; tests/production/emu/run_bpm_fault_matrix.sh
LAST TEST COMMAND: bash tests/run_off_path_pcm_compare.sh
TEST EXIT CODE: 0 (PASS)
ACTUAL RESULT: New glo.cfg saves append CRC32 after unchanged legacy parameter bytes. New loader accepts legacy length, validates CRC on new files, and falls back to glo.bak on same-length corruption. Native emulator CTest 6/6 PASS, including single-byte and all-byte primary corruption, 57 injected full-project failed saves, SD-full/removal, rename boundaries, retries, synthetic legacy upgrade and fresh slot. Step FX/LED and Shift host scripts PASS. At this exact source SHA, 12 OFF-path PCM cases have zero sample mismatches versus pinned integration f797ec7. No ARM build/image/flash.
CURRENT BLOCKER: Audit found a possible data-loss edge: preset_saveGlobals may delete the only valid backup when an existing primary has failed CRC, then a later replacement save fails. This sequence is NOT YET TESTED. Torn FAT/power-loss and third-save backup cleanup faults also remain unverified. Candidate blocked.
EXACT NEXT ACTION: Add a cold-start fault reproducer for corrupt primary -> valid backup -> failed resave; preserve backup when primary is invalid, then rerun globals and project fault matrices. Do not ARM-build until save integrity is verified.

£0 additional spending; experimental branch only; no ARM image, merge or flash.
