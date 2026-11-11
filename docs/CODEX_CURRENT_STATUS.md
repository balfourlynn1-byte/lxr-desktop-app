# JIMBONES — CURRENT STATUS

TASK: L03 BPM cold-start persistence; P2 OFF-path audio identity
STATUS: PROJECT GLOBALS PATH FIX PUSHED; KIT-SPECIFIC TEMPO STILL OPEN
BRANCH: agents/jb-wavetable-host-experiment
CURRENT COMMIT: 27cd885cdd7f383dfe26a5416ceb5295f34dadcc (pre-checkpoint)
LAST SOURCE COMMIT: 27cd885cdd7f383dfe26a5416ceb5295f34dadcc
TESTED COMMIT: 27cd885cdd7f383dfe26a5416ceb5295f34dadcc
CHANGED FILES: Core/Src/Preset/presetManager.c; tests/production/emu/CMakeLists.txt; tests/production/emu/bpm_persist.c; tests/production/emu/run_bpm_persist.sh; recovery/job-tree docs
LAST TEST COMMAND: bash tests/run_off_path_pcm_compare.sh
TEST EXIT CODE: 0
ACTUAL RESULT: Native emulator reproduced 137 BPM before restart becoming 120 after restart, including after an explicit globals save. Root cause: live working directory is /tmp, while project globals load from /proj00. Absolute current-project glo.cfg path fixes explicit save/restart; CTest 3/3 PASS (selftest, full Shift, 137 BPM cold start). Step FX/LED and Shift menu host suites PASS. At source SHA 27cd885, 12 OFF-path PCM cases versus pinned f797ec7 each produced 4096 samples with zero mismatches and max absolute difference 0; overlap host sum clipped 3 samples in both builds. ARM, image and physical hardware NOT RUN.
CURRENT BLOCKER: Kit-associated BPM policy and missing/corrupt/SD-full recovery remain unverified. Live BPM edits without explicit globals/project save still revert; no autosave policy change made.
EXACT NEXT ACTION: Test project/global save failure paths and decide compatible kit tempo recall semantics before implementation; keep current 137 BPM explicit-save regression and rerun host tests after any change.

£0 additional spending; no Codespace, ARM build, image, merge or flash.
