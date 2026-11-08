# JB-15 — FM Drum body/noise synthesis
**Branch:** agents/jb-15-fm-drum
**Status:** implementation assigned; not yet coded or hardware tested. Roles are virtual, not autonomous processes.

## Implementer
Implement optional lightweight body/noise/pitch-sweep architecture; reuse existing metallic FM where practical. No duplicate engine when existing controls suffice. Host sound regression and ARM budget.

## Independent reviewer
Inspect source changes and tests; check original six-voice sound, old kit loading, interrupt safety, four simultaneous modulated voices and long tails, cycle budget and ARM compilation. Reject changes with unverified memory layout or undocumented UI conflicts. No USB audio, no bootloader modifications, no changes to working v1.8.02.

## Delivery
Actual source commits and host regression, exact CI run, ARM image SHA, hardware test checklist, and explicit merge recommendation. Research-only documents do not count as implementation.
