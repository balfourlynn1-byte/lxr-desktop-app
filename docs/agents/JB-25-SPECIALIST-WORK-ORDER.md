# JB-25: Morph A/B/C and voice protection

**Assignment status: created, not autonomously executing.**

## Implementation scope
Implement A immutable/protected, edit B/C without affecting A, per-voice inclusion/exclusion including kick, LFO/velocity morph, interpolation and click-free transitions; versioned kit storage and native MOD UI.

## Contract
- Work from `agents/jb13-native-submenu-ui` and deliver real source code, regression tests, measured results, and draft PR against that integration branch.
- Do not directly merge, overwrite known-working firmware, or publish a hardware-ready image without review.
- Coordinate: JB-21↔JB-03/JB-23; JB-22↔JB-02/JB-26; JB-23↔JB-20; JB-24↔JB-13/JB-20; JB-25↔JB-04/JB-13; JB-26↔JB-08/JB-09/JB-19.
- **P0 release criterion:** any unintentional clipping, crunching, crackle, pops or dropouts is FAIL. Intentional effects only when explicitly enabled; native bypass clean; test 2–4 simultaneous voices with overlapping long decays.
- Include exact test commands, failure reproduction, baseline vs patched measurements, CPU/peak impact and review checklist. Mark missing hardware tests UNVERIFIED.

## Review gates
1. Self-review and deterministic host test evidence.
2. Cross-review by another named workstream, with actual code comments/PR approval (not a simulated agent signoff).
3. JB-24 verifies ARM compilation and memory constraints; JB-20 decides promotion. Physical LXR-02 listening test remains mandatory for NO CLIP PASS.
