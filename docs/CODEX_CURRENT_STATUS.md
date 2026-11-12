# JIMBONES — CURRENT STATUS

TASK: P0 project/globals save integrity and experimental ARM hardware-test build
STATUS: HOST AND ARM BUILD PASS; controlled hardware test candidate, hardware untested
BRANCH: agents/jb-wavetable-host-experiment
CURRENT COMMIT: 6c6fbf7dbb4d18dbb0053e42c6f99b8c45c13fe3 (before this documentation checkpoint)
LAST SOURCE COMMIT: e93185028632b8044c214dc8500b1f62203239e2
TESTED COMMIT: e93185028632b8044c214dc8500b1f62203239e2
CHANGED FILES: tests/production/emu/bpm_persist.c; tests/production/emu/run_project_save_repro.sh; artifacts/jimbones-e931850/LXR_V2_V1.8.07.img; artifacts/jimbones-e931850/BUILD_MANIFEST.md
LAST TEST COMMAND: git show origin/agents/jb-wavetable-host-experiment:artifacts/jimbones-e931850/LXR_V2_V1.8.07.img | shasum -a 256
TEST EXIT CODE: 0
ACTUAL RESULT: At e931850: native CTest 6/6 PASS; project matrix 62 injected failed saves with complete old/new cold-start generations and successful retries; globals matrix 24 fault/recovery cases plus four successful retries PASS; Step FX/LED and Shift host scripts PASS; OFF-path PCM 12/12 zero mismatches against f797ec786fc57d008afeaabad964906f35a43177. Overlap host-sum has three clipped samples in both builds. ARM GCC 16.1.0 clean build exit 0, ELF/BIN/IMG generated; img payload/header/checksum verified. Pinned integration also built with same compiler. Experimental image size 306672 B, SHA-256 8698c941e14d8192011399a88172f6f28cbd962772d88fd6f486ae1bbc3dfbab. Remote artifact fetched and hash matches. See artifacts/jimbones-e931850/BUILD_MANIFEST.md.
CURRENT BLOCKER: Physical SD power loss, FAT sector tearing, target CPU load and on-device audio remain untested. ITCM is 95.36% full. Legacy unchecksummed globals cannot be authenticated. No hardware flash has occurred.
EXACT NEXT ACTION: Back up a sacrificial SD card and a known-working rollback image; run controlled on-device project save/restart and power-interruption tests on the experimental image, recording the recovered generation and any FatFs errors. Do not use for live performance until hardware behavior is verified.

£0 additional spending; experimental branch only; no production merge or automatic flash.
