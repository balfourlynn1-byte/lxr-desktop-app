# A04 — approved v1.8.06 bouncing-skull source preservation map

**Status: source-trace DONE; host execution NOT RUN in this audit.** Reference branch `agents/jb-26-skull-screensaver-v185` is the approved skull lineage; this does not establish exact flashed v1.8.06 image identity (A05).

## Exact source references (Git blob IDs)
- `Core/Src/menu/JimbonesSkull.c`: `0f6ee956bc93576cf164525cc173285cb451b268`; `Core/Inc/menu/JimbonesSkull.h`: `494a586faaf18888d7ac6cf63a3ee5d00a77bd41`. Original 10x12 bitmap in four 5x8 CGRAM glyphs; 8-frame vertical offsets 0,1,2,3,4,3,2,1, no audio-path work.
- `Core/Src/menu/screensaver.c`: `38395dc3468e090b9988e162b37cc8ba04e13444`. `JB_SKULL_FRAME_MS=170` at line 19; `screensaver_touch` lines 48–55; `SKULL_DRAIN_MENU`/clear/glyph/write/hold/exit state machine lines 73–145; `screensaver_check` lines 148–159. State drains in-flight LCD work via `sendDisplayBuffer` before CGRAM writes; wake is nonblocking, clears LCD and calls `menu_repaintAll` once.
- `Core/Src/menu/menu.c`: `6282d73b761860faa25192a5535793f5bca402e7`; stable `refresh_latched_char` at lines 829, 897–898, 939–943; menu refresh suppresses writes while saver active at line 989; `screensaver_touch` invoked by input paths near lines 2919 and 4141–4181.

## Existing executable host tests (source existence verified, not executed here)
- `tests/run_jb26_skull.sh` blob `db86265dd8d7eefcd31db8b70bdb0461223c8ea1` runs C glyph/bounce test, Python LCD ownership source assertions, and C mocked screensaver state test.
- `tests/test_jb26_skull.c` blob `72ba863c5bcdcfda896e04f944a9124318b26660`: checks bounded glyphs, 8-frame bounce symmetry and looping.
- `tests/test_jb26_screensaver_source.py` blob `866edca07745cb3e11ad49a638440c831d99f8ee`: asserts nibble latching, menu drain, nonblocking exit/repaint, CGRAM codes 4–7.
- `tests/test_jb26_screensaver_state.c` blob `201b4f9d9eb3af3722a4302a69182df1927c8d0d`: mocks LCD to check menu drain, glyph writes, 200ms animation tick, touch wake, repaint and reactivation.
- `tests/run_jb26_screensaver_state.sh` blob `3fcfcb21fa232637a0eb4f142d259e15c6d6fbc4`. Integration `tests/run_host.sh` invokes `sh tests/run_jb26_skull.sh`.

## Exact host commands / gate
```sh
sh tests/run_jb26_skull.sh
CC=clang sh tests/run_jb26_skull.sh
```
Run on a complete checkout and record stdout, compiler version, exit status and exact SHA. These tests have **not** been executed in this audit, so they cannot be marked PASS. Before integration, compare the above blob IDs against the destination and rerun both compilers. Hardware LCD timing/wake remains unverified after any new integration; no ARM build without user approval.
