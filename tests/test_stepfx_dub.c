/* Host regression for Step FX dub send controller; does not claim audio routing. */
#include "Soundengine/StepFX.h"
#include <assert.h>
int main(void) {
 stepfx_init();
 int16_t src[4]={4000,-4000,32767,-32768};
 int16_t bus[8]={0};
 stepfx_dubTap(1,src,bus,4);
 for(unsigned i=0;i<8;i++)assert(bus[i]==0); /* default OFF */
 assert(stepfx_dubEnabled()==0);
 assert(stepfx_dubSendMask()==0x3eu); /* kick excluded */
 assert(stepfx_dubEffectiveMask()==0);
 stepfx_dubThrow(1,1);
 assert(stepfx_dubEffectiveMask()==0);
 stepfx_dubEnable(1);
 assert(stepfx_dubEffectiveMask()==2);
 stepfx_dubTap(1,src,bus,4);
 assert(bus[0]==1000 && bus[1]==1000);
 assert(bus[2]==-1000 && bus[3]==-1000);
 assert(bus[4]==8191 && bus[5]==8191);
 assert(bus[6]==-8192 && bus[7]==-8192);
 stepfx_dubVoice(1,0);
 assert(stepfx_dubEffectiveMask()==0);
 stepfx_dubTap(1,src,bus,4);
 assert(bus[0]==1000); /* no input after throw-off */
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
