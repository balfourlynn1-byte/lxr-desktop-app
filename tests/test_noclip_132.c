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

/* A03 edge: exact 132-sample boundary with mixed real-world DMA block sizes.
 * A zero-length call must not consume a pending note-on or fade sample. */
static void test_mixed_block_boundaries(void) {
    RetriggerSmoother s; retrigger_smoother_init(&s);
    int16_t seed = -1200;
    retrigger_smoother_process(&s, &seed, 1);
    retrigger_smoother_note_on(&s, 1);
    const uint8_t sizes[] = {64, 0, 64, 4, 16};
    unsigned offset = 0;
    for (unsigned b = 0; b < sizeof(sizes)/sizeof(sizes[0]); ++b) {
        int16_t samples[64];
        for (unsigned j = 0; j < 64; ++j) samples[j] = 0;
        retrigger_smoother_process(&s, samples, sizes[b]);
        for (unsigned j = 0; j < sizes[b]; ++j) {
            unsigned index = offset + j;
            if (index == 0) assert(samples[j] == -1200);
            if (index == 131) assert(samples[j] == 0);
            if (index >= 132) assert(samples[j] == 0);
        }
        offset += sizes[b];
        assert(s.remaining == (offset < 132 ? 132 - offset : 0));
    }
    assert(offset == 148 && s.pending == 0 && s.correction == 0.0f);
}
/* A03: extreme opposite-polarity retrigger must clamp safely and settle. */
static void test_full_scale_opposite_polarity(void) {
    const int16_t old_values[2] = {-32768, 32767};
    const int16_t new_values[2] = {32767, -32768};
    for (unsigned polarity = 0; polarity < 2; ++polarity) {
        RetriggerSmoother s; retrigger_smoother_init(&s);
        int16_t seed = old_values[polarity];
        retrigger_smoother_process(&s, &seed, 1);
        retrigger_smoother_note_on(&s, 1);
        for (unsigned i = 0; i < LXR_RETRIGGER_FADE_SAMPLES; ++i) {
            int16_t sample = new_values[polarity];
            retrigger_smoother_process(&s, &sample, 1);
            if (i == 0) assert(sample == old_values[polarity]);
            if (i == LXR_RETRIGGER_FADE_SAMPLES - 1u)
                assert(sample >= new_values[polarity] - (new_values[polarity] > 0 ? 1 : 0) &&
                       sample <= new_values[polarity] + (new_values[polarity] < 0 ? 1 : 0));
        }
        assert(s.remaining == 0u && s.correction == 0.0f);
        int16_t after = new_values[polarity];
        retrigger_smoother_process(&s, &after, 1);
        assert(after == new_values[polarity]);
    }
}

/* A03: a zero-frame callback immediately after note-on must preserve the pending trigger. */
static void test_pending_survives_zero_frame(void) {
    RetriggerSmoother s; retrigger_smoother_init(&s);
    int16_t seed = 2718;
    retrigger_smoother_process(&s, &seed, 1);
    retrigger_smoother_note_on(&s, 1);
    unsigned pending_before = s.pending;
    unsigned remaining_before = s.remaining;
    int16_t untouched = -1234;
    retrigger_smoother_process(&s, &untouched, 0);
    assert(untouched == -1234);
    assert(s.pending == pending_before && s.remaining == remaining_before);
    int16_t next = -3000;
    retrigger_smoother_process(&s, &next, 1);
    assert(next == 2718 && s.remaining == LXR_RETRIGGER_FADE_SAMPLES - 1u);
}

int main(void) {
    assert(LXR_RETRIGGER_FADE_SAMPLES == 132u);
    test_fresh_note_and_zero_length();
    test_pending_survives_zero_frame();
    test_exact_ramp_across_blocks();
    test_retrigger_mid_ramp_and_saturation();
    test_independent_voices();
    test_mixed_block_boundaries();
    test_full_scale_opposite_polarity();
    puts("No Clip 132-sample host regression PASS");
    return 0;
}
