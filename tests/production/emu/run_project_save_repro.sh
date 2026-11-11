#!/bin/sh
# Known failing regression: an interrupted full-project save can delete the
# previously valid project. Keep this red until project replacement is safe.
set -eu
card="${1:-/tmp/jb-project-save-repro.sdchunks}"
JB_FAIL_WRITE=2 "$BPM_PERSIST_EXE" project_fault "$card"
"$BPM_PERSIST_EXE" verify_project "$card"
