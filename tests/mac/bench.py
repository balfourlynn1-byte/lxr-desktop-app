"""Run the installed Intel production renderers. Uses Python 3 standard library."""
import argparse
import array
import hashlib
import json
import math
import os
from mixed_kit import run_mixed
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
    p.add_argument('group', choices=['all','noclip','stepfx','layers','cpu','crunch','custom'])
    p.add_argument('output', type=Path)
    p.add_argument('controls',nargs='*',help='NRPN index=value for custom render')
    a = p.parse_args()
    preset=json.loads((RES/'aggressive-reference.json').read_text())
    custom={int(k):v for k,v in preset['controls'].items()}
    for entry in a.controls:
        key,value=map(int,entry.split('='))
        if key not in custom or not 0<=value<=127: p.error('invalid custom control')
        custom[key]=value
    a.output=a.output.resolve()
    a.output.mkdir(parents=True, exist_ok=True)
    out = Path(tempfile.mkdtemp(prefix=a.group+'-', dir=a.output))
    checks, metrics = {}, {}
    def check(key, ok):
        checks[key] = bool(ok)
        print(('PASS ' if ok else 'FAIL ')+key, flush=True)

    def render(name, engine, mode=0, voices=6, slot=0, shape=0, controls=None, events_file=None, mask=None, fader=None, fxmask=63, balance=False, save=True):
        raw = out/(name+'.s16')
        cmd = [str(RES/engine),str(events_file or RES/'events.txt'),str(mode),str(raw),
               str(voices),str(slot),str(shape)]
        cmd.extend(f"{k}={v}" for k,v in (controls or {}).items())
        layer_mask=(1<<voices)-1 if mask is None else mask
        percent=(100 if mode==4 else 40) if fader is None else fader
        env=dict(os.environ,JB_MASK=str(layer_mask),JB_FADER=str(percent/100),
                 JB_LIVE=str(int(balance)),JB_FX_MASK=str(fxmask))
        log = subprocess.check_output(cmd, cwd=out, text=True,env=env)
        (out/(name+'.log')).write_text(log)
        pcm = raw.read_bytes()
        with wave.open(str(out/(name+'.wav')), 'wb') as w:
            w.setparams((2,2,RATE,0,'NONE','not compressed'))
            w.writeframes(pcm)
        if not save: (out/(name+'.wav')).unlink()
        raw.unlink()
        x = array.array('h',pcm)
        timing = re.search(r'64-frame calls (\d+) mean_us ([\d.]+) peak_us ([\d.]+)',log)
        peak = max(abs(min(x)),max(x))
        m = dict(sha256=hashlib.sha256(pcm).hexdigest(), frames=len(x)//2,
                 peak=peak, peak_dbfs=20*math.log10(max(1,peak)/32768),
                 full_scale=x.count(-32768)+x.count(32767),
                 max_adjacent=max(abs(b-a) for a,b in zip(x,x[2:])),
                 host_calls=int(timing[1]),mean_us=float(timing[2]),peak_us=float(timing[3]),
                 engine=engine,mode=mode,layer_mask=layer_mask, fx_mask=fxmask,live_balance=balance, fader=percent/100, controls=controls,stepfx_slot=slot+1,shape=shape)
        largest=m['max_adjacent']
        at=next(i for i in range(2,len(x)) if abs(x[i]-x[i-2])==largest)
        m.update(largest_step_frame=at//2,largest_step_channel=at%2+1,
                 largest_step_seconds=(at//2)/RATE,
                 events=(events_file.name if events_file else 'performance.mid'))
        metrics[name]=m
        check(name+' complete audible render', len(x)==RATE*12*2 and peak>0)
        return m

    def group(name):
        return a.group in ('all',name)

    try:
        events = [tuple(map(int,l.split())) for l in (RES/'events.txt').read_text().splitlines() if l.strip()]
        write_midi(events, out/'performance.mid')
        program=[]
        # All legacy synthesis defaults in OSC page order, then v1.8 controls.
        params=[3,64,64,127,32,0,0,0,64,64,64,64,48,24,32,0,64,0,64,20,0,0,0,127,0,0,0,64,127,64,127,64]
        for voice in range(6):
            for key,val in enumerate(params):
                for cc,data in ((99,96+voice),(98,key),(6,val)):
                    program.append((len(program)*44,0xb0+voice,cc,data))
        write_midi(program,out/'Aggressive-reference-settings.mid')
        custom_preset=json.loads((RES/'custom-reference.json').read_text())
        approved={int(k):v for k,v in custom_preset['controls'].items()}
        custom_program=[(at,status,cc,approved.get(program[i-1][3],val) if cc==6 else val)
                        for i,(at,status,cc,val) in enumerate(program)]
        write_midi(custom_program,out/'Custom-reference-settings.mid')
        dry = render('NoClip','baseline')
        bypass = render('Combined-off','combined')
        check('Mode 0 PCM identical to No Clip', dry['sha256']==bypass['sha256'])
        if a.group=='custom':
            render('My-mix','combined',3,slot=7,controls=custom)
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
            original=render('Approved-v1799','reference',3,slot=7)
            configured=render('Aggressive-reference','combined',3,slot=7,
                              controls={int(k):v for k,v in preset['controls'].items()})
            check('approved attacks and entire reference PCM identical',
                  original['sha256']==configured['sha256']==both['sha256'])
            for n in range(1,7):
                lanes=[3,0,1,2,5,4][:n]
                # Equal onset frames, rapid rolls and alternating ghost/accent hits.
                sequence=[(frame,144+voice,48+(hit%4)*3,32 if hit%3==1 else 120)
                          for hit,frame in enumerate(range(0,RATE*6,1103))
                          for voice in lanes]
                eventpath=out/f'Simultaneous-{n}.txt'
                eventpath.write_text(''.join('%d %d %d %d\n'%row for row in sequence))
                write_midi(sequence,eventpath.with_suffix('.mid'))
                isolated=render(f'Simultaneous-{n}','combined',4,6,7,events_file=eventpath)
                check(f'{n} simultaneous voices at full faders no rails',isolated['full_scale']==0)

                before=render(f'Full-before-{n}','reference',4,n,7)
                after=render(f'Full-after-{n}','combined',4,n,7)
                check(f'full faders {n} lanes reduce full-scale hits',after['full_scale']<before['full_scale'])
            check('fully layered full-fader mix has zero rail samples',after['full_scale']==0)
            muted=render('Levels-zero','combined',4,slot=7,controls={1:0,2:0,5:127,17:127})
            dry_fx=render('Full-dry-with-repeats','combined',4,0,7)
            check('zero levels restore full-fader dry PCM with noise and repeats',muted['sha256']==dry_fx['sha256'])
            silent_fx=render('Zero-repeat-amount','combined',3,slot=7,controls={30:0})
            check('zero repeat amount leaves original hits intact',silent_fx['sha256']==again['sha256'])
            zero_mix=render('Zero-blend','combined',3,controls={28:0,5:127})
            check('zero blend preserves original PCM',zero_mix['sha256']==dry['sha256'])
            for param,value in [(1,12),(2,0),(24,90),(29,118),(20,59),(30,45),(28,45),(31,117)]:
                adjusted=render(f'Control-{param}','combined',3,slot=7,controls={param:value})
                check(f'MIDI control {param} changes rendered sound', adjusted['sha256']!=both['sha256'])
            previous=render('Approved-custom-v180','previous',3,slot=7,controls=approved)
            current=render('Approved-custom-v181','combined',3,slot=7,controls=approved,balance=True)
            check('user custom recording exact PCM',previous['sha256']==current['sha256']==custom_preset['pcm_sha256_intel_local'])
            check('original approved recording exact PCM',both['sha256']==preset['pcm_sha256_intel_local'])
            run_mixed(render,check,out,write_midi,approved)
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
