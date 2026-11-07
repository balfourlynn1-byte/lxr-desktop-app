#!/bin/bash
# Build on Intel macOS with Apple CLT, CMake and Ninja.
set -eu
original=$(cd "$1" && pwd)
baseline=$(cd "$2" && pwd)
mkdir -p "$3"
dest=$(cd "$3" && pwd)
cd "$(dirname "$0")/../.."
mkdir -p build
stage=$(mktemp -d "$PWD/build/mac.XXXXXX")
app="$stage/JIMBONES.app"
res="$app/Contents/Resources"
mkdir -p "$res" "$app/Contents/MacOS" "$stage/audio" "$stage/cpu"
test "$(uname -m)" = x86_64
for engine in original baseline combined; do
    case "$engine" in original) src="$original";; baseline) src="$baseline";; combined) src="$PWD";; esac
    cmake -G Ninja -S tests/production/emu -B "build/$engine" -DFW="$src"
    cmake --build "build/$engine" --target regression -j4
    cp "build/$engine/regression" "$res/$engine"
done
clang -arch x86_64 -O2 -fobjc-arc -framework Cocoa tests/mac/Launcher.m -o "$app/Contents/MacOS/Bench"
clang -O2 -DLXR_EMU=1 -ICore/Inc tests/test_audio_diagnostics.c Core/Src/Soundengine/AudioDiagnostics.c -o "$res/diagnostics"
cp tests/mac/bench.py tests/production/events.txt LICENSE NOTICE.md "$res/"
git rev-parse HEAD > "$res/revision.txt"
python3 - "$app/Contents/Info.plist" <<'PY'
import plistlib,sys
with open(sys.argv[1],'wb') as f:
    plistlib.dump(dict(CFBundleIdentifier='org.jimbones.macbench', CFBundleExecutable='Bench',
        CFBundleName='JIMBONES',CFBundlePackageType='APPL',CFBundleShortVersionString='1.7.99',
        CFBundleVersion='99',NSHighResolutionCapable=True),f)
PY
python3 "$res/bench.py" all "$stage/audio" > "$stage/cpu/test.log" || { cat "$stage/cpu/test.log"; exit 1; }
report=$(find "$stage/audio" -name results.json)
folder=$(dirname "$report")
cp "$folder/NoClip.wav" "$folder/Layers-plus-StepFX.wav" "$res/"
cp "$report" "$stage/cpu/"
for log in build/host.log build/arm.log; do
    if test -f "$log"; then cp "$log" "$stage/cpu/"; fi
done
cp tests/mac/README.md "$stage/"
cp tests/mac/README.md "$stage/cpu/README.md"
cp tests/mac/README.md "$stage/audio/README.md"
for dir in "$stage/audio" "$stage/cpu"; do cp "$res/revision.txt" "$dir/"; done
cd "$stage"
ditto -c -k --keepParent audio "$dest/JIMBONES-Audio-Replay-Tests.zip"
ditto -c -k --keepParent cpu "$dest/JIMBONES-CPU-Diagnostics.zip"
mkdir Mac-Test-Bench
mv JIMBONES.app README.md Mac-Test-Bench/
ditto -c -k --keepParent Mac-Test-Bench "$dest/JIMBONES-Mac-Test-Bench.zip"
