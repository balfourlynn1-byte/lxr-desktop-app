/* SPDX-License-Identifier: GPL-3.0-only
 * JB-20: lightweight (block-rate) timing adapter for existing FX_DELAY.
 *
 * The original delay engine remains the only feedback buffer. No delay
 * allocation, new voice render, or per-sample timing calculation.
 */
#include "Soundengine/DubDelaySync.h"
#include "Soundengine/StepFxSend.h"
#include "Soundengine/DSPAudio/FX/Delay.h"
#include <stdint.h>

#define DUB_FS 44140u
#define DUB_TAIL_FRAMES (DUB_FS * 4u)
#define DUB_MIN_SAMPLES 883u /* original HI_MIN = 0.02s at 44140Hz */
#define DUB_MAX_MONO ((uint32_t)FX_BUFFER_SIZE)
/* Original Delay.c high-range pingpong interpolation ends at HI_MAX*0.48f. */
#define DUB_MAX_PINGPONG ((uint32_t)(FX_BUFFER_SIZE * 48u / 100u))
#define DUB_COOLDOWN_FRAMES (DUB_FS / 4u)

static const char *const names[5] = {"1/16","1/8","1/4","D8","T8"};
static uint8_t requested_division=1u; /* default eighth-note echo */
static uint8_t status=DUB_DELAY_OFF;
static uint8_t may_send, wet_only, last_mode=255u, was_ready, dirty;
static uint32_t tail_frames, last_samples, cooldown_frames;
static float last_feedback;

void dub_delay_init(void) {
    __atomic_store_n(&requested_division,1u,__ATOMIC_RELAXED);
    __atomic_store_n(&status,DUB_DELAY_OFF,__ATOMIC_RELAXED);
    __atomic_store_n(&may_send,0u,__ATOMIC_RELAXED);
    __atomic_store_n(&wet_only,0u,__ATOMIC_RELAXED);
    __atomic_store_n(&dirty,0u,__ATOMIC_RELAXED);
    last_mode=255u;
    was_ready=0u;
    last_samples=0u;
    tail_frames=0u;
    cooldown_frames=DUB_COOLDOWN_FRAMES;
    last_feedback=-1.f;
}
uint8_t dub_delay_division(void) {
    return __atomic_load_n(&requested_division,__ATOMIC_RELAXED);
}
void dub_delay_adjust_division(int delta) {
    if(!delta)return;
    const uint8_t current=dub_delay_division();
    uint8_t next=current;
    if(delta>0 && next<4u)next++;
    if(delta<0 && next>0u)next--;
    __atomic_store_n(&requested_division,next,__ATOMIC_RELAXED);
}
const char *dub_delay_division_name(void){return names[dub_delay_division()];}
uint8_t dub_delay_status(void){return __atomic_load_n(&status,__ATOMIC_RELAXED);}
const char *dub_delay_status_name(void){
    switch(dub_delay_status()){
    case DUB_DELAY_READY:return "OK";
    case DUB_DELAY_NEEDS_FX:return "NOFX";
    case DUB_DELAY_NO_CLOCK:return "CLK";
    case DUB_DELAY_TOO_LONG:return "LONG";
    case DUB_DELAY_TOO_SHORT:return "SHORT";
    default:return "OFF";
    }
}
uint8_t dub_delay_can_send(void){return __atomic_load_n(&may_send,__ATOMIC_RELAXED);}
uint8_t dub_delay_wet_only(void){return __atomic_load_n(&wet_only,__ATOMIC_RELAXED);}
void dub_delay_mark_dirty(void){__atomic_store_n(&dirty,1u,__ATOMIC_RELAXED);}

uint8_t dub_delay_plan(uint16_t bpm,uint8_t mode,uint8_t division,
                       uint32_t *samples,float *normalized) {
    static const uint8_t numerator[5]={1u,1u,1u,3u,1u};
    static const uint8_t denominator[5]={4u,2u,1u,4u,3u};
    if (bpm<10u || bpm>400u || division>=5u || mode>1u)
        return DUB_DELAY_NO_CLOCK;
    const uint64_t num=(uint64_t)DUB_FS*60u*numerator[division];
    const uint32_t den=(uint32_t)bpm*denominator[division];
    const uint32_t n=(uint32_t)((num+den/2u)/den);
    const uint32_t max_samples=mode?DUB_MAX_PINGPONG:DUB_MAX_MONO;
    if(n>DUB_MAX_MONO || n>max_samples)return DUB_DELAY_TOO_LONG;
    if(n<DUB_MIN_SAMPLES)return DUB_DELAY_TOO_SHORT;
    if(samples)*samples=n;
    if(normalized){
        const float high_min=0.02f*(float)DUB_FS;
        const float high_max=mode?(float)FX_BUFFER_SIZE*0.48f:(float)FX_BUFFER_SIZE;
        float value=((float)n-high_min)/(high_max-high_min);
        if(value<0.f)value=0.f;
        if(value>1.f)value=1.f;
        *normalized=value;
    }
    return DUB_DELAY_READY;
}

