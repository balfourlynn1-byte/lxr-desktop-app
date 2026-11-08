# JB-13 Native UI implementation map — agent-owned submenu buttons
**Status:** Native CLICK+ and MIX+ UI implemented on feature branch; all other pages are assigned but **not yet implemented**. Do not merge until CI and hardware review.

## Source audit
The actual voice submenu indices in `Core/Inc/menu/menuPages.h` are **0 OSC, 1 AMP EG, 2 MOD, 3 FM, 4 TRANSIENT (CLICK), 5 FILTER, 6 LFO, 7 MIX**. These are the real button/page identifiers, not imagined FX or PERF buttons. `Core/Src/menu/menu.c` owns `menu_switchSubPage`, `menu_repaintGeneric`, knob and encoder handlers. The existing OSC extended page uses a special second-press cycle. Performance has existing Step FX and CPU pages. A shared submenu registry must respect the existing eight native page slots.

## Ownership: one button / submenu per feature agent
| Agent | Physical section / native subpage | Assigned extension | Status |
|---|---|---|---|
| JB-01 | GLOBAL/SYSTEM | Build/rollback read-only | Assignment only |
| JB-02 | PERFORMANCE existing CPU page | CPU load, peak, misses | Existing diagnostic UI; review pending |
| JB-03 | PERFORMANCE CPU details | Final mixer peaks/clips | Assignment only |
| JB-04 | MOD / 2 | MORPH A/B/C | Assignment only |
| JB-05 | MIX / 7 | TAPE per-voice | Assignment only |
| JB-06 | None | USB audio deferred | Excluded |
| JB-07 | PERFORMANCE existing Step FX | STEP FX and SEND | Existing UI, extension assigned |
| JB-08 | OSC / 0 | METAL | Existing extended OSC parameters |
| JB-09 | OSC / 0 | WAVE and SHAPE | Existing extended OSC parameters |
| JB-10 | MOD / 2 | HAT (voice-specific) | Assignment only |
| JB-11 | MOD / 2 | SNARE VAR | Assignment only |
| JB-12 | FILTER / 5 | BASS and COMB | Assignment only |
| JB-13 | All native buttons | Router, collisions, consistent 16x2 screen and four knobs | CLICK+ / MIX+ first implementation |
| JB-14 | TRANSIENT/CLICK / 4 | CLICK+ transient ATTACK and SUSTAIN | **Wired to JB-14 runtime DSP**; UI TEMP, no kit persistence |
| JB-15 | FM / 3 | FM DRUM BODY/NOISE | Assignment only |
| JB-16 | MOD / 2 | VEL ROUTE | Assignment only |
| JB-17 | FILTER / 5 | COMB +/- and routing | Assignment only |
| JB-18 | MIX / 7 | MIX+ LOFI SRR | **Wired to JB-18 runtime DSP**; UI TEMP, no kit persistence |
| JB-19 | OSC / 0 | CLUSTER | Assignment only |
| JB-20 | PERFORMANCE/GLOBAL | Build verification and release status | Assignment only |

## Implemented gesture in this branch
On a voice page, pressing native CLICK (subpage 4) selects its original parameters; pressing CLICK again enters **CLICK+ TRANS TEMP**; pressing again exits to original CLICK. Knob 1 adjusts attack (-64..64), knob 2 sustain (-64..64); per-voice DSP API `jb_setTransient` called. Native MIX (subpage 7) similarly opens **MIX+ LOFI TEMP**; knob 1 controls SRR factor (0..32), with 0/1 bypass. Original parameter pages are preserved. Voice switching clears the extension. Existing OSC and Performance pages remain unchanged.

## Required next integration gates
- Host menu fixture and ARM CI for exact branch; verify no conflicts with `menu_switchPage`, encoder handling and save/load.
- Verify runtime-only UI cache resets on kit load and cannot show stale settings after `jb_loadKit`; otherwise explicitly block release.
- Add versioned kit storage, safe bypass, other parameters, full menu router for all assigned pages, no hidden overwrites of native controls.
- Validate real 16x2 display, four knobs, page buttons, MIDI and all six voices on physical device. **Not hardware tested.**

## Implementation update — native menu router
Commit `01ed3a4a8cfb6124b5095dc38374e56e3a060123` adds code-level native submenu cycling for MOD (JB-04/10/11/16), FM (JB-15), CLICK (JB-14), FILTER (JB-12/17) and MIX (JB-05/18), preserving the existing OSC experimental page and existing Performance screens. Only CLICK attack/sustain and MIX SRR are DSP-wired; every other page explicitly displays `PENDING` and does not modify audio. MIX requires cycling through TAPE first to reach LOFI. This is deliberately a non-releasable draft until host+ARM CI, physical UI tests, and NO CLIP P0 qualification pass. No claim of complete feature menus, kit persistence or verified hardware.

## Next milestone progress — kit-reload consistency
JB-13 branch now has DSP `jb_kitRevision()` incrementing at initialization and kit reload; UI clears temporary transient/SRR knob cache when the revision changes. Host DSP regression checks revision changes and legacy OFF kit load. These commits are **not yet CI-verified**; P0 No Clip remains UNVERIFIED and the draft PR is not a flash-ready release. Next gates: run exact-commit host/ARM CI, independently review menu transitions and verify physical button workflow; do not merge before those gates.
