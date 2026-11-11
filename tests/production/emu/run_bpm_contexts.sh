#!/bin/sh
set -eu
"$BPM_PERSIST_EXE" contexts "$1"
"$BPM_PERSIST_EXE" verify_context "$1"
"$BPM_PERSIST_EXE" save_sync "$1"
"$BPM_PERSIST_EXE" verify_sync "$1"
