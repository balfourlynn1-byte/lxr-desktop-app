#!/usr/bin/env python3
"""Exercise the production drum trigger against stateful peripheral/DSP stand-ins."""
from pathlib import Path
import subprocess,sys,os
root=Path(__file__).resolve().parents[1]
p='Core/Src/Soundengine/DSPAudio/DrumVoice.c'
s=subprocess.check_output(['git','show',sys.argv[1]+':'+p],cwd=root).decode('latin1') if len(sys.argv)>1 else (root/p).read_text(encoding='latin1')
s=s.replace('\r\n','\n'); start=s.index('void Drum_trigger('); end=s.index('\n}\n',start)+3
fixture=r'''
#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#define SINE 0
#define REC 3
#define EG_STOPPED 0
struct Osc { uint32_t phase; int waveform; int note; };
struct EG { int state; };
struct Trans { int waveform; float volume; };
static struct { struct Osc osc,modOsc; struct EG oscVolEg; struct Trans transGen; double ampFilterInput; float velo; int filter,oscPitchEg,snapEg; } voiceArray[3];
static int velocityModulators[3],filterResets,egTriggers,layerTriggers;
static void jb_trigger(unsigned v,unsigned vel,unsigned n,unsigned o) { (void)v;(void)vel;(void)n;(void)o;layerTriggers++; }
static void lfo_retrigger(unsigned v) { (void)v; }
static void modNode_updateValue(int *v,float x) { (void)v;(void)x; }
static void osc_setBaseNote(struct Osc *o,unsigned n) { o->note=n; }
static void DecayEg_trigger(int *e) { (void)e; }
static void slopeEg2_trigger(struct EG *e) { e->state=1;egTriggers++; }
static void transient_trigger(struct Trans *t) { (void)t; }
static void SnapEg_trigger(int *e) { (void)e; }
static void SVF_reset(int *f) { *f=0;filterResets++; }
static void Drum_updatePan(unsigned v) { (void)v; }
'''
test=r'''
int main(void) {
 for(unsigned v=0;v<3;v++) {
  voiceArray[v].osc.phase=123456;voiceArray[v].modOsc.phase=765432;
  voiceArray[v].oscVolEg.state=2;voiceArray[v].ampFilterInput=.7;voiceArray[v].filter=99;
  int before=filterResets;
  Drum_trigger(v,90,54);
  assert(voiceArray[v].osc.phase==123456 && voiceArray[v].modOsc.phase==765432);
  assert(filterResets==before && voiceArray[v].filter==99);
  assert(voiceArray[v].osc.note==54 && voiceArray[v].modOsc.note==54);
  assert(voiceArray[v].velo>.7 && voiceArray[v].oscVolEg.state==1);
  voiceArray[v].oscVolEg.state=EG_STOPPED;
  Drum_trigger(v,100,48); assert(voiceArray[v].osc.phase!=123456 && filterResets==before+1);
  voiceArray[v].osc.phase=123456;voiceArray[v].ampFilterInput=0;
  Drum_trigger(v,100,48); assert(voiceArray[v].osc.phase!=123456 && filterResets==before+2);
  voiceArray[v].osc.phase=123456;voiceArray[v].ampFilterInput=.7;voiceArray[v].transGen.waveform=1;
  Drum_trigger(v,100,48); assert(voiceArray[v].osc.phase!=123456 && filterResets==before+3);
 }
 assert(egTriggers==12 && layerTriggers==12);
 puts("PASS original drum trigger: ringing phase/filter retained; silent and explicit-offset reset; pitch, velocity and envelope retrigger retained");
}
'''
out=root/'build/host/original_retrigger.c';out.write_text(fixture+s[start:end]+test)
subprocess.run([os.environ.get('CC','cc'),'-std=c11','-O1','-g','-Wall','-Wextra','-Werror','-fsanitize=address,undefined',str(out),'-o',str(out.with_suffix(''))],check=True)
subprocess.run([str(out.with_suffix(''))],check=True)
