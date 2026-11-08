/* Host regression for per-voice dub send and click-free throw ramps.
 * This does not prove the native delay's audible tails or hardware timing. */
#include "Soundengine/StepFX.h"
#include <assert.h>
#include <stdint.h>
#include <string.h>
int main(void) {
 int16_t src[64],bus[128];
 for(unsigned i=0;i<64;i++)src[i]=16000;
 memset(bus,0,sizeof(bus));
 stepfx_init();
 assert(stepfx_dubEnabled()==0);
 assert(stepfx_dubSendMask()==0x3eu); /* kick excluded */
 stepfx_dubTap(1,src,bus,64);
 for(unsigned i=0;i<128;i++)assert(bus[i]==0);
 stepfx_dubThrow(1,1);
 assert(stepfx_dubEffectiveMask()==0);
 stepfx_dubEnable(1);
 assert(stepfx_dubEffectiveMask()==2);
 stepfx_dubTap(1,src,bus,64);
 assert(bus[0]==62 && bus[1]==62);
 assert(bus[126]==4000 && bus[127]==4000);
 for(unsigned i=1;i<64;i++)assert(bus[2*i]>=bus[2*(i-1)]);
 /* Disabling the throw ramps down new sends, not a hard cut. */
 stepfx_dubThrow(1,0);
 assert(stepfx_dubEffectiveMask()==0);
 memset(bus,0,sizeof(bus));
 stepfx_dubTap(1,src,bus,64);
 assert(bus[0]>0 && bus[0]<4000);
 assert(bus[126]==0 && bus[127]==0);
 for(unsigned i=1;i<64;i++)assert(bus[2*i]<=bus[2*(i-1)]);
 memset(bus,0,sizeof(bus));
 stepfx_dubTap(1,src,bus,64);
 for(unsigned i=0;i<128;i++)assert(bus[i]==0);
 stepfx_dubThrow(0,1);
 assert((stepfx_dubEffectiveMask()&1u)==0u); /* kick protected */
 stepfx_dubThrow(7,1);
 assert(stepfx_dubThrowMask()==1u);
 stepfx_dubEnable(0);
 assert(stepfx_dubEffectiveMask()==0);
 /* Two loud voice sends must saturate the FX bus, never wrap. */
 stepfx_init();
 stepfx_dubEnable(1);
 stepfx_dubThrow(1,1);
 stepfx_dubThrow(2,1);
 for(unsigned i=0;i<64;i++)src[i]=32767;
 memset(bus,0,sizeof(bus));
 stepfx_dubTap(1,src,bus,64); /* fade reaches full send */
 stepfx_dubTap(2,src,bus,64);
 assert(bus[126]==16382 && bus[127]==16382);
 for(unsigned i=0;i<128;i++)assert(bus[i]>=0);
 /* A pre-existing loud FX input must clamp rather than wrap negative. */
 for(unsigned i=0;i<128;i++)bus[i]=30000;
 stepfx_dubTap(1,src,bus,64);
 stepfx_dubTap(2,src,bus,64);
 for(unsigned i=0;i<128;i++)assert(bus[i]==32767);
 /* Kick remains excluded even when thrown unless explicitly included. */
 stepfx_dubThrow(0,1);
 memset(bus,0,sizeof(bus));
 stepfx_dubTap(0,src,bus,64);
 for(unsigned i=0;i<128;i++)assert(bus[i]==0);
 stepfx_dubVoice(0,1);
 stepfx_dubTap(0,src,bus,64);
 assert(bus[126]>0);
 stepfx_init();
 assert(stepfx_dubEnabled()==0 && stepfx_dubThrowMask()==0);
 return 0;
}
