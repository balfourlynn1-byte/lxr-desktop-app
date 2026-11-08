# JIMBONES K01 — historical emulator candidate audit (partial)

Date: 2026-11-08. Status: PARTIAL, NOT RUN. Host-only; absolute ARM hold remains.

## Verified repository facts
- `agents/jb-03-dsp-stress-v184`, `agents/jb-23-hardware-test-pack-v184`, and `agents/jb-28-full-integration-v186` share `Core/Src/Soundengine/DSPAudio/Jimbones.c` blob `149807fd1574f6f130f6272949920908c4f5e536`.
- These branches share `Core/Inc/Soundengine/DSPAudio/Jimbones.h` blob `a1910ddce801eac70b3ecfdf5a20f41b3ff14cda`, which explicitly defines `JB_METALLIC_ENABLED 0` (JB-22). Their DSP file contains `#if JB_METALLIC_ENABLED` sections, so the expensive legacy path is compiled out in these branches.
- Earlier branches `agents/jb-15-fm-drum`, `agents/jb-19-osc-cluster`, and `agents/jb-20-integration-qa` have header blob `f1722b359d58e351256e543b87a2255acb460230` without this disable flag. They are investigation candidates, NOT confirmed matches for a particular flashed .img.
- Upstream emulator `bg1eym/lxr-desktop-app` branch `emu` documents native CLI WAV rendering and ctest, but its original firmware v1.7 source cannot directly run an STM32 firmware .img or a different Sonic Potions firmware image.

## Execution attempt / blocker
- Local runtime has `git` and `clang`, but no repository checkout. A direct `git ls-remote https://github.com/bg1eym/lxr-desktop-app.git refs/heads/emu` failed with `Could not resolve host: github.com`.
- GitHub connector can read individual files but does not supply a runnable checkout to the local compiler. Therefore no native emulator build, ctest, WAV render, DSP stress measurement or A/B comparison was executed. Do not mark any of these PASS.

## Next exact actions
1. Identify pre-JB22 commit where Metallic is actually enabled and pin source/image identity; inspect early FM branches' renderer and header.
2. Obtain runnable complete source trees in an execution environment with GitHub checkout or a supplied source archive; do not infer performance from code alone.
3. Port pinned candidate into emulator adapter and run alternating modulated Tom 1/Tom 2, 3–4 simultaneous hits, long tails, sequencer on/off, output WAV/checksums.
4. Compare against pinned Metallic-OFF candidate and publish measured metrics, preserving ARM hold.
