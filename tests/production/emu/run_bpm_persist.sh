#!/bin/sh
set -eu
"$BPM_PERSIST_EXE" save_explicit "$1"
"$BPM_PERSIST_EXE" verify "$1"
