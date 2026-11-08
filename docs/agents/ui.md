# JB-13 UI diagnostics

**State:** Research proposal, no autonomous agent executing.

## Improvement investigation
Reuse menu pages and display buffer limits, display peak CPU and overrun counts without audio-thread formatting, test double-press debounce and menu indices; defer waveform visuals.

## Required work
Identify upstream references and license constraints; define baseline and test fixtures; implement on isolated branch; verify host tests, ARM build and image; require hardware testing before release.

Do not overwrite known-working v1.8.02.

## Solution research handoff — 2026-11-08
**JB-13 design:** Reuse existing menu hooks. Show render-load current/peak and missed-deadline count using nonblocking snapshots. Label as render load, not total CPU; debounce double-press and protect menu indices. No audio-thread formatting.
**Acceptance:** menu simulation, host/ARM CI, hardware legibility, no audio deadline regressions. Proposed only; not implemented.
**References:** https://github.com/erica-synths/lxr-desktop-app and https://github.com/brendanclarke/LXR02Open (license review before copying). USB audio excluded.
