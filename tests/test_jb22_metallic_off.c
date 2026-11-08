/* JB-22: legacy FM-only mode must bypass original audio bit-for-bit;
 * mode 3 must sound exactly like wavetable-only mode 2 with FM compiled out.
 * Run production wave synthesis, not a mock.
 */
#include "Soundengine/DSPAudio/Jimbones.h"
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

static void seeded_note(unsigned mode,int16_t*out,unsigned n){
    jb_init();
    jb_parameters[2][JB_MODE]=(uint8_t)mode;
    jb_parameters[2][JB_DECAY]=96u;
    jb_parameters[2][JB_WAVE_LEVEL]=105u;
    jb_parameters[2][JB_METAL_LEVEL]=127u; /* should not react */
    jb_parameters[2][JB_POSITION]=75u;
    jb_parameters[2][JB_MOVEMENT]=40u;
    jb_parameters[2][JB_TEXTURE]=23u;
    jb_parameters[2][JB_AGGRESSION]=12u;
    jb_parameters[2][JB_BLEND]=127u;
    jb_trigger(2u,115u,48u,0u);
    for(unsigned i=0;i<n;i++)out[i]=(int16_t)(i*17u-2000);
    jb_render(2u,out,n,1.f);
}
int main(void){
#if JB_METALLIC_ENABLED
#error JB-22 must not include metallic DSP in candidate firmware.
#endif
    int16_t orig[512],fm[512],wave[512],oldboth[512];
    for(unsigned i=0;i<512u;i++)orig[i]=(int16_t)(i*17u-2000);
    seeded_note(1u,fm,512u);
    assert(memcmp(orig,fm,sizeof orig)==0);
    seeded_note(2u,wave,512u);
    seeded_note(3u,oldboth,512u);
    assert(memcmp(wave,oldboth,sizeof wave)==0);
    assert(memcmp(orig,wave,sizeof wave)!=0);
    /* FM-only mode may not inject noise or attenuate native fader volume. */
    jb_init();jb_parameters[2][JB_MODE]=1u;
    jb_parameters[2][JB_NOISE_LEVEL]=127u;
    jb_parameters[2][JB_WAVE_LEVEL]=0u;
    jb_trigger(2u,127u,48u,0u);
    int16_t dry[64],ref[64];
    for(unsigned i=0;i<64u;i++)dry[i]=ref[i]=(int16_t)(i*150u-4000);
    jb_preFilterNoise(2u,dry,64u);
    jb_stageVoice(2u,dry,64u,.6f);
    assert(memcmp(dry,ref,sizeof dry)==0);
    puts("PASS JB-22: Metallic FM compiled out, old FM-only kit bypass, old combo kit matches wavetable only, dry and pre-filter isolation");
    return 0;
}
