#!/usr/bin/env python3
"""Exercise actual kit/RAM/autosave functions with an in-memory FatFs stand-in."""
from pathlib import Path
import os, re, subprocess
root=Path(__file__).resolve().parents[1]
s=(root/'Core/Src/Preset/presetManager.c').read_text(encoding='latin1')
names=['preset_saveTmpKit','preset_LoadTmpKit','preset_writeDrumsetData','preset_loadDrumsetRam','preset_saveDrumsetRam','preset_saveDrumsetSD','preset_readDrumsetData','preset_loadDrumsetSD']
functions=[]
for name in names:
 m=re.search(r'^(?:static )?(?:void|FRESULT) '+name+r'\([^\n]*\)\n\{',s,re.M)
 assert m,name
 end=re.search(r'^};?$',s[m.end():],re.M)
 functions.append(s[m.start():m.end()+end.end()])
fixture=r'''
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "Preset/JimbonesPreset.h"
#define USE_SD_CARD 1
#define USE_INTERRUPT_ONLY_DURING_SAVE_PROJ 0
#define CALC_SAMPLES_IN_INTERRUPT 1
#define NUM_KIT_SLOTS 64
#define END_OF_SOUND_PARAMETERS 64
#define KIT_NAMES 0
#define SAVE_TYPE_KIT 0
#define PAR_VEL_DEST_1 0
#define PAR_VEL_DEST_6 5
#define PAR_TARGET_LFO1 6
#define PAR_TARGET_LFO6 11
#define PAR_VOICE_LFO1 12
#define PAR_VOICE_LFO6 17
#define PAR_MORPH 18
#define FEXT_SOUND 0
#define FA_CREATE_ALWAYS 1
#define FA_WRITE 2
#define FA_OPEN_EXISTING 4
#define FA_READ 8
#define FR_OK 0
#define FR_DISK_ERR 1
#define FR_NO_FILE 2
typedef int FIL;
typedef int FRESULT;
typedef unsigned UINT;
static FIL SDFile;
static uint8_t jb_kits[64][JB_KIT_BYTES],mainParameters[64][64],activeParameters[64];
static uint8_t morphBuffer[64],*morphParameters=morphBuffer;
static char savefileNames[64][1][8];
static uint8_t menu_selectedPresetNr[1];
static uint8_t disk[4096]; static unsigned length,cursor;
static FRESULT f_open(FIL *f,const char *name,int flags) { (void)f;(void)name; cursor=0; if(flags&1) length=0; return FR_OK; }
static FRESULT f_close(FIL *f) { (void)f; return FR_OK; }
static FRESULT f_write(FIL *f,const void *buf,unsigned n,unsigned *got) {
 (void)f; assert(cursor+n<=sizeof(disk)); memcpy(disk+cursor,buf,n); cursor+=n; length=cursor; *got=n; return FR_OK;
}
static FRESULT f_read(FIL *f,void *buf,unsigned n,unsigned *got) {
 (void)f; if(n>length-cursor) n=length-cursor; memcpy(buf,disk+cursor,n); cursor+=n; *got=n; return FR_OK;
}
static unsigned getNumModTargets(void) { return 127; }
static void preset_sendDrumsetParameters(void) {}
static void preset_sendDrumsetParametersImmediate(void) {}
static void soundengine_process(void) {}
static uint8_t preset_getMorphValue(unsigned i,uint8_t amount) { (void)amount; return activeParameters[i]; }
static void preset_makeFileNameSave(char *s,unsigned n,unsigned ext,char *name) { (void)n;(void)ext;(void)name; strcpy(s,"kit.snd"); }
static void preset_makeFileNameOld(char *s,unsigned n,unsigned ext) { (void)n;(void)ext; strcpy(s,"kit.snd"); }
static FRESULT preset_makeFileNameLoad(char *s,unsigned n,unsigned ext) { preset_makeFileNameOld(s,n,ext); return f_open(&SDFile,s,FA_READ); }
static int preset_isNameEmpty(char *name) { return name[0]==0; }
'''
test=r'''
int main(void) {
 uint8_t expected[JB_KIT_BYTES],defaults[JB_KIT_BYTES]; jb_init(); jb_defaults(defaults);
 memset(activeParameters,23,sizeof(activeParameters));
 for(unsigned v=0;v<6;v++) { jb_parameters[v][JB_MODE]=v%4; jb_parameters[v][JB_METAL]=20+v; }
 memcpy(expected,jb_parameters,sizeof(expected));
 preset_saveDrumsetRam(3,0); jb_init(); preset_loadDrumsetRam(3,0,1); assert(!memcmp(expected,jb_parameters,sizeof(expected)));
 assert(preset_saveDrumsetSD(3,0)==FR_OK); assert(length==8+64+JB_RECORD_BYTES);
 for(unsigned i=8;i<8+64;i++) assert(disk[i]==23); // original payload remains in place
 memset(jb_kits[3],0,sizeof(jb_kits[3])); assert(preset_loadDrumsetSD(3,0)==FR_OK);
 preset_loadDrumsetRam(3,0,1); assert(!memcmp(expected,jb_parameters,sizeof(expected)));
 length=8+64; assert(preset_loadDrumsetSD(3,0)==FR_OK); preset_loadDrumsetRam(3,0,1); assert(!memcmp(defaults,jb_parameters,sizeof(defaults)));
 jb_loadKit(expected); menu_selectedPresetNr[0]=3;
 assert(preset_saveTmpKit()==FR_OK); assert(length==1+64+JB_RECORD_BYTES);
 jb_init(); assert(preset_LoadTmpKit()==FR_OK); assert(!memcmp(expected,jb_parameters,sizeof(expected)));
 disk[length-1]^=1; assert(preset_LoadTmpKit()==FR_OK); assert(!memcmp(defaults,jb_parameters,sizeof(defaults)));
 length=1+64; jb_loadKit(expected); assert(preset_LoadTmpKit()==FR_OK); assert(!memcmp(defaults,jb_parameters,sizeof(defaults)));
 disk[0]=255; assert(preset_LoadTmpKit()==FR_DISK_ERR);
 puts("PASS actual kit/RAM/autosave save+restore paths, legacy payload offsets, missing/corrupt extensions and invalid temporary slot");
}
'''
out=root/'build/host/storage_fixture.c'; out.write_text(fixture+'\n'.join(functions)+test)
cmd=[os.environ.get('CC','cc'),'-std=c11','-O1','-g','-fsanitize=address,undefined','-Itests/host','-ICore/Inc',str(out),'Core/Src/Soundengine/DSPAudio/Jimbones.c','Core/Src/Soundengine/DSPAudio/wavetable.c','Core/Src/Preset/JimbonesPreset.c','-lm','-o',str(out.with_suffix(''))]
subprocess.run(cmd,cwd=root,check=True); subprocess.run([str(out.with_suffix(''))],check=True)
