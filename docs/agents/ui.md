# JB-13 UI diagnostics

**State:** Research proposal, no autonomous agent executing.

## Improvement investigation
Reuse menu pages and display buffer limits, display peak CPU and overrun counts without audio-thread formatting, test double-press debounce and menu indices; defer waveform visuals.

## Required work
Identify upstream references and license constraints; define baseline and test fixtures; implement on isolated branch; verify host tests, ARM build and image; require hardware testing before release.

Do not overwrite known-working v1.8.02.
