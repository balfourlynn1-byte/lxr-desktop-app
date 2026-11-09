# JIMBONES live Codex checkpoint

Updated 2026-11-09 (UTC). Branch: `agents/jb-wavetable-host-experiment`.

## Confirmed work
- C09 wavetable ON C-kernel PCM test passed GCC/Clang: Actions run `42699351706`.
- C08 and repaired C06 guards passed: Actions run `42699202075`.
- C10 immutable four-wave ROM bank, opt-in `jb_drumScanConfigureBuiltin` API and GCC/Clang test committed.

## Pending / failure evidence
- C10 test run `42699661787` failed in both compilers at C10 test; likely test compared waveform outputs at phase 0 where waveforms may share sample value. Assertion changed to quarter-cycle phase in commit `5d0e19f2fc7fdca3fceb4b174cb548c63b8fc5c8`; **rerun not yet verified**.
- No complete native DrumVoice ON-path PCM render, source registration, DSP performance evidence, UI/kit persistence or hardware build yet.

## Resume
1. Read `docs/CODEX_CHECKPOINT_AND_RECOVERY.md` and follow mandatory commit+push protocol.
2. Verify C10 workflow result after fix; inspect failed job logs if red.
3. Add/verify ROM source registration and a real native render harness. Check OFF identity and ON audible changes.
4. Update checkpoint with exact latest pushed SHA and next task after each atomic change.

**Strict ARM HOLD:** No ARM compilation, firmware image, merge or hardware-ready claim without explicit user approval of a frozen candidate.

### 2026-11-09 — C11 desktop build registration
- Added `Src/Soundengine/DSPAudio/JimbonesNativeWaveBank.c` to the actual desktop `Core/CMakeLists.txt` app source list, commit `04e5089a2f9b8496228be2decc814384672b5356`. Added source guard `tests/test_c11_native_build_registration.py` and wired into GCC/Clang CI commit `3ee234c3c0d4e8a331ed14bceabad0733d1bbdca`.
- **NOT full native compile yet:** C11 guard checks CMake source registration and API text; the complete DrumVoice/Oscillator compilation, linking, PCM and CPU remain unverified. Continue to prioritize actual native host harness, wavetable ON rendering, OFF identity. Strict ARM hold. Mandatory Codex checkpoint and push protocol remains in force (`docs/CODEX_CHECKPOINT_AND_RECOVERY.md`).

### 2026-11-09 21:24 UTC — C12 default-OFF source guard (one atomic task)
- Integration branch `agents/jb-28-full-integration-v186` observed HEAD `f797ec786fc57d008afeaabad964906f35a43177`; isolated branch before this task `a0a027a4517c09dc2ce9aced99349dd90eaeaf89`. No merge.
- C11 Actions run `42700501364` **PASS** at commit `3ee234c3c0d4` (GCC/Clang source guard; **not** complete DrumVoice host compile). C10 rerun `42699996137` **PASS**.
- C12 adds `tests/test_c12_wave_off_source.py` commit `a72afd221a7ecc7855d3916c5fef2e1377734f6c`, and CI step commit `9c26d45cbe3e3917c93adba4319504d4d9ed3c8c`. Command: `python3 tests/test_c12_wave_off_source.py`; checks static default OFF, explicit enable/disable, and legacy `calcNextOscSampleBlock` fallback. **NOT RUN / CI pending at time of checkpoint.** This is only a structural source guard, **not bit-identical PCM evidence**.
- Next unblocked: inspect C12 CI and fix if red; then complete native DrumVoice host compile and OFF-path PCM identity test. No ARM build, .img, main merge or hardware-ready claim. Mandatory checkpoint-and-push instructions apply; each GitHub contents write creates an immediately remote-backed commit.

**C12 CI verified PASS:** GitHub Actions run `42702335053`, commit `9c26d45cbe3e3917c93adba4319504d4d9ed3c8c`, both compiler matrix jobs completed successfully. Source guard only; native DrumVoice compile and OFF PCM bit identity remain NOT RUN. https://github.com/balfourlynn1-byte/lxr-desktop-app/actions/runs/42702335053

### 2026-11-09 — Approved UI roadmap (design only; implement AFTER native wavetable ON audio is validated)
User approved the proposed UI direction. **Do not mistake approval of this roadmap for approval to compile ARM firmware or ship a hardware image.** Maintain strict ARM hold and mandatory commit/push checkpoint policy in `docs/CODEX_CHECKPOINT_AND_RECOVERY.md`.

