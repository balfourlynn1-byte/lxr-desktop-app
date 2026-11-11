#!/bin/sh
# Cold-start regression for interrupted replacement of a saved project.
set -eu

# Fresh project slot must save and cold-boot with its own tempo; this covers
# integrity validation introduced before the old missing-directory path.
"$BPM_PERSIST_EXE" project_fresh_slot "$1-fresh.sdchunks"
"$BPM_PERSIST_EXE" verify_fresh_slot "$1-fresh.sdchunks"

# Three clean replacements must succeed before fault injection; this also
# checks that the newest BPM survives a separate process cold start.
"$BPM_PERSIST_EXE" project_repeat "$1-repeat.sdchunks"
"$BPM_PERSIST_EXE" verify_repeat "$1-repeat.sdchunks"
last_write=$("$BPM_PERSIST_EXE" project_measure "$1-measure.sdchunks")
case "$last_write" in *[!0-9]*|'') exit 2;; esac
echo "project replacement low-level writes=$last_write"
late_one=$((last_write - 1))
late_two=$((last_write - 2))
late_three=$((last_write - 3))
for n in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 1000 2000 "$late_three" "$late_two" "$late_one" "$last_write"; do
    card="$1-$n.sdchunks"
    JB_FAIL_WRITE="$n" "$BPM_PERSIST_EXE" project_fault "$card"
    "$BPM_PERSIST_EXE" verify_project "$card"
done

# Also interrupt FatFs sync/flush calls during the staged project replacement.
for n in 1 2 3 4; do
    card="$1-sync-$n.sdchunks"
    JB_FAIL_SYNC="$n" "$BPM_PERSIST_EXE" project_fault "$card"
    "$BPM_PERSIST_EXE" verify_project "$card"
done

# Fail immediately before and immediately after both commit renames. The
# after cases model a completed filesystem call whose success was lost.
for n in 1 2; do
    for after in 0 1; do
        card="$1-rename-$n-$after.sdchunks"
        JB_FAIL_RENAME="$n" JB_FAIL_AFTER="$after" "$BPM_PERSIST_EXE" project_rename_fault "$card"
        "$BPM_PERSIST_EXE" verify_project "$card"
    done
done

# A genuinely full FAT32 volume must keep the old generation. Once space is
# released, a new save must succeed and survive another cold start.
card="$1-full.sdchunks"
"$BPM_PERSIST_EXE" project_full "$card"
"$BPM_PERSIST_EXE" verify_project "$card"
"$BPM_PERSIST_EXE" project_retry "$card"
"$BPM_PERSIST_EXE" verify_project "$card"

card="$1-removed.sdchunks"
"$BPM_PERSIST_EXE" project_removed "$card"
"$BPM_PERSIST_EXE" verify_project "$card"

# Existing unmarked project files must still load, and their first replacement
# must add a complete manifest without changing kit/pattern/song formats.
card="$1-legacy.sdchunks"
"$BPM_PERSIST_EXE" legacy_project "$card"
"$BPM_PERSIST_EXE" verify_legacy "$card"
"$BPM_PERSIST_EXE" project_retry "$card"
"$BPM_PERSIST_EXE" verify_project "$card"

# A failed replacement must not poison the next save after restart.
card="$1-retry.sdchunks"
JB_FAIL_WRITE="$last_write" "$BPM_PERSIST_EXE" project_fault "$card"
"$BPM_PERSIST_EXE" verify_project "$card"
"$BPM_PERSIST_EXE" project_retry "$card"
"$BPM_PERSIST_EXE" verify_project "$card"
