/* JB-20 deterministic host runtime/tempo regression.
 * Uses a small mock of the REAL delay setter API; it does not claim
 * physical audio quality or live DSP resource validation.
 */
#include "Soundengine/DubDelaySync.h"
#include "Soundengine/StepFxSend.h"
#include "Soundengine/DSPAudio/FX/Delay.h"
#include <assert.h>
#include <stdio.h>

static unsigned ranges,times,feedbacks;
void Delay_setRange(Delay*d,float r){d->range=r;ranges++;}
void Delay_setDelayTime(Delay*d,float t){d->delayTime=t;times++;}
void Delay_setFeedback(Delay*d,float f){d->feedback=f;feedbacks++;}
static void reset(Delay*d){
    stepfx_send_init();dub_delay_init();
    *d=(Delay){0};
    d->delayType=DELAY_NORMAL;
    d->write=15000u;d->oldRead=18000u;d->newRead=24000u;
    ranges=times=feedbacks=0u;
}
static void tick(Delay*d,uint16_t bpm,uint8_t ext,uint8_t fresh,uint32_t frames) {
    dub_delay_prepare(d,1u,bpm,ext,fresh,0.25f,0.5f,0.91f,frames);
}
int main(void){
    Delay d;
    uint32_t n=0;float t=-1.f;
    reset(&d);
    assert(dub_delay_division()==1u);
    assert(dub_delay_plan(120,0,1,&n,&t)==DUB_DELAY_READY);
    assert(n==11035u && t>=0.f && t<=1.f);
    assert(dub_delay_plan(120,1,1,&n,&t)==DUB_DELAY_READY);
    assert(n==11035u);
    assert(dub_delay_plan(120,1,2,&n,&t)==DUB_DELAY_TOO_LONG);
    assert(dub_delay_plan(120,1,3,&n,&t)==DUB_DELAY_TOO_LONG);
    assert(dub_delay_plan(120,1,4,&n,&t)==DUB_DELAY_READY);
    assert(dub_delay_plan(60,0,2,&n,&t)==DUB_DELAY_TOO_LONG);
    assert(dub_delay_plan(400,0,0,&n,&t)==DUB_DELAY_READY);
    assert(dub_delay_plan(120,0,9,&n,&t)==DUB_DELAY_NO_CLOCK);
    assert(dub_delay_plan(0,0,1,&n,&t)==DUB_DELAY_NO_CLOCK);

    /* At 60..180 BPM, anything outside the ring is explicitly rejected. */
    const uint16_t bpms[]={60,80,100,120,140,180};
    for(unsigned i=0;i<6;i++)for(unsigned m=0;m<2;m++)
      for(unsigned sub=0;sub<5;sub++){
        uint8_t s=dub_delay_plan(bpms[i],(uint8_t)m,(uint8_t)sub,&n,&t);
        if(s==DUB_DELAY_READY){
            assert(n>=883u);
            assert(n <= (m?15728u:32767u));
            assert(t>=0.f && t<=1.f);
        }else assert(s==DUB_DELAY_TOO_LONG || s==DUB_DELAY_TOO_SHORT);
      }

    /* OFF by default -> untouched manual delay and no sends. */
    tick(&d,120,0,1,64);assert(dub_delay_status()==DUB_DELAY_OFF);
    assert(!dub_delay_can_send() && !dub_delay_wet_only());
    assert(ranges==0 && times==0);

    stepfx_send_set_enabled(1u);
    tick(&d,120,0,1,64);
    assert(dub_delay_status()==DUB_DELAY_READY);
    assert(dub_delay_can_send() && dub_delay_wet_only());
    assert(d.range==1.f && d.delayTime>=0.f && d.delayTime<=1.f);
    assert(d.feedback==0.82f && ranges>=1 && times>=1);
    const unsigned old_time_count=times;
    tick(&d,121,0,1,64); /* 1 BPM jitter not a musical retime */
    assert(times==old_time_count);
    tick(&d,130,0,1,11035u); /* bounded update rate */
    assert(times>old_time_count);

    /* Manual FX-knob changes are reasserted with synced time on next block. */
    d.range=0.3f;d.delayTime=0.0f;
    dub_delay_mark_dirty();
    tick(&d,130,0,1,64);
    assert(d.range==1.f && d.delayTime>0.f);

    /* Mode switch: old read pointers are normalised into ping-pong half. */
    d.delayType=DELAY_PINGPONG;
    d.write=20000u;d.oldRead=19000u;d.newRead=20000u;
    d.interpValue=0.5f;d.timePending=1u;
    tick(&d,130,0,1,64);
    assert(d.write<=16383u && d.oldRead<=16383u && d.newRead<=16383u);
    assert(d.timePending==0u);
    assert(dub_delay_status()==DUB_DELAY_READY);

    /* Select unsupported division -> no new sends and retains old tail. */
    dub_delay_adjust_division(1);
    assert(dub_delay_division()==2);
    tick(&d,120,0,1,64);
    assert(dub_delay_status()==DUB_DELAY_TOO_LONG);
    assert(!dub_delay_can_send() && dub_delay_wet_only());
    dub_delay_adjust_division(-1);
    tick(&d,120,1,0,64); /* stale external clock blocks fresh throws */
    assert(dub_delay_status()==DUB_DELAY_NO_CLOCK);
    assert(!dub_delay_can_send());
    tick(&d,120,1,1,64);
    assert(dub_delay_status()==DUB_DELAY_READY && dub_delay_can_send());

    /* OFF closes new sends but retains a 4s wet-only echo tail.
     * The existing buffer is never reset; kit params restored afterward. */
    stepfx_send_set_enabled(0u);
    tick(&d,120,0,1,64);
    assert(!dub_delay_can_send() && dub_delay_wet_only());
    tick(&d,120,0,1,44140u*4u);
    assert(dub_delay_status()==DUB_DELAY_OFF);
    assert(!dub_delay_wet_only());
    assert(d.range==0.5f && d.delayTime==0.25f && d.feedback==0.91f);

    /* FX type missing suppresses send without silently changing FX type. */
    reset(&d);stepfx_send_set_enabled(1);
    dub_delay_prepare(&d,0,120,0,1,0.25f,0.5f,0.7f,64);
    assert(dub_delay_status()==DUB_DELAY_NEEDS_FX);
    assert(!dub_delay_can_send() && !dub_delay_wet_only());

    puts("PASS JB-20: timed subdivision bounds, mono/pingpong, hysteresis, parameter restore, external clock loss, feedback cap, tail lifecycle, missing FX");
    return 0;
}
