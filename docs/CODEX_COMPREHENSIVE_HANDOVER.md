# JIMBONES — comprehensive Codex engineering handover

**Prepared 2026-11-08.** Repo: [balfourlynn1-byte/lxr-desktop-app](https://github.com/balfourlynn1-byte/lxr-desktop-app). This is a **reference manual**, not the first document to read every session. **Start with [CODEX_START_HERE.md](CODEX_START_HERE.md)** and current [checkpoint](CODEX_CHECKPOINT_JB28A.md), then consult this manual only for relevant decisions. See [verified source register](CODEX_SOURCE_REGISTER.md) for repo/branch/PR links, SHA pins, licences and code map.

## Mission and non-negotiables
Build a musically useful, reliable custom Erica Synths LXR-02 firmware for stick-played live performance with Ableton and external drum triggers. Preserve **all six original sound engines**, factory oscillator/FM capabilities, existing presets and dry audio. Do not overclock. Keep kick unaffected by performance effects unless user explicitly opts in. The user prioritises **real audible results** and reliable four-pad overlapping hits over novelty or CI-only milestones.

The firmware is constrained by a tight STM32 ITCM budget and actual real-time DSP load. User has previously experienced severe crunch with Metallic FM enabled. Experimental Metallic FM is shelved; **factory FM is NOT removed**. Never alter original kit fields/IDs or the native oscillator without compatibility tests and an explicit rollback plan.

## User's actual performance rig
- LXR-02 played with Alesis Performance Pad (DIN MIDI, sticks), Roland KT-10 kick, Roland FD-8 hi-hat pedal; Ableton Live routes pads and hi-hat open/closed. Mac Intel used for test workflow. Usually 3–4 simultaneous hits with overlapping long tom decays.
- Output routing goal for future USB work: 1 kick; 2 toms 1/2; 3 snare+clap; 4 hi-hat, with stereo effects; **feasibility only**, not a delivered feature.
- Live performance requires predictable parameter changes, no clipping/crunch, stable Step FX, no audible dry signal changes when effects are OFF.

## Device evidence (separate from build claims)
| Item | Device evidence | Status |
|---|---|---|
| v1.8.04 boot | User booted successfully | PARTIAL HARDWARE PASS |
| Dub effect/delay | User hears working echo | AUDIBLY WORKING |
| Step FX core | User confirms existing effects work; pitch up/down not useful and no per-voice targeting | WORKING CORE, UI REDESIGN REQUIRED |
| DSP after Metallic FM removal | User reported ~62% DSP and no previous crunch | USER OBSERVATION, NOT FULL STRESS CERTIFICATION |
| Old Metallic FM branch | DSP 94–101%, PK 105%, LATE 999, RAIL 240 and audible crunch | REJECTED/SHELVED |
| Step 1 LED/toggle | User previously observed bug; newer code fix not yet verified on device | OPEN UNTIL TESTED |
| Five dub subdivisions in v1.8.05 | Code/CI only | HARDWARE UNVERIFIED |
| Skull/LCD repair v1.8.06 | Code/CI only | HARDWARE UNVERIFIED |
| Comb #15 | Donor code and CI passed, separate branch | NOT IN CURRENT COMBINED IMAGE |
| JB-27 native scan | Experimental branch exists; native periodic interpolation only | NOT COMBINED / NOT HARDWARE ACCEPTED |
| JB-24 auxiliary wavetable | User says it does not match demo intent | INCOMPLETE AS REQUESTED |
| Morph, tape, advanced filters, hi-hat, USB audio | Feature ideas/research and possibly other experimental branches | DO NOT CLAIM COMPLETE |

## Architecture and risks
- `Core/CMakeLists.txt` defines separate object libraries for platform, IO, UI, presets, DSP, engine, MIDI, sequencer and USB MIDI. The current build uses `Core/Src/Soundengine/DSPAudio/` and `Core/Src/Soundengine/` for audio.
- Comb donor PR #15 uses six independent 256-sample fixed-point delay rings, bounded feedback 220/256, zero-Reso exact bypass, appends filter ID 8 after old IDs 0–7. Existing Freq/Reso/Drive controls; no extra global FX bus or heap in ISR.
- JB-26 skull code targets **16×2 character OLED**, not a graphical LCD. It uses CGRAM slots 4–7 and nonblocking byte-latched updates. UI work must not block audio.
- JB-25 dub timing pads 1–5 represent 1/4, 1/8, triplet eighth, 1/16 and 1/32; legacy Step FX on 6–16 in that branch. One active effect at a time. Confirm intended interactions with the future JB-29 16-slot parameter scheme rather than silently reassigning controls.
- Historical JB-26 ITCM 560 B free; JB-12 donor 528 B free **on another branch**. Combined free memory is unknown. Hard gate >=512 B. Move control-rate code to flash when safe, not DSP kernels; no weakening gates.
- Do not assume per-voice UI names equal six DSP voice indexes. The user's intuitive grouping (snare/clap, two toms, hi-hat, kick) must be reconciled with actual factory LXR voice map, including cymbal and open/closed hi-hat behaviour. Resolve from source before writing masks.

## Dependency graph and execution plan

```text
Current JB-26 lineage (#17) ───────────────┐
                                          ├── JB-28A: transplant Comb (#15), isolated PR, CI and v1.8.07 test image
Comb donor (#15) ─────────────────────────┘
                                              │
                                              └── JB-28B: physical regression / rollback
                                                       │
                                                       ├── JB-29A: Step FX voice masks, groups, slot recall
                                                       │       └── JB-29B: RATE/DEPTH/CHAR, useful pitch range
                                                       │
                                                       └── JB-27 native wave scan: review separate existing branch
                                                               └── new isolated integration only after DSP headroom
```

**JB-28A** is the only active coding job in the current packet. Its [single-page contract](CODEX_START_HERE.md) is authoritative. Preserve PR #17 changes, selectively transplant PR #15, run focused Comb tests first, full GCC/Clang and ARM later, package only when integrity and >=512 B ITCM gates pass. One draft PR; no merge/flash. After green, user hardware tests with [acceptance sheet](JIMBONES_HARDWARE_ACCEPTANCE.md).

**JB-29A** approved controls: four knobs fixed roles VOICE, RATE, DEPTH, CHAR. Knob 1 cycles OFF, SNR, TOM, HAT, S+T, S+H, T+H, ALL. SNR=snare+clap; TOM=two toms; HAT=hi-hat; ALL=all five non-kick voices. SHIFT+Knob1 advanced individual six-bit voice mask; all 16 slots recall independent voice selection and parameters. The grouping is a UI proposal over **actual mapped voice IDs**, not proof of six independent labels. User explicitly wants snare alone and clap alone; grouped-only UI is insufficient. No kick by default. **Implement routing/masks first, parameter redesign second.** Verify pots vs encoders using `adcPots.c` and `encoder.c` before pickup logic. Existing Step FX and dub must remain audible and unaffected when not selected.

**JB-27** exists as an experimental native oscillator waveform scan on `agents/jb-27-native-wave-scan-v187`; independently authored code uses original oscillator periodic wave tables, LFO destination and per-sample interpolation. It is **not** the upstream sample/loop engine, not accepted on hardware, and must not be accidentally duplicated or mixed into JB-28A. Review its [technical design](https://github.com/balfourlynn1-byte/lxr-desktop-app/blob/agents/jb-27-native-wave-scan-v187/docs/agents/jb27_native_wave_scan.md) and branch diff only when that separate job begins.

## Remaining feature backlog — distinct jobs
- Morph A/B/C: A protected; independently edit B/C; per-voice exclusions including kick; LFO and velocity-to-morph; safe kit data and LED indication.
- Tape/tube drive per voice: preserve low-end, saturate highs; velocity drive; not a global reverb/delay replacement.
- Wavetable: full native waveform-ID scanning and smooth morph, sample transient click selector, optional custom samples only with compatible storage/licence. Existing JB-24 and JB-27 are partial and different.
- Performance Step FX: tempo-synced retrigger/random/2–4–8-bar patterns; reliable button toggle and LED; no rewrite of proven effects.
- Hi-hat: improved choking, open/closed FD-8 control, velocity; independent snare zones if Sensory Percussion becomes available.
- Per-hit random selection and probability on snare; avoid backbeat disruption.
- Independent bass HPF/LPF and comb: Comb has donor implementation; independent bass filter is separate.
- Four-channel USB audio: investigate MCU USB peripheral, DMA, bandwidth, RAM, class compliance, host/codec constraints before promising any firmware. Preserve USB MIDI. No Overbridge promise.
- CPU monitor and four-pad stress instrumentation remain priority. Report DSP/PK/LATE/RAIL and when RAIL increments (sequencer on/off).

## Third-party research and licence
[Source register](CODEX_SOURCE_REGISTER.md) verifies LXR02Open, openelixir-02, digitone-2-modding-skill and JIMBONES GitHub repos. **Do not import** LXR02Open's noncommercial source into the public GPL-only repo. Reproduce concepts independently if lawful and technically feasible, or request explicit licence permission. Octabam/modwerk owner paths not confirmed: research only during USB job. Don't pretend all external sources have been audited.

## Testing ladder
1. Static source mapping and diff review. Confirm intended paths and compatibility.
2. Focused host unit test on changed DSP/UI logic; sanitizers where supported.
3. Full host GCC and Clang regression suite.
4. ARM cross-link, ITCM >=512 B free, RAM/static size, updater vector/format/checksum and SHA256.
5. Pinned candidate with `HARDWARE_UNVERIFIED`, exact source SHA, PR and known-good rollback. **No automatic flashing**.
6. Real hardware: six original voices and kits, dry parity, kick protection, Step FX and dub, target feature, display, four-pad stress with DSP/PK/LATE/RAIL. Record user acceptance or reject.

## Persistent progress and allocation survival
Codex must read [persistence protocol](CODEX_PERSISTENCE_PROTOCOL.md) and **push** checkpoints at start, after research, after each code milestone, before costly builds, after tests/failures, and on stopping. Write [JB-28A checkpoint](CODEX_CHECKPOINT_JB28A.md) and [working queue](JIMBONES_WORK_QUEUE.md) on the same work branch. A local-only file is not durable. If allocation ends abruptly, resume from last remote SHA. Avoid unnecessary full-repo scans. Two unsuccessful attempts at same failure = stop with exact next action.

## What Codex should return
Use the fixed response template in [CODEX_START_HERE.md]: job ID; BLOCKED or CODE_READY_HARDWARE_UNVERIFIED; branch/commit; donor SHA; changed paths; focused/full host/ARM results; ITCM; image filename/size/SHA256 if validated; draft PR; hardware NOT TESTED; next action. Never label a new sound working based only on CI.

## Review questions for Codex before any additional feature
1. Does the requested feature already exist on another branch? Search [source register](CODEX_SOURCE_REGISTER.md) first.
2. Can it reuse a tested DSP path instead of allocating new per-sample work?
3. What is the exact six-voice mapping and any shared hi-hat/cymbal routing?
4. What is the effect of a disabled feature on original PCM and kit bytes?
5. What tests catch a UI-only illusion of implementation?
6. Can the user actually hear and control it with four knobs and 16 buttons?
7. What is the source, licence, memory/CPU budget, and rollback?
8. Has the latest research checkpoint been pushed before further expensive work?
