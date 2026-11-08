# JB-09 wavetable — independent agent workstream

**Owners:** DSP + Research
**State:** Research proposal, not autonomous agent execution or tested implementation.

## Improvement investigation
Measure interpolation on/off CPU, crossfade band-limited tables, avoid new large ROM banks; test aliasing and velocity blend. Review LXR02Open license and avoid direct copying without compatibility audit.

## Deliverables
1. Locate concrete integration points and relevant upstream references, with license review.
2. Define a measurable before/after fixture and performance budget.
3. Implement minimal isolated code on this branch; do not alter release branch directly.
4. Request review and run host/ARM CI. Hardware verification required for release.

## Safety
Keep original six voices and the working v1.8.02 untouched. Do not merge unverified firmware.
