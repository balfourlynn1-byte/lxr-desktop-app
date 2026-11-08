/* Host-only deterministic regression for the original 132-sample No Clip smoother. */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include "Soundengine/DSPAudio/RetriggerSmoother.h"

static void test_fresh_note_and_zero_length(void) {
    RetriggerSmoother s; retrigger_smoother_init(&s);
    int16_t a[3] = {100, 200, 300};
    retrigger_smoother_note_on(&s, 0);
    retrigger_smoother_process(&s, a, 0);
    assert(s.last_sample == 0);
    retrigger_smoother_process(&s, a, 3);
    assert(a[0] == 100 && a[1] == 200 && a[2] == 300);
    assert(s.last_sample == 300 && s.remaining == 0);
}
static void test_exact_ramp_across_blocks(void) {
    RetriggerSmoother s; retrigger_smoother_init(&s);
    int16_t seed = 1000;
    retrigger_smoother_process(&s, &seed, 1);
    retrigger_smoother_note_on(&s, 1);
    for (unsigned i = 0; i < LXR_RETRIGGER_FADE_SAMPLES; ++i) {
        int16_t sample = 0;
        retrigger_smoother_process(&s, &sample, 1);
        int expected = 1000 - (int)(1000.0f * (float)i / 131.0f);
        assert(sample >= expected - 2 && sample <= expected + 2);
        assert(s.remaining == LXR_RETRIGGER_FADE_SAMPLES - i - 1);
        if (i == 0) assert(sample == 1000);
        if (i == 131) assert(sample == 0);
    }
    int16_t post = 1234;
    retrigger_smoother_process(&s, &post, 1);
    assert(post == 1234);
}
static void test_retrigger_mid_ramp_and_saturation(void) {
    RetriggerSmoother s; retrigger_smoother_init(&s);
    int16_t seed = 32767;
    retrigger_smoother_process(&s, &seed, 1);
    retrigger_smoother_note_on(&s, 1);
    int16_t samples[8] = {-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768};
    retrigger_smoother_process(&s, samples, 8);
    assert(samples[0] == 32767 && s.remaining == 124);
    int16_t before = s.last_sample;
    retrigger_smoother_note_on(&s, 1);
    int16_t next[2] = {-32768, -32768};
    retrigger_smoother_process(&s, next, 2);
    assert(next[0] == before && s.remaining == 130);
}
static void test_independent_voices(void) {
    RetriggerSmoother a,b; retrigger_smoother_init(&a); retrigger_smoother_init(&b);
    int16_t x = 1200, y = -700;
    retrigger_smoother_process(&a,&x,1); retrigger_smoother_process(&b,&y,1);
    retrigger_smoother_note_on(&a,1);
    x = 0; y = 400;
    retrigger_smoother_process(&a,&x,1); retrigger_smoother_process(&b,&y,1);
    assert(x == 1200 && y == 400 && a.remaining == 131 && b.remaining == 0);
}
int main(void) {
    assert(LXR_RETRIGGER_FADE_SAMPLES == 132u);
    test_fresh_note_and_zero_length();
    test_exact_ramp_across_blocks();
    test_retrigger_mid_ramp_and_saturation();
    test_independent_voices();
    puts("No Clip 132-sample host regression PASS");
    return 0;
}
