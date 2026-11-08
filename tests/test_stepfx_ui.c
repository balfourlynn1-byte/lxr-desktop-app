/* Production Step FX performance selector + scheduler integration regression. */
#include "menu/JimbonesPerfUI.h"
#include "Soundengine/StepFX.h"
#include <assert.h>
int main(void){
 stepfx_init();
 jb_perfui_enter();
 assert(jb_perfui_selected()==0u);
 assert(jb_perfui_select(3u));
 assert(jb_perfui_selected()==3u && stepfx_slot()==3u);
 assert(jb_perfui_select(3u));
 assert(jb_perfui_selected()==0u && stepfx_slot()==0u);
 assert(jb_perfui_select(7u));
 assert(jb_perfui_selected()==7u && stepfx_slot()==7u);
 assert(jb_perfui_select(0u));
 assert(jb_perfui_selected()==0u && stepfx_slot()==0u);
 assert(!jb_perfui_select(16u));
 assert(jb_perfui_selected()==0u && stepfx_slot()==0u);
 return 0;
}