static float safe_feedback(float value){
    if(!(value>=0.f))return 0.f;
    return value>0.82f?0.82f:value;
}
static void restore_manual(Delay *d,float time,float range,float feedback){
    Delay_setRange(d,range);
    Delay_setDelayTime(d,time);
    Delay_setFeedback(d,feedback);
}

void dub_delay_prepare(struct DelayStruct *delay, uint8_t fx_is_delay,
                       uint16_t bpm, uint8_t external_clock,
                       uint8_t clock_fresh,
                       float manual_time, float manual_range,
                       float manual_feedback, uint32_t frames) {
    Delay *d=(Delay*)delay;
    if(!d)return;
    uint8_t next=DUB_DELAY_OFF;
    uint32_t samples=0u;
    float normalized=0.f;
    const uint8_t requested=stepfx_send_is_enabled();
    const uint8_t mode=(uint8_t)(d->delayType==DELAY_PINGPONG);
    const uint8_t mode_changed=(uint8_t)(last_mode!=255u && last_mode!=mode);
    if(requested){
        if(!fx_is_delay)next=DUB_DELAY_NEEDS_FX;
        else if(external_clock && !clock_fresh)next=DUB_DELAY_NO_CLOCK;
        else next=dub_delay_plan(bpm,mode,dub_delay_division(),
                                 &samples,&normalized);
    }

    /* Safety when changing mono vs ping-pong while a throw is active:
     * old pointers can exceed half-buffer bounds. Preserve buffer contents.
     * A mode switch during a running echo may still audibly crossfade. */
    if(fx_is_delay && (was_ready || next==DUB_DELAY_READY) &&
       mode_changed){
        const uint32_t mask=mode?(FX_BUFFER_SIZE>>1u):FX_BUFFER_SIZE;
        d->write&=mask;d->oldRead&=mask;d->newRead&=mask;
        d->interpValue=0.f;
        d->timePending=0u;
        cooldown_frames=DUB_COOLDOWN_FRAMES;
    }
    last_mode=mode;

    if(next==DUB_DELAY_READY){
        uint8_t reapply=__atomic_exchange_n(&dirty,0u,__ATOMIC_ACQ_REL);
        if(cooldown_frames<DUB_COOLDOWN_FRAMES) {
            const uint32_t room=DUB_COOLDOWN_FRAMES-cooldown_frames;
            cooldown_frames+=(frames>room?room:frames);
        }
        /* External tempo jitter must not launch a new 10ms crossfade every
         * sample block. A +/-1 BPM change is ignored until larger movement,
         * and retimes are throttled to at most 4 Hz. */
        uint32_t diff=samples>last_samples?samples-last_samples:last_samples-samples;
        const uint8_t changed=(uint8_t)(diff>=(DUB_FS/120u));
        if(!was_ready || reapply || mode_changed ||
           (changed && cooldown_frames>=DUB_COOLDOWN_FRAMES)){
            if(d->range!=1.f)Delay_setRange(d,1.f);
            Delay_setDelayTime(d,normalized);
            last_samples=samples;
            cooldown_frames=0u;
        }
        /* The kit's feedback remains user-controlled but cannot self-run at
         * 100% or higher during dub throws. */
        const float feedback=safe_feedback(manual_feedback);
        if(feedback!=last_feedback || d->feedback!=feedback){
            Delay_setFeedback(d,feedback);
            last_feedback=feedback;
        }
        was_ready=1u;
        tail_frames=0u;
        __atomic_store_n(&may_send,1u,__ATOMIC_RELAXED);
        __atomic_store_n(&wet_only,1u,__ATOMIC_RELAXED);
    }else{
        __atomic_store_n(&may_send,0u,__ATOMIC_RELAXED);
        if(was_ready){was_ready=0u;tail_frames=DUB_TAIL_FRAMES;}
        if(!fx_is_delay)tail_frames=0u;
        if(tail_frames && fx_is_delay){
            tail_frames=(tail_frames>frames)?tail_frames-frames:0u;
            __atomic_store_n(&wet_only,1u,__ATOMIC_RELAXED);
        }else{
            if(last_samples) {
                restore_manual(d,manual_time,manual_range,manual_feedback);
                last_samples=0u;last_feedback=-1.f;
                cooldown_frames=DUB_COOLDOWN_FRAMES;
                __atomic_store_n(&dirty,0u,__ATOMIC_RELAXED);
            }
            __atomic_store_n(&wet_only,0u,__ATOMIC_RELAXED);
        }
    }
    __atomic_store_n(&status,next,__ATOMIC_RELAXED);
}
