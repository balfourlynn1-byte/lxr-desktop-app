/* Host regression for Step FX dub send controller; does not claim audio routing. */
#include "Soundengine/StepFX.h"
#include <assert.h>
int main(void) {
 stepfx_init();
 assert(stepfx_dubEnabled()==0);
 assert(stepfx_dubSendMask()==0x3eu); /* kick excluded */
 assert(stepfx_dubEffectiveMask()==0);
 stepfx_dubThrow(1,1);
 assert(stepfx_dubEffectiveMask()==0);
 stepfx_dubEnable(1);
 assert(stepfx_dubEffectiveMask()==2);
 stepfx_dubVoice(1,0);
 assert(stepfx_dubEffectiveMask()==0);
 stepfx_dubVoice(1,1);
 assert(stepfx_dubEffectiveMask()==2);
 stepfx_dubThrow(1,0);
 assert(stepfx_dubEffectiveMask()==0);
 stepfx_dubThrow(7,1);
 assert(stepfx_dubThrowMask()==0);
 stepfx_dubEnable(0);
 assert(stepfx_dubEffectiveMask()==0);
 stepfx_init();
 assert(stepfx_dubEnabled()==0 && stepfx_dubThrowMask()==0);
 return 0;
}
