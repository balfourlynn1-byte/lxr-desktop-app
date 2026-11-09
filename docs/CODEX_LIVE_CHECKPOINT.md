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
