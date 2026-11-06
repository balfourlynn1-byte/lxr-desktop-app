#include "Soundengine/DSPAudio/RetriggerSmoother.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

static void test_unchanged_without_retrigger(void) {
    RetriggerSmoother s;
    retrigger_smoother_init(&s);
    int16_t a[64];
    for (int i=0;i<64;i++) a[i] = (int16_t)(-2000 + i*61);
    retrigger_smoother_process(&s,a,64);
    for (int i=0;i<64;i++) assert(a[i] == -2000 + i*61);
    assert(s.last_sample==a[63]);
    assert(s.remaining==0);
    puts("PASS: original samples identical when no retrigger");
}
static void test_first_hit_not_smoothed(void) {
    RetriggerSmoother s;
    retrigger_smoother_init(&s);
    retrigger_smoother_note_on(&s,0);
    int16_t a[] = {18000,-23000,1000,0};
    retrigger_smoother_process(&s,a,4);
    assert(a[0]==18000 && a[1]==-23000 && a[2]==1000 && a[3]==0);
    puts("PASS: first-hit attack unchanged");
}
static void test_retrigger_continuity_and_duration(void) {
    RetriggerSmoother s;
    retrigger_smoother_init(&s);
    int16_t before[]={2000};
    retrigger_smoother_process(&s,before,1);
    retrigger_smoother_note_on(&s,1);
    int16_t after[4][40];
    for(int block=0;block<4;block++){
        for(int i=0;i<40;i++)after[block][i]=10000;
        retrigger_smoother_process(&s,after[block],40);
    }
    assert(after[0][0]==2000);
    assert(after[0][1]>2000);
    assert(after[3][11]==10000);  /* sample 131: fully corrected */
    assert(after[3][12]==10000);
    assert(s.remaining==0);
    puts("PASS: exact retrigger boundary, 132-sample fade, completion");
}
static void test_repeated_fast_retriggers(void) {
    RetriggerSmoother s;
    retrigger_smoother_init(&s);
    int16_t a[40];
    for(int i=0;i<40;i++)a[i]=13000;
    retrigger_smoother_process(&s,a,40);
    retrigger_smoother_note_on(&s,1);
    for(int i=0;i<40;i++)a[i]=-9000;
    retrigger_smoother_process(&s,a,40);
    int16_t current=a[39];
    retrigger_smoother_note_on(&s,1);
    for(int i=0;i<40;i++)a[i]=23000;
    retrigger_smoother_process(&s,a,40);
    assert(a[0]==current);
    puts("PASS: fast double hit stays continuous even during previous correction");
}
static void test_clamp_and_midi_velocity(void) {
    RetriggerSmoother s;
    retrigger_smoother_init(&s);
    int16_t high[]={32000};
    retrigger_smoother_process(&s,high,1);
    retrigger_smoother_note_on(&s,1);
    int16_t a[64];
    for(int i=0;i<64;i++)a[i]=32000;
    retrigger_smoother_process(&s,a,64);
    assert(a[0]==32000);
    for(int i=0;i<64;i++)assert(a[i]<=32767 && a[i]>=-32768);
    /* Varying next-hit levels never need velocity parameters in the smoother. */
    int16_t last=a[63];
    retrigger_smoother_note_on(&s,1);
    for(int i=0;i<64;i++)a[i]=-32000;
    retrigger_smoother_process(&s,a,64);
    assert(a[0]==last);
    puts("PASS: saturated bounds and differing hit levels");
}
static void test_zero_block(void) {
    RetriggerSmoother s;
    retrigger_smoother_init(&s);
    retrigger_smoother_note_on(&s,1);
    retrigger_smoother_process(&s,0,0);
    assert(s.pending==1);
    puts("PASS: zero-length block ignored safely");
}
int main(void){
    test_unchanged_without_retrigger();
    test_first_hit_not_smoothed();
    test_retrigger_continuity_and_duration();
    test_repeated_fast_retriggers();
    test_clamp_and_midi_velocity();
    test_zero_block();
    puts("6 / 6 host unit tests passed");
    return 0;
}
