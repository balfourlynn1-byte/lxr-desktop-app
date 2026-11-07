/* Deterministic MIDI-through-production-DSP comparison. GPL-3.0-only. */
#include "lxr_api.h"
#include "Parameters.h"
#include "buttonHandler.h"
#include "Soundengine/MIDI/MidiParser.h"
#include "Soundengine/MIDI/Uart.h"
#include "menu/menu.h"
#ifdef TEST_STEPFX
#include "Soundengine/StepFX.h"
#include "menu/JimbonesPerfUI.h"
#endif
#ifdef TEST_METAL
#include "Soundengine/DSPAudio/Jimbones.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#undef NDEBUG
#include <assert.h>
#include <string.h>
#include <time.h>
static void click(int b){lxr_button_click(b);lxr_idle_blocks(8);}
int main(int argc,char **argv){
 assert(argc==4 || argc>=7);int mode=atoi(argv[2]);assert(lxr_init(123,128)==0);lxr_set_jacks(3);lxr_idle_blocks(1600);
 if(!strcmp(argv[1],"ui")){
#ifdef TEST_STEPFX
  click(BUT_MODE2);assert(jb_perfui_page()==0);click(BUT_MODE2);assert(jb_perfui_page()==1);click(BUT_MODE2);assert(menu_stepFxPageActive());
  const int buttons[]={BUT_SEQ1,BUT_SEQ2,BUT_SEQ3,BUT_SEQ4,BUT_SEQ5,BUT_SEQ6,BUT_SEQ7,BUT_SEQ8,BUT_SEQ9,BUT_SEQ10,BUT_SEQ11,BUT_SEQ12,BUT_SEQ13,BUT_SEQ14,BUT_SEQ15,BUT_SEQ16};
  for(int i=0;i<16;i++){click(buttons[i]);assert(stepfx_slot()==i);printf("slot %d LCD [%.16s][%.16s]\n",i+1,lxr_get_lcd(),lxr_get_lcd()+16);}
  click(BUT_SEQ1);assert(!stepfx_pending());click(BUT_MODE2);assert(jb_perfui_cpu_active());click(BUT_SEQ2);click(BUT_MODE2);assert(jb_perfui_page()==0);click(BUT_MODE1);assert(!menu_stepFxPageActive());
#ifdef JB_MIXED_KIT
  const int controls[]={1,2,20,24,28,29,30,31};
  const char *labels[]={"FML","WTL","Drv","Pos","Mix","Shp","FxA","vMx"};
  for(int voice=0;voice<7;voice++) {
   click(BUT_VOICE_1+voice); click(BUT_SELECT2); click(BUT_SELECT1);
   for(int bank=0;bank<8;bank++) {
    click(BUT_SELECT1);
    for(unsigned k=0;k<8;k++) if(controls[k]/4==bank) {
     int par=controls[k],v=voice>5?5:voice;
     const char *lcd=lxr_get_lcd();
     assert(!memcmp(lcd+(par%4)*4,labels[k],3));
     uint8_t saved[JB_VOICES];for(int i=0;i<6;i++) saved[i]=jb_parameters[i][par];
     int dir=saved[v]>63?-1:1;
     menu_parseKnobValue(par%4,dir);
     for(int i=0;i<6;i++) assert(jb_parameters[i][par]==saved[i]+(i==v?dir:0));
    }
   }
  }
#endif
#ifdef JB_MIXED_KIT
  click(BUT_VOICE_4);click(BUT_SELECT2);click(BUT_SELECT1);click(BUT_SELECT1);
  lxr_encoder_turn(1);lxr_idle_blocks(32);
  lxr_encoder_button(1);lxr_idle_blocks(32);lxr_encoder_button(0);lxr_idle_blocks(32);
  int old=jb_parameters[3][JB_METAL_LEVEL];
  lxr_encoder_turn(1);lxr_idle_blocks(32);
  assert(jb_parameters[3][JB_METAL_LEVEL]!=old);
  click(BUT_MODE2);click(BUT_MODE2);click(BUT_MODE2);click(BUT_MODE2);
  assert(jb_perfui_cpu_active());menu_parseEncoder(-1,0);
  menu_parseKnobValue(0,1);assert(jb_liveBalance);
  menu_parseKnobValue(0,-1);assert(!jb_liveBalance);
#endif
  puts("PASS actual button-handler page cycle and all 16 selector buttons");
#endif
 return 0;
 }
 for(int v=0;v<7;v++){midi_MidiChannels[v]=v;lxr_set_param(PAR_MIDI_NOTE1+v,0);}midi_MidiChannels[7]=15;
 lxr_set_param(PAR_VELOD1,115);lxr_set_param(PAR_VELOD2,115);lxr_set_param(PAR_VELOD3,115);
 /* Fixed moderate faders retain mix headroom; max-level stress is separate. */
 float fader=getenv("JB_FADER")?atof(getenv("JB_FADER")):(mode==4?1:.4f);
 assert(fader>=0 && fader<=1);
 for(int v=0;v<6;v++)lxr_set_fader(v,fader);
#ifdef JB_MIXED_KIT
 jb_liveBalance=getenv("JB_LIVE") && atoi(getenv("JB_LIVE"));
#endif
#ifdef TEST_METAL
 int count=argc>=7?atoi(argv[4]):6;
 assert(count>=0 && count<=6);
 unsigned mask=getenv("JB_MASK")?(unsigned)atoi(getenv("JB_MASK")):(1u<<count)-1;
 for(int v=0;v<6;v++)if(mask&(1u<<v)){jb_parameters[v][JB_MODE]=mode==4?3:mode;jb_parameters[v][JB_DECAY]=127;
 if(argc>=7 && atoi(argv[6])) {jb_parameters[v][JB_CHARACTER]=110; jb_parameters[v][JB_AGGRESSION]=78;}
}
#endif
#ifdef TEST_STEPFX
 int slot=argc>=7?atoi(argv[5]):(mode?15:0); assert(slot>=0 && slot<=15);
 if(slot) {stepfx_select(slot);for(int v=0;v<6;v++)stepfx_include(v,!getenv("JB_FX_MASK") || (atoi(getenv("JB_FX_MASK"))&(1<<v)));}
#endif
#ifdef JB_EXTENDED_CONTROLS
 /* Exercise the actual external MIDI entry point used by controllers. */
 for(int arg=7;arg<argc;arg++) {
  int param,value; assert(sscanf(argv[arg],"%d=%d",&param,&value)==2);
  assert(param>=0 && param<JB_PARAMS && value>=0 && value<=jb_limit(param));
  for(int v=0;v<6;v++)if(mask&(1u<<v)){
   uint8_t messages[]={0xb0+v,99,96+v,0xb0+v,98,param,0xb0+v,6,value};
   lxr_midi_in(messages,sizeof(messages));uart_processMidi();
   assert(jb_parameters[v][param]==value);
  }
 }
#endif
 lxr_idle_blocks(16);FILE *events=fopen(argv[1],"r"),*out=fopen(argv[3],"wb");assert(events&&out);
 unsigned at=0,status=0,note=0,vel=0;int have=fscanf(events,"%u %u %u %u",&at,&status,&note,&vel)==4;
 unsigned total=44140*12,rendered=0;clock_t start=clock();
 struct timespec before,after; double peakUs=0,sumUs=0; unsigned blocks=0;
 while(rendered<total){
  while(have&&at==rendered){uint8_t msg[]={status,note,vel};lxr_midi_in(msg,3);have=fscanf(events,"%u %u %u %u",&at,&status,&note,&vel)==4;}
  unsigned n=total-rendered;if(n>64)n=64;if(have&&at>rendered&&at-rendered<n)n=at-rendered;
  int16_t samples[128];clock_gettime(CLOCK_MONOTONIC,&before);assert(lxr_render_i16(samples,n,2)==(int)n);
  clock_gettime(CLOCK_MONOTONIC,&after);
  if(n==64) {
   double us=(after.tv_sec-before.tv_sec)*1e6+(after.tv_nsec-before.tv_nsec)/1e3;
   if(us>peakUs)peakUs=us;
   sumUs+=us; blocks++;
  }
  fwrite(samples,2,n*2,out);rendered+=n;
 }
 printf("Host 64-frame calls %u mean_us %.3f peak_us %.3f\n",blocks,sumUs/blocks,peakUs);
 fclose(out);fclose(events);printf("Rendered %u frames, desktop CPU %.3fs (not ARM timing)\n",rendered,(double)(clock()-start)/CLOCKS_PER_SEC);return 0;
}
