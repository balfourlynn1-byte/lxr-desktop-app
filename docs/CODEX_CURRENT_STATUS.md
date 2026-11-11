# JIMBONES — CURRENT STATUS

TASK: P0 full-project save data integrity
STATUS: IN PROGRESS; native fault matrix PASS, ARM candidate BLOCKED
BRANCH: agents/jb-wavetable-host-experiment
CURRENT COMMIT: 7fb89cf2877ed4bcdc7583b0dcd6965e7ad990a6 (pre-checkpoint)
LAST SOURCE COMMIT: 05f36a077753b85c2a8f43282375e8c13f2ac692
TESTED COMMIT: 7fb89cf2877ed4bcdc7583b0dcd6965e7ad990a6 (native); 602c1d62fcbad67f30bb31e8cbbf40fe47a7d030 (PCM, same production source)
CHANGED FILES: Core/Src/Preset/presetManager.c; tests/production/emu/bpm_persist.c; tests/production/emu/run_project_save_repro.sh; tests/production/emu/hw/emu_hw.h; tests/production/emu/hw/sd_emu.c
LAST TEST COMMAND: sh tests/run_stepfx_ui.sh; sh tests/run_shift_relative_menu_host.sh
TEST EXIT CODE: 0; 0 (PASS)
ACTUAL RESULT: Native emulator CTest 6/6 PASS at 7fb89cf. Project-save gate includes 57 injected failed saves: 46 write ordinals, four sync ordinals, four before/after rename points, full SD, removed SD, and a repeated late failure. Every tested restart recovered a complete old or new project generation; successful retries after full-card and late-write failures cold-booted the new generation. Checks cover prj.nfo, all 194 required files through tx.ok, distinguishable kit/pattern/song names and data, BPM, and project selection. Synthetic unmarked legacy project loads and upgrades to marked format; fresh project slot 2 saves and recalls 151 BPM. Step FX/LED and Shift host scripts PASS at latest HEAD. OFF-path PCM at 602c1d6 (same production source as current HEAD) PASS 12/12 with zero mismatches against f797ec7; overlap host-sum has three clipped samples in both builds. No ARM build, image, merge or flash.
CURRENT BLOCKER: Physical power-loss and torn FAT directory updates remain unverified; the host rename hook simulates failures before/after FatFs calls, not torn sectors. Project tx.ok fingerprints kit/pattern/song contents but excludes globals contents because glo.cfg can be saved independently. Same-length corrupt globals are not detected by this manifest. A synthetic unmarked target with a valid backup is conservatively rolled back; that ambiguous legacy state needs policy review. ARM candidate remains blocked.
EXACT NEXT ACTION: Design and test checksummed globals recovery compatible with independent globals saves and legacy projects; inject same-length corruption, verify rollback and project-generation consistency. Then exercise third-save backup cleanup faults, rerun all host gates, and only then reassess ARM build eligibility.

£0 additional spending; experimental branch only; no ARM image, merge or flash.
