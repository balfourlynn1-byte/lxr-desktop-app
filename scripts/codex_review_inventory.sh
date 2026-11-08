#!/bin/sh
# JIMBONES Codex review: local, read-only inventory. Run from checked-out repo.
set -eu
cd "$(dirname "$0")/.."
printf 'JIMBONES source inventory (read only)\n'
printf 'Branch: '; git branch --show-current
printf 'HEAD: '; git rev-parse HEAD
printf '\nWorking tree status:\n'; git status --short
printf '\nRecent commits:\n'; git log -5 --format='%h %s'
printf '\nKey references (local or remote):\n'
for ref in agents/jb-26-skull-screensaver-v185 agents/jb-12-comb-filter-v184 agents/jb-27-native-wave-scan-v187 agents/wavetable-research; do
 if git rev-parse --verify --quiet "$ref^{commit}" >/dev/null; then
  printf '%s: %s\n' "$ref" "$(git rev-parse "$ref")"
 elif git rev-parse --verify --quiet "origin/$ref^{commit}" >/dev/null; then
  printf '%s: %s\n' "$ref" "$(git rev-parse "origin/$ref")"
 else
  printf '%s: NOT FETCHED (fetch only if required for current job)\n' "$ref"
 fi
done
printf '\nKey source paths present:\n'
for f in Core/CMakeLists.txt tests/run_host.sh Core/Src/Soundengine/DSPAudio/ResonantFilter.c Core/Inc/Soundengine/DSPAudio/ResonantFilter.h Core/Inc/menu/MenuText.h docs/CODEX_START_HERE.md docs/CODEX_SOURCE_REGISTER.md docs/CODEX_COMPREHENSIVE_HANDOVER.md; do
 if test -f "$f"; then printf 'OK %s\n' "$f"; else printf 'MISSING %s\n' "$f"; fi
done
printf '\nNo files, branches or commits were modified.\n'
