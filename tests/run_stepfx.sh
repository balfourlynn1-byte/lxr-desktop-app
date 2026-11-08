#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host
# The original test_stepfx.c assumes all voices are armed by default.
# The new stepfx engine safely defaults to snare and hi-hat, so exercise
# its current grid/phrase/voice-mask regression instead.
bash tests/run_combined_stepfx.sh
sh tests/run_stepfx_ui.sh
sh tests/retrigger/run_tests_mac.sh
