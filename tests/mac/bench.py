"""Run the installed Intel production renderers. Uses Python 3 standard library."""
import argparse
import array
import hashlib
import json
import math
from pathlib import Path
import platform
import re
import struct
import subprocess
import sys
import tempfile
import wave

RES = Path(__file__).resolve().parent
RATE = 44140


def varint(n):
    digits = [n & 127]
    n >>= 7
    while n:
        digits.append(128 | (n & 127))
        n >>= 7
    return bytes(reversed(digits))


def write_midi(events, dest):
    # 22070 ticks per beat at 120 BPM equals one tick per source frame.
    track = bytearray(b'\x00\xff\x51\x03\x07\xa1\x20')
    previous = 0
    for at, status, note, vel in events:
        track.extend(varint(at - previous) + bytes((status, note, vel)))
        previous = at
    track.extend(b'\x00\xff\x2f\x00')
    dest.write_bytes(b'MThd' + struct.pack('>IHHH', 6, 0, 1, 22070)
                     + b'MTrk' + struct.pack('>I', len(track)) + track)


def main():
    p = argparse.ArgumentParser()
    p.add_argument('group', choices=['all','noclip','stepfx','layers','cpu','crunch'])
    p.add_argument('output', type=Path)
    a = p.parse_args()
    a.output.mkdir(parents=True, exist_ok=True)
    out = Path(tempfile.mkdtemp(prefix=a.group+'-', dir=a.output))
    checks, metrics = {}, {}
    def check(key, ok):
        checks[key] = bool(ok)
        print(('PASS ' if ok else 'FAIL ')+key, flush=True)

    def render(name, engine, mode=0, voices=6, slot=0, shape=0):
        raw = out/(name+'.s16')
        cmd = [str(RES/engine),str(RES/'events.txt'),str(mode),str(raw),
               str(voices),str(slot),str(shape)]
        log = subprocess.check_output(cmd, cwd=out, text=True)
        (out/(name+'.log')).write_text(log)
        pcm = raw.read_bytes()
        with wave.open(str(out/(name+'.wav')), 'wb') as w:
            w.setparams((2,2,RATE,0,'NONE','not compressed'))
            w.writeframes(pcm)
        raw.unlink()
        x = array.array('h',pcm)
        timing = re.search(r'64-frame calls (\d+) mean_us ([\d.]+) peak_us ([\d.]+)',log)
        peak = max(abs(min(x)),max(x))
        m = dict(sha256=hashlib.sha256(pcm).hexdigest(), frames=len(x)//2,
                 peak=peak, peak_dbfs=20*math.log10(max(1,peak)/32768),
                 full_scale=x.count(-32768)+x.count(32767),
                 max_adjacent=max(abs(b-a) for a,b in zip(x,x[2:])),
                 host_calls=int(timing[1]),mean_us=float(timing[2]),peak_us=float(timing[3]),
                 engine=engine,mode=mode,enabled_voices=voices,stepfx_slot=slot+1,shape=shape)
        metrics[name]=m
        check(name+' complete audible render', len(x)==RATE*12*2 and peak>0)
        return m

    def group(name):
        return a.group in ('all',name)

    try:
        events = [tuple(map(int,l.split())) for l in (RES/'events.txt').read_text().splitlines() if l.strip()]
        write_midi(events, out/'performance.mid')
        dry = render('NoClip','baseline')
        bypass = render('Combined-off','combined')
        check('Mode 0 PCM identical to No Clip', dry['sha256']==bypass['sha256'])
        if group('noclip'):
            original = render('Erica','original')
            check('No Clip changes retrigger performance',original['sha256']!=dry['sha256'])
        if group('stepfx'):
            subprocess.run([str(RES/'combined'),'ui','0','/dev/null'],cwd=out,check=True)
            fx = render('Pitched-repeats','combined',slot=7)
            check('Pitched repeats audible with both layers off',fx['sha256']!=dry['sha256'])
        if group('layers'):
            previous = set([dry['sha256']])
            for name,mode,shape in [('Metallic',1,0),('Wavetable',2,0),('WaveShape',2,1)]:
                m = render(name,'combined',mode,shape=shape)
                check(name+' distinct PCM',m['sha256'] not in previous)
                previous.add(m['sha256'])
        if group('crunch') or group('cpu'):
            for n in [1,2,3,4,6]:
                m = render('Overlap-'+str(n),'combined',3,n)
                check(str(n)+' voices no additional rail samples',m['full_scale']<=dry['full_scale'])
            both=render('Layers-plus-StepFX','combined',3,slot=7)
            check('combined effects no additional rail samples',both['full_scale']<=dry['full_scale'])
            again=render('Repeat-six','combined',3)
            check('six-voice replay deterministic',again['sha256']==m['sha256'])
        if group('cpu'):
            subprocess.run([str(RES/'diagnostics')],cwd=out,check=True)
            check('diagnostic counters and reset',True)
    except Exception as exc:
        check(str(exc),False)
    report=dict(source=(RES/'revision.txt').read_text().strip(),host=platform.platform(),
                seed=123, fader=0.4, sample_rate=RATE, group=a.group, checks=checks, metrics=metrics)
    (out/'results.json').write_text(json.dumps(report,indent=2))
    print('Results saved in '+str(out),flush=True)
    return int(not all(checks.values()))

if __name__=='__main__':
    sys.exit(main())
