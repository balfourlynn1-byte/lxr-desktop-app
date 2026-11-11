# JIMBONES — CURRENT STATUS

TASK: P0 full-project save integrity and cold-start recovery
STATUS: IN PROGRESS; host fault matrix and regressions PASS, physical power-loss safety unverified
BRANCH: agents/jb-wavetable-host-experiment
CURRENT COMMIT: e7d329edd7560133f0d0aafbcec5fc8ee37f60ff (pre-checkpoint)
LAST SOURCE COMMIT: e7d329edd7560133f0d0aafbcec5fc8ee37f60ff
TESTED COMMIT: e7d329edd7560133f0d0aafbcec5fc8ee37f60ff
CHANGED FILES: Core/Src/Preset/presetManager.c; tests/production/emu/bpm_persist.c; tests/production/emu/run_project_save_repro.sh
LAST TEST COMMAND: bash tests/run_off_path_pcm_compare.sh
TEST EXIT CODE: 0 (PASS)
ACTUAL RESULT: Host emulator CTest 6/6 PASS, including 40 early write faults, mid/late replacement faults selected from 2051 measured writes, four sync faults, three clean replacements, globals write/sync faults, full-card and zero-byte globals recovery. Cold restart verified matching old/new project header, kit/pattern/song names, pattern and song data, BPM and project selection. Step FX/LED and Shift host scripts PASS. Twelve OFF-path PCM cases at this exact source SHA match pinned integration with zero sample mismatches; overlap host-sum clipping remains the previously documented three samples in both builds. Separate tx.ok manifest covers 194 required project files; legacy file formats are unchanged but legacy fixture load is NOT RUN. No ARM build/image/flash.
CURRENT BLOCKER: Host matrix does not yet cover every rename/power-loss state, project SD-full, short writes at FatFs level, repeated failure/retry, or real SD media. Independent globals updates have their existing separate recovery protocol; project manifest checks globals size/readability but not its contents. Candidate remains blocked.
EXACT NEXT ACTION: Extend native emulator with stage/rename and SD-full fault injection plus full-generation cold-start assertions; test repeated failure then successful resave. Audit legacy and project-switching paths, rerun full host gates, then consider ARM build only if integrity evidence is sufficient.

£0 additional spending; experimental branch only; no merge or flash.
