#!/usr/bin/env python3
"""Replay production MIDI/DSP/mixer against baseline and combined firmware.
This host emulator is NOT a cycle-accurate ARM or physical hardware test.
"""
from pathlib import Path
import argparse, subprocess, array, hashlib, wave, json, math, time
p=argparse.ArgumentParser()
p.add_argument("baseline",type=Path)
p.add_argument("combined",type=Path)
p.add_argument("--out",type=Path,default=Path("build/production-combined"))
a=p.parse_args()
root=Path(__file__).resolve().parent
out=a.out.resolve()
out.mkdir(parents=True,exist_ok=True)
def run(cmd, logfile):
    with (out/logfile).open("w") as f:
        subprocess.run(list(map(str,cmd)),stdout=f,stderr=subprocess.STDOUT,check=True)
builds={}
for name,fw in (("baseline",a.baseline),("combined",a.combined)):
    build=out/("build-"+name)
    run(["cmake","-S",root/"emu","-B",build,"-DFW="+str(fw.resolve())],name+"-config.log")
    run(["cmake","--build",build,"-j4"],name+"-build.log")
    builds[name]=build
run([builds["combined"]/"regression","ui","0","/dev/null"],"combined-ui.log")
for name,modes in (("baseline",(0,4)),("combined",(0,1,2,3,4))):
    for mode in modes:
        run([builds[name]/"regression",root/"events.txt",mode,out/(name+str(mode)+".s16")],f"{name}{mode}.log")
metrics={}
for file in sorted(out.glob("*.s16")):
    data=file.read_bytes()
    values=array.array("h")
    values.frombytes(data)
    if not values: raise AssertionError(f"empty render: {file}")
    peak=max(abs(x) for x in values)
    full=sum(x in (-32768,32767) for x in values)
    rms=math.sqrt(sum(x*x for x in values)/len(values))
    metrics[file.stem]=dict(sha256=hashlib.sha256(data).hexdigest(),frames=len(values)//2,
        peak=peak,peak_dBFS=20*math.log10(peak/32768) if peak else -120,
        rms=rms,full_scale_samples=full,
        max_adjacent_change=max(abs(values[i]-values[i-2]) for i in range(2,len(values))))
    with wave.open(str(file.with_suffix(".wav")),"wb") as w:
        w.setparams((2,2,44140,0,"NONE","not compressed"))
        w.writeframes(data)
assert metrics["baseline0"]["sha256"]==metrics["combined0"]["sha256"],"MODE 0 PRODUCTION MIXER DIFFERS FROM NO CLIP BASELINE"
assert metrics["baseline0"]["peak"]>0,"baseline playback silent"
for mode in (1,2,3):
    assert metrics[f"combined{mode}"]["sha256"]!=metrics["combined0"]["sha256"],f"combined mode {mode} inactive"
# Full-scale stress is informational (baseline can clip at max faders).
for mode in (1,2,3):
    clips=metrics[f"combined{mode}"]["full_scale_samples"]
    assert clips<=metrics["baseline0"]["full_scale_samples"],f"combined mode {mode}: {clips} new full-scale samples; baseline {metrics['baseline0']['full_scale_samples']}"
# A second render with identical seed/MIDI must be bit-reproducible.
run([builds["combined"]/"regression",root/"events.txt",3,out/"replay.s16"],"combined-replay.log")
assert (out/"replay.s16").read_bytes()==(out/"combined3.s16").read_bytes(),"nondeterministic combined render"
(out/"metrics.json").write_text(json.dumps(metrics,indent=2))
print(json.dumps(metrics,indent=2))
print("PASS: production mixer Mode 0 identity; modes 1/2/3 active; matched-input headroom; UI selectors; deterministic MIDI")
print("IMPORTANT: host emulation cannot prove ARM CPU deadlines or physical MIDI/pedal timing")
