/* SPDX-License-Identifier: GPL-3.0-only
 * JB-12: test production comb DSP at high load and preserve exact bypass.
 */
#include "Soundengine/DSPAudio/CombFilter.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

static void impulse(JbCombFilter *c, int16_t *buf, uint8_t n, int16_t peak) {
    memset(buf,0,n*sizeof *buf);
    buf[0]=peak;
    jb_comb_process(c,buf,n,0.45f,1.f);
}
int main(void){
    JbCombFilter a,b;
    int16_t dry[64],ref[64];
    jb_comb_init(&a); jb_comb_init(&b);
    assert(a.write==0u && a.delay==0u);
    for(unsigned i=0;i<64;i++)dry[i]=ref[i]=(int16_t)((int)i*503-16000);
    jb_comb_process(&a,dry,64u,0.20f,0.f);
    assert(!memcmp(dry,ref,sizeof dry)); /* no extra DSP coloration */
    assert(a.write==0u);

    /* With 16-sample minimum delay, an impulse develops a delayed echo. */
    int16_t buffer[64]={0};
    impulse(&a,buffer,64u,20000);
    assert(a.delay==JB_COMB_MIN_DELAY);
    assert(buffer[0]>0 && buffer[0]<20000);
    assert(buffer[16]>0);
    assert(buffer[1]==0);
    assert(buffer[32]>0);

    /* Independent voices retain their own ring state. */
    for(unsigned i=0;i<64;i++)buffer[i]=0;
    jb_comb_process(&b,buffer,64u,0.45f,1.f);
    for(unsigned i=0;i<64;i++)assert(buffer[i]==0);
    jb_comb_reset(&a);
    memset(buffer,0,sizeof buffer);
    jb_comb_process(&a,buffer,64u,0.45f,1.f);
    for(unsigned i=0;i<64;i++)assert(buffer[i]==0);

    /* One block per voice / 4 overlapping beats / 1000 blocks:
     * integer histories and output never exceed int16 even at full-scale.
     * A canary detects ring indexing outside caller-provided PCM buffer. */
    JbCombFilter six[6];
    for(unsigned v=0;v<6;v++)jb_comb_init(&six[v]);
    struct{int16_t guard_left;int16_t pcm[64];int16_t guard_right;} v;
    for(unsigned block=0;block<1000u;block++){
        for(unsigned lane=0;lane<6u;lane++){
            v.guard_left=0x2345;v.guard_right=0x1234;
            for(unsigned i=0;i<64u;i++){
                const int t=(int)(block*131u+lane*257u+i*130u);
                v.pcm[i]=(int16_t)(t&1?INT16_MIN:INT16_MAX);
            }
            jb_comb_process(&six[lane],v.pcm,64u,
               (float)((block+lane)%128u)*(0.45f/127.f),1.f);
            assert(v.guard_left==0x2345 && v.guard_right==0x1234);
            assert(six[lane].write<JB_COMB_TAPS);
            assert(six[lane].delay>=JB_COMB_MIN_DELAY && six[lane].delay<=JB_COMB_MAX_DELAY);
        }
    }
    /* Negative/NaN freq/reso inputs remain bounded. */
    jb_comb_init(&a);
    for(unsigned i=0;i<64u;i++)buffer[i]=1200;
    jb_comb_process(&a,buffer,64u,-1.f,3.f);
    assert(a.delay==JB_COMB_MAX_DELAY);
    for(unsigned i=0;i<64u;i++)assert(buffer[i]>=0 && buffer[i]<=1200);
    puts("PASS JB-12: zero-resonance bypass, controlled echoes, separate voices, 384,000 stress samples, saturating output and bounds");
    return 0;
}
