#!/usr/bin/env python3
"""C02: independent, host-only four-frame periodic waveform scanner audio fixture.
Not a port of non-commercial upstream code; NOT wired to LXR audio/firmware.
"""
import math
import pathlib
import struct
import wave

RATE = 44140
LENGTH = RATE // 2
TABLE = 256
FRAMES = 4
OUT = pathlib.Path("build/host/wavetable-c02.wav")

def clamp16(x):
    return max(-32768, min(32767, int(round(x))))

def frame_value(frame, phase):
    x = phase / TABLE
    if frame == 0:
        return math.sin(2 * math.pi * x)
    if frame == 1:
        return 2 * x - 1
    if frame == 2:
        return 4 * abs(x - .5) - 1
    return 1.0 if x < .5 else -1.0

def scan(position, phase):
    pos = max(0., min(float(FRAMES - 1), position))
    a = int(pos)
    b = min(FRAMES - 1, a + 1)
    mix = pos - a
    return (1 - mix) * frame_value(a, phase) + mix * frame_value(b, phase)

def main():
    assert scan(-10, 64) == scan(0, 64)
    assert scan(999, 64) == scan(3, 64)
    for p in (0, .25, 1, 1.5, 2.9, 3):
        for phase in (0, 32, 127, 255):
            assert -1.000001 <= scan(p, phase) <= 1.000001
    assert abs(scan(1.5, 64) - (scan(1, 64) + scan(2, 64)) / 2) < 1e-12
    samples = []
    for n in range(LENGTH):
        phase = (n * 110 * TABLE // RATE) % TABLE
        position = 3 * n / max(1, LENGTH - 1)
        samples.append(clamp16(scan(position, phase) * 12000))
    assert len(set(samples)) > 100
    OUT.parent.mkdir(parents=True, exist_ok=True)
    with wave.open(str(OUT), "wb") as f:
        f.setnchannels(1)
        f.setsampwidth(2)
        f.setframerate(RATE)
        f.writeframes(struct.pack("<" + "h" * len(samples), *samples))
    print(f"C02 PASS: {OUT}, {LENGTH} frames, 44140 Hz, mono PCM16; isolated host fixture only")

if __name__ == "__main__":
    main()
