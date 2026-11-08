/* JB-24: run production UI mapping without hardware dependencies. */
#include "menu/JimbonesWaveUI.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

int main(void){
 const uint8_t forbidden[]={
     JB_METAL_LEVEL,JB_METAL,JB_VMETAL,JB_RATIO,JB_DEPTH,JB_FEEDBACK
 };
 uint8_t seen[JB_PARAMS]={0};
 assert(JB_WAVEUI_VISIBLE==26u);
 assert(JB_WAVEUI_BANKS==7u);
 assert(JB_WAVEUI_INTERP==26u);
 for(unsigned i=0;i<JB_WAVEUI_VISIBLE;i++){
    const uint8_t p=jb_waveui_param((uint8_t)i);
    assert(p<JB_PARAMS);
    assert(!seen[p]);
    seen[p]=1u;
    assert(strlen(jb_waveui_short((uint8_t)i))==3u);
    assert(strlen(jb_waveui_long((uint8_t)i))>0u);
    for(unsigned j=0;j<sizeof forbidden;j++)assert(p!=forbidden[j]);
    assert(jb_waveui_col((uint8_t)(i/4u),(uint8_t)(i%4u))==i);
 }
 for(unsigned j=0;j<sizeof forbidden;j++)assert(!seen[forbidden[j]]);
 assert(jb_waveui_param(JB_WAVEUI_INTERP)==JB_PARAMS);
 assert(jb_waveui_col(6u,2u)==JB_WAVEUI_INTERP);
 assert(jb_waveui_col(6u,3u)==JB_WAVEUI_INTERP);
 assert(jb_waveui_col(7u,0u)==JB_WAVEUI_INTERP);
 assert(jb_waveui_col(0u,4u)==JB_WAVEUI_INTERP);
 assert(jb_waveui_param(0u)==JB_MODE);
 assert(jb_waveui_param(1u)==JB_WAVE_LEVEL);
 assert(jb_waveui_param(2u)==JB_POSITION);
 assert(jb_waveui_param(3u)==JB_MOVEMENT);
 assert(jb_waveui_param(4u)==JB_SHAPE);
 assert(jb_waveui_param(5u)==JB_CHARACTER);

 uint8_t index=0u;
 for(unsigned i=0;i<JB_WAVEUI_INTERP;i++)
    index=jb_waveui_next(index,1);
 assert(index==JB_WAVEUI_INTERP);
 assert(jb_waveui_next(index,1)==0u);
 assert(jb_waveui_next(0u,-1)==JB_WAVEUI_INTERP);
 assert(jb_waveui_next(10u,0)==10u);
 assert(jb_waveui_next(250u,0)==0u);

 assert(strcmp(jb_waveui_shape_name(0u,1u),"SINE")==0);
 assert(strcmp(jb_waveui_shape_name(42u,1u),"SINE>TRI")==0);
 assert(strcmp(jb_waveui_shape_name(84u,1u),"TRI>SAW")==0);
 assert(strcmp(jb_waveui_shape_name(126u,1u),"SAW>PULSE")==0);
 assert(strcmp(jb_waveui_shape_name(127u,1u),"PULSE")==0);
 assert(strcmp(jb_waveui_shape_name(42u,0u),"TRI")==0);
 assert(strcmp(jb_waveui_shape_name(127u,0u),"PULSE")==0);
 puts("PASS JB-24: 26 wave-only controls, hidden legacy FM fields, 7 banks + interpolation, named shapes, navigation");
 return 0;
}
