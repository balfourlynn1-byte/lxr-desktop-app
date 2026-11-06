#!/usr/bin/env python3
"""Run the same production MIDI/DSP scenario against three source trees."""
from pathlib import Path
import argparse,subprocess,array,json,hashlib,wave,math
p=argparse.ArgumentParser();p.add_argument('baseline',type=Path);p.add_argument('stepfx',type=Path);p.add_argument('metal',type=Path);p.add_argument('--out',type=Path,default=Path('results'));a=p.parse_args();root=Path(__file__).resolve().parent;out=a.out.resolve();out.mkdir(parents=True,exist_ok=True)
def run(cmd,log):
 with log.open('w') as f:subprocess.run(list(map(str,cmd)),stdout=f,stderr=subprocess.STDOUT,check=True)
for name,fw in [('baseline',a.baseline),('stepfx',a.stepfx),('metal',a.metal)]:
 build=out/('build-'+name)
 run(['cmake','-S',root/'emu','-B',build,'-DFW='+str(fw.resolve())],out/(name+'-config.log'))
 run(['cmake','--build',build,'-j8'],out/(name+'-build.log'))
 if name=='stepfx':run([build/'regression','ui','0','/dev/null'],out/'stepfx-ui.log')
 modes=[0,4] if name=='baseline' else ([0,1] if name=='stepfx' else [0,1,2,3,4])
 for mode in modes:run([build/'regression',root/'events.txt',mode,out/(name+str(mode)+'.s16')],out/(name+str(mode)+'.log'))
metrics={}
for file in sorted(out.glob('*.s16')):
 data=file.read_bytes();samples=array.array('h',data);peak=max(abs(x) for x in samples);power=sum(x*x for x in samples)/len(samples)
 clips=sum(x in (-32768,32767) for x in samples)
 jumps=max(abs(samples[i]-samples[i-2]) for i in range(2,len(samples)))
 # Single-pole 100 Hz lowpass energy is a repeatable bass proxy, not spectral perfection.
 low=[0.,0.];energy=0.;alpha=1-math.exp(-2*math.pi*100/44140)
 for i,x in enumerate(samples):c=i%2;low[c]+=alpha*(x-low[c]);energy+=low[c]**2
 metrics[file.stem]=dict(sha256=hashlib.sha256(data).hexdigest(),frames=len(samples)//2,peak=peak,rms=math.sqrt(power),full_scale_samples=clips,max_adjacent_change=jumps,bass_100Hz_lowpass_rms=math.sqrt(energy/len(samples)))
 with wave.open(str(file.with_suffix('.wav')),'wb') as f:f.setparams((2,2,44140,0,'NONE','not compressed'));f.writeframes(data)
assert metrics['baseline0']['sha256']==metrics['stepfx0']['sha256']==metrics['metal0']['sha256'],'Bypass differs'
assert metrics['baseline0']['peak']>0,'Silent test'
for n in ['stepfx1','metal1','metal2','metal3']:assert metrics[n]['sha256']!=metrics['baseline0']['sha256'],n+' inactive'
# Equivalent controller/manual streams are represented by identical raw MIDI: replay for identity.
run([out/'build-metal/regression',root/'events.txt',3,out/'replay.s16'],out/'replay.log')
assert (out/'replay.s16').read_bytes()==(out/'metal3.s16').read_bytes()
(out/'metrics.json').write_text(json.dumps(metrics,indent=2));print(json.dumps(metrics,indent=2));print('PASS production bypass identity, active effects and deterministic equivalent MIDI replay')
