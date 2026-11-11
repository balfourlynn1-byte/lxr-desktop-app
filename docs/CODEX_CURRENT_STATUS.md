# JIMBONES — CURRENT STATUS

TASK: P0 full-project save transaction and cold-start recovery
STATUS: LATE PROMOTION FAILURE REPRODUCED; CANDIDATE BLOCKED
BRANCH: agents/jb-wavetable-host-experiment
CURRENT COMMIT: 0ae1b6b0e9c9b9e7b21b557616255cd3fc9e82f7 (pre-checkpoint)
LAST SOURCE COMMIT: 0ae1b6b0e9c9b9e7b21b557616255cd3fc9e82f7
TESTED COMMIT: 0ae1b6b0e9c9b9e7b21b557616255cd3fc9e82f7 plus late-fault test change
CHANGED FILES: tests/production/emu/run_project_save_repro.sh; docs/CODEX_CURRENT_STATUS.md; docs/CODEX_LIVE_CHECKPOINT.md
LAST TEST COMMAND: ctest --test-dir /tmp/jb-host-emu -R project_save_faults --output-on-failure
TEST EXIT CODE: 8 (FAIL)
ACTUAL RESULT: After fetching concurrent branch updates, six native CTests initially failed at repeated clean save (FatFs 4), then passed after fixing project cleanup to use a 256-byte bounded work path; source fix pushed at 0ae1b6b. Host write count shows 2052 low-level writes in a clean second project save. Added late fault 2052 to regression: save returns FatFs 1, but cold restart finds /proj00/prj.nfo missing (FatFs 4), project load fails and BPM becomes 120. Other tested write faults 1-40, 1000, 2000, 2040, 2048 and 2051 preserve old project. Late promotion recovery remains broken. No ARM build/image/flash.
CURRENT BLOCKER: Recovery treats a directory entry for an incomplete promoted project as valid merely because /proj00 exists; it does not validate project contents or prefer the valid backup. Full-generation integrity and power-loss safety are not established.
EXACT NEXT ACTION: Add a separate project completeness marker/manifest after all staged files are verified, validate target and backup on boot, recover the valid generation after fault 2052, then rerun expanded fault matrix and exact-source PCM. Keep ARM blocked until all save integrity gates pass.

£0 additional spending; experimental branch only; no merge or flash.
