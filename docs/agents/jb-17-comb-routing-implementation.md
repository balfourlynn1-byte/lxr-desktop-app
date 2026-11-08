# JB-17 — Comb polarity and filter routing
**Branch:** agents/jb-17-comb-routing
**Status:** implementation assigned; not yet coded or hardware tested. Roles are virtual, not autonomous processes.

## Implementer
Implement stable Comb+ and Comb- variants alongside Octatrack-style comb filter, and safe pre/post routing; bounded feedback, no DC/clip spikes. Host feedback and memory tests.

## Independent reviewer
Inspect source changes and tests; check original six-voice sound, old kit loading, interrupt safety, four simultaneous modulated voices and long tails, cycle budget and ARM compilation. Reject changes with unverified memory layout or undocumented UI conflicts. No USB audio, no bootloader modifications, no changes to working v1.8.02.

## Delivery
Actual source commits and host regression, exact CI run, ARM image SHA, hardware test checklist, and explicit merge recommendation. Research-only documents do not count as implementation.
