# JB-06 USB multitrack — independent agent workstream

**Owners:** Research + Embedded
**State:** Research proposal, not autonomous agent execution or tested implementation.

## Improvement investigation
Audit STM32F765 USB peripheral speed, available pins, DMA, descriptors, current USB MIDI routing and buffer RAM. Quantify 4x 16/24-bit 48kHz throughput, class-compliant host support and channel routing. Feasibility gate before any USB firmware.

## Deliverables
1. Locate concrete integration points and relevant upstream references, with license review.
2. Define a measurable before/after fixture and performance budget.
3. Implement minimal isolated code on this branch; do not alter release branch directly.
4. Request review and run host/ARM CI. Hardware verification required for release.

## Safety
Keep original six voices and the working v1.8.02 untouched. Do not merge unverified firmware.
