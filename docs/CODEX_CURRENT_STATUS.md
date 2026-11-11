# JIMBONES — CURRENT STATUS

TASK: P0 project and globals save data integrity
STATUS: IN PROGRESS; host regression gates PASS, ARM candidate BLOCKED
BRANCH: agents/jb-wavetable-host-experiment
CURRENT COMMIT: fe8c69eef3cf039b58e5e66070ce9e6100ef9517 (pre-checkpoint)
LAST SOURCE COMMIT: fe8c69eef3cf039b58e5e66070ce9e6100ef9517
TESTED COMMIT: fe8c69eef3cf039b58e5e66070ce9e6100ef9517
CHANGED FILES: Core/Src/Preset/presetManager.c; tests/production/emu/bpm_persist.c; tests/production/emu/run_bpm_fault_matrix.sh
LAST TEST COMMAND: bash tests/run_off_path_pcm_compare.sh
TEST EXIT CODE: 0 (PASS)
ACTUAL RESULT: Six native emulator CTests PASS. Full-project gate has 57 injected failed saves (46 write, four sync, four rename-boundary, full SD, removed SD, repeated late failure), all cold-starting a complete old or new generation with matching project header, kit/pattern/song identities and data, BPM and selection. Globals gate has 24 fault/recovery scenarios: 12 write, four sync, zero-byte, single-byte same-length and all-byte same-length corruption, four corrupt-primary interrupted resaves, full SD. All PASS; after corrupt-primary resave fault, old 137/channel 2 or complete new 160/channel 6 recovers. Synthetic original-length globals and unmarked project load and upgrade; fresh project slot 2 saves/recalls 151 BPM. Step FX/LED and Shift host scripts PASS. Exact-source OFF-path PCM 12/12 PASS with zero mismatches against f797ec786fc57d008afeaabad964906f35a43177; overlap host-sum has three clipped samples in both builds. No ARM build/image/flash.
CURRENT BLOCKER: Native fault injection does not model torn FAT sectors or physical power loss during directory entry updates. Third-save backup cleanup failures have not been selected/tested explicitly. The unmarked target plus valid backup recovery policy conservatively selects backup and could roll back a legacy generation; needs review. New globals CRC catches corruption in checksummed files; a legacy file with no checksum cannot be authenticated. Candidate remains blocked.
EXACT NEXT ACTION: Add measured third-save backup-cleanup write faults with cold-start generation checks and successful retry; then review/resolve ambiguous unmarked target plus backup, perform physical power-loss validation with rollback plan, and only then reassess ARM build eligibility.

£0 additional spending; experimental branch only; no ARM image, merge or flash.
