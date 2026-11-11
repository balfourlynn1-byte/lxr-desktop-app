#!/bin/sh
set -eu
for n in 1 2 3 4 5 6 7 8 9 10 11 12; do
    card="$1-write-$n.sdchunks"
    echo "write fault $n"
    JB_FAIL_WRITE="$n" "$BPM_PERSIST_EXE" save_fault "$card"
    "$BPM_PERSIST_EXE" verify_fault "$card"
done
for n in 1 2 3 4; do
    card="$1-sync-$n.sdchunks"
    echo "sync fault $n"
    JB_FAIL_SYNC="$n" "$BPM_PERSIST_EXE" save_fault "$card"
    "$BPM_PERSIST_EXE" verify_fault "$card"
done
echo "zero-byte primary recovery"
card="$1-corrupt.sdchunks"
"$BPM_PERSIST_EXE" save_corrupt "$card"
"$BPM_PERSIST_EXE" verify_old "$card"
echo "full-card recovery"
card="$1-full.sdchunks"
"$BPM_PERSIST_EXE" save_full "$card"
"$BPM_PERSIST_EXE" verify_old "$card"
