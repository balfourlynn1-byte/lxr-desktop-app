# JB-14 — Transient designer in CLICK submenu
**Branch:** agents/jb-14-transient-click
**Status:** implementation assigned; not yet coded or hardware tested. Roles are virtual, not autonomous processes.

## Implementer
Implement per-voice attack and sustain shaping, optional transient click/noise layer, velocity sensitivity. UI MUST live in existing LXR-02 CLICK submenu (not a new top-level page), retaining native click selection and original controls. Inspect real menu routing and free parameter/storage capacity before changing mapping. Disabled/bypass preserves bit-exact legacy sound; no kit corruption. Host impulse tests, four-voice CPU/rail measurements, ARM build and hardware validation.

## Independent reviewer
Inspect source changes and tests; check original six-voice sound, old kit loading, interrupt safety, four simultaneous modulated voices and long tails, cycle budget and ARM compilation. Reject changes with unverified memory layout or undocumented UI conflicts. No USB audio, no bootloader modifications, no changes to working v1.8.02.

## Delivery
Actual source commits and host regression, exact CI run, ARM image SHA, hardware test checklist, and explicit merge recommendation. Research-only documents do not count as implementation.
