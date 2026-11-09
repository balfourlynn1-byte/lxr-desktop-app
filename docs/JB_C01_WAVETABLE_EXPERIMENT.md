# C01 — isolated wavetable experiment branch (2026-11-09)

Status: DONE for branch isolation only. This is **not** an implemented or tested wavetable feature.

- Base integration source SHA: `bb7d5bd8dd5cd3dea57fc776241770f19c83c0ab` on `agents/jb-28-full-integration-v186`.
- Isolated branch: `agents/jb-wavetable-host-experiment`.
- Rollback: discard this experimental branch; original integration branch remains unchanged at its independently advancing head. Do not cherry-pick unverified oscillator changes.
- Native oscillator source blob at branch creation: `Core/Src/Soundengine/DSPAudio/Oscillator.c` `009226ea23cc07bd0706a42bd56d7ed6a3fe93ac`; header blob `9b1be9f39646e636523b530fe04c70f38f3a852b`.
- First implementation/test task C02: build a deterministic host-only legacy oscillator bypass fixture against native oscillator behavior, then check bit-identical output when optional interpolation is OFF. After C02, implement bounded periodic waveform scan; preserve native IDs 0–5 (PWM remains 5), six original voices, existing FM, 132-sample No Clip and approved skull. Do not import Brendan's non-commercial source.
- No local GCC/Clang tests executed as part of C01; no ARM compiler, .img packaging, main merge or hardware readiness claim. ARM approval hold remains.
