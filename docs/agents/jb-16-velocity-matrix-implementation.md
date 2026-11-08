# JB-16 — Velocity modulation routes
**Branch:** agents/jb-16-velocity-matrix
**Status:** implementation assigned; not yet coded or hardware tested. Roles are virtual, not autonomous processes.

## Implementer
Implement bounded per-voice velocity modulation to transient, FM index, morph, filter and tape drive; protected A and optional kick exclusion. Deterministic host tests.

## Independent reviewer
Inspect source changes and tests; check original six-voice sound, old kit loading, interrupt safety, four simultaneous modulated voices and long tails, cycle budget and ARM compilation. Reject changes with unverified memory layout or undocumented UI conflicts. No USB audio, no bootloader modifications, no changes to working v1.8.02.

## Delivery
Actual source commits and host regression, exact CI run, ARM image SHA, hardware test checklist, and explicit merge recommendation. Research-only documents do not count as implementation.
