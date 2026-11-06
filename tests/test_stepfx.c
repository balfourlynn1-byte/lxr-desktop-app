#include "Soundengine/StepFX.h"
#include <assert.h>
#include <stdio.h>
static unsigned count,perBlock,lastVoice,lastVel;
static void emit(uint8_t v,uint8_t n,uint8_t vel){assert(v<=6&&n==48&&vel>=1&&vel<=127);count++;perBlock++;lastVoice=v;lastVel=vel;}
static void ticks(unsigned n){for(unsigned i=0;i<n;i++){perBlock=0;stepfx_process(64,120,0,emit);assert(perBlock<=6);}}
int main(void){
 stepfx_init();stepfx_live(1,48,127);ticks(1000);assert(count==0&&stepfx_pending()==0);
 stepfx_init();stepfx_select(1);stepfx_live(1,48,100);ticks(172);assert(count==0);ticks(1);assert(count==1&&lastVoice==1&&lastVel==100);ticks(600);assert(count==3&&stepfx_pending()==0);
 count=0;stepfx_select(15);for(unsigned v=0;v<7;v++)stepfx_live(v,48,127);assert(stepfx_pending()==5);stepfx_include(0,1);stepfx_live(0,48,127);assert(stepfx_pending()==6);stepfx_include(1,0);assert(stepfx_pending()==5);ticks(100);stepfx_select(0);unsigned old=count;ticks(1000);assert(count==old&&stepfx_pending()==0);
 stepfx_select(4);stepfx_live(6,48,127);stepfx_live(5,48,100);assert(stepfx_pending()==1);ticks(24);assert(lastVoice==5&&lastVel==100);stepfx_stop();assert(!stepfx_pending());
 stepfx_select(8);stepfx_probability(-100);stepfx_live(2,48,127);old=count;ticks(2000);assert(count==old);
 for(unsigned s=1;s<16;s++){stepfx_init();stepfx_select(s);stepfx_live(1,48,127);count=0;ticks(24000);assert(!stepfx_pending());assert(count>0&&count<=512);}
 stepfx_init();stepfx_process(64,120,1,emit);stepfx_select(2);stepfx_live(1,48,127);assert(!stepfx_pending());stepfx_clock();stepfx_live(1,48,127);assert(stepfx_pending());count=0;
 for(unsigned i=0;i<7;i++){stepfx_clock();stepfx_process(64,120,1,emit);}assert(count>=1);stepfx_process(44140,120,1,emit);assert(!stepfx_pending());
 puts("PASS bypass, tempo grid, bounded six lanes, all 15 effects drain, masks, hat replacement, velocity, probability, immediate off, stop and external clock loss");
}
