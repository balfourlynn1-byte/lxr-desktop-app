# H02 — LXR02Open sample loading and memory audit (2026-11-08)

**Status:** Upstream source audit DONE; JIMBONES integration and fault-injection tests **NOT RUN / NOT IMPLEMENTED**. Reference commit `brendanclarke/LXR02Open@c896bfd0efe86a4037418bf9f46f1fcc39255958`. No upstream code copied; no ARM build.

## Pinned upstream sources
- `Core/SampleRom/SampleMemory.c` blob `77f5ce8cb44943f72d896715eb690c7b897fcffa`
- `Core/SampleRom/SampleMemory.h` blob `055ad30d96e85832967e3948014ee1f2de76f322`
- `Core/SampleRom/sampleFlash.c` blob `d1fab6f71f08443c636a6d4b549eb347743cf5c3`
- `Core/Hardware/SD/filesystem.c` blob `48219b1b2bdd50b9060ae49a61474de8abd5c5fb`
- `Core/DSPAudio/Oscillator.c` blob `5c1bf056b8016ff1a43a87f7f72ae6bf59bb4467`

## Confirmed architecture
1. **Flash layout** (`SampleMemory.h:70-96`): STM32F765 single-bank internal flash sectors 6–11, six 256-KiB sectors = 1.5 MiB *reserved*, with sample metadata and display names at top of sector 11. Actual PCM capacity is less than 1.5 MiB because of tables/headers/padding. `SAMPLE_MAX_COUNT=120`; metadata uses 12-byte `SampleInfo` records, 8-byte display names, a high-bit loop flag and 16-bit PCM sample count.
2. **Manifest cache** (`SampleMemory.c:76-125`): `sampleMemory_refresh()` increments a nonzero generation, validates flash metadata and fills in-RAM info/name/loop arrays. `sampleMemory_getSampleInfo()` returns a silence sentinel for out-of-range IDs; invalid manifest entries terminate scanning. Generation changes invalidate oscillator cache.
3. **Audio hot-path cache** (`Oscillator.c:116-134`): `osc_updateSampleCache()` retains pointer, size, loop flag and sample index in oscillator state until either index or manifest generation changes. This avoids repeated table lookups on unchanged sample IDs, but not the actual PCM reads from flash.
4. **Install sequence** (`SampleMemory.c:165-325`): fresh install clears caches and erases sectors 6–11; append mode calculates last occupied address and retains existing manifest. Each sample checks count/capacity, stores loop flag and appends a zero pad word; commit writes metadata/names then refreshes cache. Failure before commit can leave partially modified flash; **do not assume transactional recovery**.
5. **Flash access** (`sampleFlash.c:168-210`): sector erases are restricted to 6–11 and use status polling/timeouts. Same-bank flash erase/program can stall instruction/sample reads, explicitly warned in `SampleMemory.h:39-41`. Sample installation must not be treated as real-time audio-safe.
6. **SD async facade** (`Core/Hardware/SD/filesystem.c:63,2539-2627,2698-2702`): SD transfers are serviced through asyncfatfs polling in one context; installer orchestrates sample writes and manifest commit. Async SD I/O **does not** make synchronous internal flash erase/program nonblocking.
7. **Format**: upstream README describes up to 120 44.1-kHz/16-bit files from `samples/` and `loops/` directories; these are LXR02Open conventions, not verified compatibility with JIMBONES/Erica kits.

## Engineering decision for JIMBONES
**Do not port the entire sample loader or flash layout.** JIMBONES should preserve existing Erica preset/SD behaviour and original six voices. Consider only independently implemented metadata generation invalidation and oscillator-local cached sample descriptors if measurements show a benefit. Keep new wavetable tables in known-safe memory; reserve flash/ITCM/RAM with linker evidence after ARM approval. Never erase sectors based solely on upstream assumptions.

## Tests required before adopting any idea
- Host: 0/1/120/121 sample count; invalid/truncated/corrupt manifest; pointer bounds; 16-bit length overflow; loop vs one-shot; zero-length sample; stale cache after generation rollover and sample replacement.
- Host: install capacity exactly at limit, +1 byte overflow, alignment and zero padding, append after partial write, interrupted commit, bad SD read, failed flash erase, power-loss simulation, full card and previous-kit rollback.
- Audio: sample ID switch during 3–4 overlapping long-tail hits, cache invalidation, click/peak analysis, CPU benchmark cached vs uncached; ensure default OFF path bit-identical.
- Integration: verify no flash erase/write in audio ISR, no non-atomic state publication, no memory-region collision, no regression in No Clip/comb/Step FX/legacy preset recovery.
- Hardware-only: measure flash stalls and real SD disconnect recovery on approved image; record DSP/PK/LATE/RAIL, not yet testable under ARM hold.

**Risk register:** Single-bank flash stalls; partial commit/recovery; invalid pointer if manifest changes during audio render; stale cache on generation wrap; potential wrong sample format assumptions; 1.5-MiB reserve is not available RAM. Next H03 MIDI morph source map; H02 follow-on tests remain TODO in G07/H06.
