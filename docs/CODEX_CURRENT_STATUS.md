# JIMBONES — CURRENT STATUS

TASK: L02 project save recovery; L03 project BPM recall semantics
STATUS: PROJECT BPM CONTEXTS PASS; FULL PROJECT SAVE RECOVERY STILL BLOCKS ARM CANDIDATE
BRANCH: agents/jb-wavetable-host-experiment
CURRENT COMMIT: d00087716c95c8818e56d7f12babf7937134802d (pre-checkpoint)
LAST SOURCE COMMIT: d00087716c95c8818e56d7f12babf7937134802d
TESTED COMMIT: d00087716c95c8818e56d7f12babf7937134802d
CHANGED FILES: Core/Src/Preset/presetManager.c; tests/production/emu/CMakeLists.txt; tests/production/emu/bpm_persist.c; tests/production/emu/hw/emu_hw.h; tests/production/emu/hw/sd_emu.c; tests/production/emu/run_bpm_fault_matrix.sh; recovery/job-tree docs
LAST TEST COMMAND: ctest --test-dir /tmp/jb-host-emu --output-on-failure
TEST EXIT CODE: 0
ACTUAL RESULT: Before fix, injected second low-level write caused preset_saveGlobals to report success while 137 BPM became 120 after cold restart. New save writes glo.new, checks byte count, syncs and closes, retains glo.bak during replacement; load falls back to complete backup and never installs a short read. Native emulator CTest 5/5 PASS including 12 persistent write-fault, 4 sync-fault, zero-byte primary and genuine full-card cold restarts; every failed save reports FatFs error and restores a complete old (137 BPM/channel 2) or committed new (145 BPM/channel 5) configuration, project 0. Step FX/LED and Shift menu suites PASS. At exact source SHA, 12 OFF-path PCM cases versus pinned integration f797ec7 have zero mismatches; overlap host sum clips 3 samples equally in both builds. Project 0/1 tempo 137/145, kit load unchanged, unsaved 160 reverts, and saved external sync mode 0 restores runtime mode 1 in native emulator. ARM/image/hardware NOT RUN.
CURRENT BLOCKER: preset_saveProject deletes the existing project folder before writing replacements and ignores preset_saveGlobals result; interrupted project save can still lose kits/patterns. Mid-sector write and rename power-loss checks remain. Candidate is not ready for ARM packaging.
EXACT NEXT ACTION: Reproduce preset_saveProject failure on a saved project using host FatFs fault injection, then stage/roll back project data or hold candidate with exact blocker. Test performance BPM separately. Only after persistence gate, attempt local arm-none-eabi-gcc build.

£0 additional spending; no Codespace, merge or flash.
