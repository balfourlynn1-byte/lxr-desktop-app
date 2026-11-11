# JIMBONES — CURRENT STATUS

TASK: L02 full-project interrupted-save recovery; L03 project BPM
STATUS: CRITICAL PROJECT SAVE DATA-LOSS REPRODUCED; ARM CANDIDATE BLOCKED
BRANCH: agents/jb-wavetable-host-experiment
CURRENT COMMIT: cf3a527f653c7cd5ac32ea1c20ff6049b6c9e68b (pre-reproducer checkpoint)
LAST SOURCE COMMIT: d00087716c95c8818e56d7f12babf7937134802d
TESTED COMMIT: cf3a527f653c7cd5ac32ea1c20ff6049b6c9e68b plus uncommitted diagnostic test
CHANGED FILES: tests/production/emu/bpm_persist.c; tests/production/emu/run_project_save_repro.sh; recovery/job-tree docs
LAST TEST COMMAND: BPM_PERSIST_EXE=/tmp/jb-host-emu/bpm_persist sh tests/production/emu/run_project_save_repro.sh /tmp/jb-project-save-repro.sdchunks
TEST EXIT CODE: 4 (expected defect reproduction; NOT PASS)
ACTUAL RESULT: After saving project 0 at 137 BPM, injected low-level write fault 2 during resave returns FatFs error 1, but cold restart finds /proj00/prj.nfo missing (FatFs 4) and preset_loadProject returns 4. Write faults 2-10 all lost prj.nfo; faults 8-10 also reverted BPM to 120. Full-project save currently empties the existing folder before writing replacements and ignores preset_saveGlobals error. Previous project-globals staged save passed 16 fault points, zero-byte fallback and genuine full-card recovery; CTest 5/5 and 12-case OFF-path PCM at d000877 pass. Project BPM 137/145, kit load stability, unsaved edit rollback and external sync PASS in native emulator. ARM/image/hardware NOT RUN.
CURRENT BLOCKER: Full-project save can destroy the last valid project on interrupted write. This is a release-blocking data-integrity defect; no ARM candidate should be promoted until transactional project replacement and cold-start recovery pass.
EXACT NEXT ACTION: Implement staging of complete project folder and backup/rollback around final directory rename; add injected fault matrix and cold-start assertions for prj.nfo, kits, patterns, globals and project index. Re-run existing tests before ARM build.

£0 additional spending; no Codespace, merge or flash.
