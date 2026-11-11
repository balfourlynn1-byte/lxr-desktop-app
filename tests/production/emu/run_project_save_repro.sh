#!/bin/sh
# Cold-start regression for interrupted replacement of a saved project.
set -eu
for n in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40; do
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