**Priority order and acceptance gates:**
1. **WT Macro (P1 after native ON audio):** OSC double-press opens a clearly labelled extra page with four controls: WAVE, SCAN, VEL, LFO. Preserve original OSC screen, navigation and all six original voice sounds; ensure no accidental parameter changes when switching pages.
2. **Per-voice wavetable toggle (P1):** explicit OFF/ON for each voice, with six-voice overview and quick kick exclusion. Never silently enable wavetable; ON uses durable built-in waveform data and OFF preserves exact factory path.
3. **Kit save/recall status (P1):** modified-kit indicator, save confirmation, error/recovery warnings; save/reload wavetable per-voice settings and kit BPM safely without breaking factory kit ABI or cold-start recovery. No false 'saved' indicator on SD failure.
4. **Performance FX (P2):** steps 1–16 independent toggle behavior; Step 1 all-FX bypass and Step 16 morph LFO; correct sequencer Step 1 LED when armed. Define unambiguous mode separation so FX mapping cannot interfere with sequencer editing.
5. **Diagnostics (P2):** DSP %, peak-hold, LATE and RAIL counters on an accessible page; keep CPU impact negligible and show resets clearly.
6. **WT detail and modulation pages (P2):** OSC additional presses reveal BANK/POS/BLEND/MODE and velocity/LFO rate/depth controls, subject to hardware display/encoder constraints and real engine support.
7. **Waveform mini-display (P3):** lightweight static/slowly refreshed waveform and scan-position marker; no full oscilloscope unless proven CPU-safe. Drop animation if DSP margin suffers.
8. **Hold-to-audition A/B (P2):** temporary original oscillator audition while holding an assigned control; release returns to configured wavetable without modifying saved state. Choose a non-conflicting physical control after hardware button-map review.

**Codex implementation requirements:** first inspect actual LXR-02 menu/button/encoder architecture and document feasible page mappings; prototype on host/emulator, validate UI state transitions and audio OFF/ON switching, check DSP headroom and persistence. Each atomic UI change must be committed and **pushed** with test evidence and live checkpoint. Do not implement UI ahead of native ON-path PCM acceptance.

### 2026-11-09 — Approved addition: PERFORMANCE / FX ALL page
- Add a second page within the Performance section labelled **FX ALL** (proposed UI; not yet implemented), for controlling effects across all eligible voices together.
- Provide an explicit **ALL FX ON/OFF** master bypass and quick access to global effect amounts/assignments; keep per-voice exclusions, especially optional kick protection, visible and respected.
- Distinguish the FX ALL page from the Step FX 1–16 toggle page and normal sequencer mode; preserve Step 1 sequencer LED and existing Step 1 bypass / Step 16 morph LFO proposals without ambiguous button mappings.
- Design safeguard: master bypass should not erase per-effect toggle states, so re-enabling restores prior configuration. No kit writes until explicit save, and no unintended effect on original sound when disabled.
- First inspect actual performance-menu hardware architecture and existing global vs per-voice effect routing. Specify what 'all' covers before implementation; no new DSP-heavy global effect chain by default.
- **Priority:** P2, after native wavetable-ON audio and P1 controls. Prototype and test on host/emulator, measure CPU, commit/push each atomic step and update live checkpoint. Strict ARM hold remains.

### 2026-11-09 — Approved design direction: universal hold-to-modify performance controls
User wants Torso Electronics S-4-style press-and-hold behavior and Digitakt TRACK-like held modifier **across all settings, not only FX**. This supersedes treating momentary FX ALL as an isolated feature.

