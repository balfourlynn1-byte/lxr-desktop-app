# JB-12 filters

**State:** Research proposal, no autonomous agent executing.

## Improvement investigation
Prototype a stable bass HPF/LPF and bounded comb delay; calculate coefficients outside sample loop, measure worst-case resonance, memory and CPU; protect existing six-voice signal path.

## Required work
Identify upstream references and license constraints; define baseline and test fixtures; implement on isolated branch; verify host tests, ARM build and image; require hardware testing before release.

Do not overwrite known-working v1.8.02.
