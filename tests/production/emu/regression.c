/* Deterministic MIDI-through-production-DSP comparison. GPL-3.0-only. */
#include "lxr_api.h"
#include "Parameters.h"
#include "buttonHandler.h"
#include "Soundengine/MIDI/MidiParser.h"
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
 assert(argc==4);int mode=atoi(argv[2]);assert(lxr_init(123,128)==0);lxr_set_jacks(3);lxr_idle_blocks(1600);
 if(!strcmp(argv[1],"ui")){
#ifdef TEST_STEPFX
  click(BUT_MODE2);assert(jb_perfui_page()==0);click(BUT_MODE2);assert(jb_perfui_page()==1);click(BUT_MODE2);assert(menu_stepFxPageActive());
  const int buttons[]={BUT_SEQ1,BUT_SEQ2,BUT_SEQ3,BUT_SEQ4,BUT_SEQ5,BUT_SEQ6,BUT_SEQ7,BUT_SEQ8,BUT_SEQ9,BUT_SEQ10,BUT_SEQ11,BUT_SEQ12,BUT_SEQ13,BUT_SEQ14,BUT_SEQ15,BUT_SEQ16};
  for(int i=0;i<16;i++){click(buttons[i]);assert(stepfx_slot()==i);printf("slot %d LCD [%.16s][%.16s]\n",i+1,lxr_get_lcd(),lxr_get_lcd()+16);}
  click(BUT_SEQ1);assert(!stepfx_pending());click(BUT_MODE2);assert(jb_perfui_cpu_active());click(BUT_SEQ2);click(BUT_MODE2);assert(jb_perfui_page()==0);click(BUT_MODE1);assert(!menu_stepFxPageActive());
  puts("PASS actual button-handler page cycle and all 16 selector buttons");
#endif
 return 0;
 }
 for(int v=0;v<7;v++){midi_MidiChannels[v]=v;lxr_set_param(PAR_MIDI_NOTE1+v,0);}midi_MidiChannels[7]=15;
 lxr_set_param(PAR_VELOD1,115);lxr_set_param(PAR_VELOD2,115);lxr_set_param(PAR_VELOD3,115);
 /* Fixed moderate faders retain mix headroom; max-level stress is separate. */
 for(int v=0;v<6;v++)lxr_set_fader(v,mode==4?1:.4f);
#ifdef TEST_METAL
 for(int v=0;v<6;v++){jb_parameters[v][JB_MODE]=mode==4?3:mode;jb_parameters[v][JB_DECAY]=127;}
#endif
#ifdef TEST_STEPFX
 if(mode) {stepfx_select(15);for(int v=0;v<6;v++)stepfx_include(v,1);}
#endif
 lxr_idle_blocks(16);FILE *events=fopen(argv[1],"r"),*out=fopen(argv[3],"wb");assert(events&&out);
 unsigned at=0,status=0,note=0,vel=0;int have=fscanf(events,"%u %u %u %u",&at,&status,&note,&vel)==4;
 unsigned total=44140*12,rendered=0;clock_t start=clock();
 while(rendered<total){
  while(have&&at==rendered){uint8_t msg[]={status,note,vel};lxr_midi_in(msg,3);have=fscanf(events,"%u %u %u %u",&at,&status,&note,&vel)==4;}
  unsigned n=total-rendered;if(n>64)n=64;if(have&&at>rendered&&at-rendered<n)n=at-rendered;
  int16_t samples[128];assert(lxr_render_i16(samples,n,2)==(int)n);fwrite(samples,2,n*2,out);rendered+=n;
 }
 fclose(out);fclose(events);printf("Rendered %u frames, desktop CPU %.3fs (not ARM timing)\n",rendered,(double)(clock()-start)/CLOCKS_PER_SEC);return 0;
}
