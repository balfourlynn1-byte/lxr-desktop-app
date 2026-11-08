# JIMBONES — all-agent UI/submenu design request
2026-11-08 | Status: **assigned work orders; recommendations below are initial proposals, not independent agent submissions or implemented menus.**

## Shared UI contract — mandatory for every feature agent
1. **Keep the native LXR-02 workflow.** Reuse existing sound/voice, OSC, MOD, FILTER, CLICK, FX and performance/step pages where present. Do not add an unrelated top-level interface. Verify actual source menu identifiers and available physical controls before implementation; labels here are proposals.
2. **One consistent navigation rule:** first press = existing page, second press of same section key = JIMBONES extended page; further presses cycle extensions only if necessary. Back/exit returns to native page. Define precise timeout/debounce and no conflict with existing double-press actions. Do not steal an existing gesture without migration.
3. **Always show voice, page, parameter and value.** Keep knob/encoder assignments consistent with native conventions. No surprise changes to another voice. Default BYPASS/OFF; preserve native sound and old-kit load/save.
4. **Kit storage:** propose versioned backward-compatible serialization and explicit default values. Runtime-only controls must be visibly marked TEMP until persistence exists.
5. **Performance controls:** never require deep menus mid-performance; preserve immediate pads/steps, sequencer visibility and LED meanings. Active Step FX LED must flash, inactive LED unlit; step 1 is assignable. No invisible global OFF.
6. **Safety:** bounded ranges, no runaway comb feedback, no unsafe drive/level jumps, CPU load and deadline-miss diagnostics visible without ISR formatting. Kick exclusion must be explicit where applicable.
7. **Every agent submission must include:** (a) native parent submenu + exact entry gesture, (b) 4-or-fewer parameters per screen with labels/ranges/defaults, (c) encoder/button and LED feedback, (d) kit persistence + legacy fallback, (e) bypass and failure handling, (f) CPU/RAM/ISR cost, (g) test cases, (h) dependencies/conflicts and alternative if native mapping differs.
8. **Review gate:** JB-13 UI owner consolidates navigation and detects conflicts; JB-20 checks source-accurate menu hooks, six-voice parity, hardware test steps. Recommendations are not approval to merge.

## Individual feature-agent briefs and *initial* UI recommendations
| Agent | Parent submenu / proposed entry | Proposed compact control pages | Must report/test |
|---|---|---|---|
| JB-01 Baseline | SYSTEM > JIMBONES > ABOUT (only if a system extension exists) | Version, build SHA, legacy/bypass status, rollback reminder; read-only | Do not replace native settings or add bootloader features |
| JB-02 CPU monitor | PERF second press > CPU | LOAD, PEAK, MISSES, RESET (hold to confirm); render-only vs real deadline separately labelled | Avoid expensive ISR rendering, prove real timing |
| JB-03 Crunch diagnostics | PERF > CPU > AUDIO diagnostic subpage | PRE PEAK, POST PEAK, CLIP COUNT, LAST MISS; read-only | Counters must be from actual signal path; no false fix claims |
| JB-04 Morph A/B/C | MOD second press > MORPH | TARGET A/B/C, POSITION, LFO DEPTH, VOICE MASK; A protected; B edit indicator flashes | No edits to A from B/C, explicit kick opt-out, kit migration |
| JB-05 Tape/tube | FX second press > VOICE DRIVE | TYPE, DRIVE, MIX, VEL DEPTH; per-voice, not master | Level-matched bypass, bass preservation, CPU |
| JB-06 USB multitrack | **Deferred; no candidate UI** | Research-only external proposal, no menu hooks | Must not be included in this firmware |
| JB-07 Step FX / dub sends | PERF > STEP FX or existing step-FX page | Step assignment/type, 2/4/8 BAR, SEND, FEEDBACK; 16 steps independently toggle and flash when active | No step-1 OFF sentinel, sync and kick exclusion, no feedback runaway |
| JB-08 Metallic FM | OSC second press > METAL | ENGINE, LEVEL, RATIO, DEPTH; optional layer | Native oscillator intact, CPU headroom |
| JB-09 Wavetable/waveshaping | OSC second press > WAVE then SHAPE | TABLE, BLEND, FOLD/SHAPE, INTERP; second extension only if needed | No unlicensed sample/table imports, alias and memory checks |
| JB-10 FD-8 hi-hat | MOD second press > HAT (voice-specific) | PEDAL CC, OPEN/CLOSED, CHOKE, CURVE | Preserve native hat MIDI, no cross-voice choking |
| JB-11 Snare variation | MOD second press > VAR (snare voice) | PROBABILITY, SEED, BACKBEAT LOCK, MIX | Per-trigger only, deterministic recall, no mid-tail switching |
| JB-12 Bass HPF/LPF + comb | FILTER second press > BASS then COMB | HPF, LPF, COMB TUNE, RES/FEEDBACK | Safe resonance bounds, original filter unchanged |
| JB-13 UI coordination | Existing page hierarchy | Produce complete navigation tree, key/LED map, shared labels and collision matrix | Native source audit before approval |
| JB-14 Transient Designer | **Existing CLICK submenu**, extended CLICK page | ATTACK, SUSTAIN, CLICK/NOISE MIX, VEL DEPTH; original click selection retained | Preserve original CLICK functions, default off, no menu collision |
| JB-15 FM Drum body/noise | OSC second press > FM DRUM | BODY, NOISE, PITCH SWEEP, MIX | Coexist with JB-08; avoid duplicate oscillator controls |
| JB-16 Velocity matrix | MOD second press > VEL ROUTE | DESTINATION, DEPTH, CURVE, VOICE INCLUDE | No kick routing unless enabled; bounded modulation |
| JB-17 Comb polarity/routing | FILTER > COMB extended | POLARITY +/−, ROUTE PRE/POST, MIX, LIMIT | JB-12 owns core comb page; no duplicated conflicting controls |
| JB-18 Bit/SRR | FX second press > LOFI | BITS, SRR FACTOR, MIX, VEL DEPTH | Existing bit control retained, SRR bypass, no switching clicks |
| JB-19 Oscillator cluster | OSC > CLUSTER extended | COUNT, DETUNE, SPREAD, LEVEL | CPU cap with four-voice pad stress |
| JB-20 Integration QA | PERF > BUILD STATUS if feasible; otherwise no extra UI | Exact SHA/version, TEST vs STABLE indicator, optional diagnostic reset | Cross-feature menu collisions, old kits, SD rollback and real hardware acceptance |

## Submission format — one file per agent
Create `docs/agents/ui-submissions/JB-XX-UI.md` with **STATUS: PROPOSAL**, source paths + actual menu symbol names, navigation state diagram, parameter map, kit bytes/versioning, bypass behavior, CPU impact, host/UI tests, device steps, conflicts, and reviewer sign-off field. JB-13 then prepares `docs/JIMBONES_UI_NAVIGATION_MASTER.md`; JB-20 may only approve after source audit and hardware evidence. Do not mark proposals as completed implementations.

## Priority for current hardware candidate
P0: JB-13 actual native UI mapping, JB-14 CLICK, JB-02 CPU and JB-03 crunch display. P1: JB-07 Step FX LEDs, JB-04 Morph and JB-18 LOFI. P2: all other extension pages. No user-visible menu claim until code + CI + hardware verification.