**Design specification (proposal, not implemented):**
- Add a consistent **momentary / latch** interaction model across supported parameters and pages. A held physical modifier temporarily applies an alternate state; release restores the underlying state. Optional latch mode stays engaged until toggled again.
- Apply to FX ALL and individual FX bypass/engage, morph target, wavetable A/B original-engine audition and scan position, filters, randomization, and selected/all-voice parameter edits. Respect voice exclusions, especially kick.
- Support held modifier + encoder/parameter movement for selected/all-voice adjustment, **but do not commit persistent parameter changes unless explicitly confirmed**. Differentiate temporary audition from intentional editing.
- Capture original values at press-down; release restores deterministically even if the user changes page/voice while holding. Define conflict resolution for concurrent modifiers, retriggers, kit changes, transport, and rapid press/release; no stuck state after page exit or kit load.
- Keep ordinary short press and existing sequencer/Step FX buttons functional; avoid conflicts with existing hardware combinations. No specific button assignment until actual LXR-02 control mapping is inspected. Clear held/latch status indicator; avoid CPU-heavy display work.
- Acceptance: host/emulator UI state-machine tests for press/hold/release, latch toggle, simultaneous hits, page switching, exclusions, save/reload, and audio bypass identity; verify no unintended kit writes and no new DSP spikes.
- **Priority P2** after full native wavetable-ON PCM validation and P1 core controls. Atomically commit **and push** every coherent step; update `docs/CODEX_LIVE_CHECKPOINT.md` per mandatory timeout protocol. Strict ARM hold unchanged.

### 2026-11-09 — User-defined SHIFT + RELATIVE EDIT (six-voice proportional offset)
**Confirmed requirement:** Holding SHIFT while adjusting a compatible parameter applies the **same signed delta** to that parameter on every included voice, **not the same absolute value**. Example cutoff [20,45,60,35,75,90] +10 -> [30,55,70,45,85,100] (0–100 range). Preserve each voice's independent offsets/timbre; clamp each voice at its own valid bounds. This is a **persistent in-memory edit**, NOT momentary; release SHIFT does not revert. Kit file changes only after normal explicit save.

- Candidate parameters: pitch, decay, filter cutoff/resonance, drive, wavetable scan and morph, **only where each target voice actually supports equivalent semantics**. Avoid applying unrelated values or incompatible parameter types.
- Every physical encoder tick produces one delta, with no accumulation from redraws or repeat frames. Use correct native resolution/scaling per parameter (not naive raw byte increments for differently scaled controls). Preserve voice exclusion masks including optional kick protection. Clearly indicate REL EDIT / included voices on display.
- Handle boundaries, negative movement, voice changes while held, undo/cancel policy, kit load, parameter automation and MIDI overrides; verify no surprise jumps or kit writes. Do not conflate this SHIFT edit behavior with separate hold-to-audition momentary effects.
- Before implementation inspect existing SHIFT button and encoder mappings; do not break normal SHIFT shortcuts or Step FX/sequencer controls. Test distinct initial values, positive/negative delta, clamps, exclusions, and saved/reloaded state in host/emulator.
- **Priority P2** after native wavetable ON audio gate and core UI. Atomic commit/push and checkpoint after each task; strict ARM hold unchanged.

### 2026-11-09 — SHIFT relative-edit implementation research R00
- Completed first source trace and saved `docs/JB_SHIFT_RELATIVE_EDIT_CODE_RESEARCH.md` commit `85e738138b55be82b45753d4834ea1c92c0805bb`. Confirmed `buttonHandler_getShift()` in `Core/Src/buttonHandler.c`, encoder edit entry `menu_encoderChangeParameter(int8_t inc)` in `Core/Src/menu/menu.c`, shared `activeParameters` and MIDI_CC/CC_2 dispatch, and existing SHIFT+Performance shortcut. **Do not assume parameter IDs have a per-voice stride**. Next R01 enumerate semantic per-voice parameter mappings; R02 trace per-voice dispatch and persistence; R03 pure signed saturating delta tests; R04 gated UI; R05 host acceptance. Research only, no runtime changes, ARM hold. Follow mandatory continuous commit/push and live checkpoint protocol.

### 2026-11-09 — SHIFT relative edit R02b code trace
- Research updated in `docs/JB_SHIFT_RELATIVE_EDIT_CODE_RESEARCH.md` commit `7370f34e88b530bf505a160e8cc8a3b0cebee264`. Discovered `Core/Src/Soundengine/MIDI/ParameterArray.c` maps individual per-voice parameter IDs to engine fields via `parameterArray_init()`; voice types use different structures, so semantic ID groups required. `paramArray_setParameter()` active implementation forwards to `midiParser_ccHandler`, not a guaranteed safe direct write. Next enumerate compatible six-voice ID groups and automation/preset behavior; no runtime code change. Continuous save/push protocol and ARM hold unchanged.
