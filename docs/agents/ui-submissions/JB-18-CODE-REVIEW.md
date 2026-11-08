# JB-18 — MIX / LOFI submenu implementation handoff

STATUS: **ASSIGNED / NOT IMPLEMENTED OR REVIEWED IN THIS BRANCH**. This branch starts from the JB-13 UI router; any existing implementation there is a baseline, not a completed deliverable for this task.

## Implementation request
- Native section: **MIX / LOFI**. Follow `docs/agents/JIMBONES_UI_CODE_IMPLEMENTATION_ASSIGNMENTS.md` and `docs/JIMBONES_UI_NAVIGATION_MASTER.md`.
- Implement physical button navigation, 16x2 LCD, 4 encoder/knob controls, per-voice display, safe parameter bounds, OFF/bypass, kit state or explicit TEMP.
- Only connect sound controls to tested DSP; never activate an incomplete processor. Do not overwrite native menu parameters.
- Supply tests: original menu parity, all six voices, two or more modulated voices, four simultaneous long tails, final rail/clipping/deadline counters, intentional-FX distinction, old kit restore.
- Deliver actual code changes, deterministic host tests, ARM build proof and draft PR for JB-13/JB-20 review.

**P0 NO CLIP:** any unintentional crunch, clipping, pop or dropout blocks release. Do not merge until sound-quality review and physical hardware validation.
