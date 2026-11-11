/* Real front-panel, menu, MIDI and DSP Shift-relative regression on native host. */
#include "lxr_api.h"
#include "Parameters.h"
#include "buttonHandler.h"
#include "menu/menu.h"
#include "Soundengine/MIDI/ParameterArray.h"
#include "Soundengine/Sequencer/sequencer.h"
#include <stdio.h>
#undef NDEBUG
#include <assert.h>

static const int seed[6] = {20, 40, 60, 80, 100, 120};

static void idle(void) { lxr_idle_blocks(32); }
static void click(uint8_t button) { lxr_button_click(button); idle(); }
static void encoder_click(void)
{
    lxr_encoder_button(1); idle();
    lxr_encoder_button(0); idle();
}
static void turn(int delta) { lxr_encoder_turn(delta); idle(); }
static void shift(uint8_t down)
{
    lxr_button(BUT_SHIFT, down); idle();
    assert(buttonHandler_getShift() == down);
}
static void set_group(int base, const int values[6])
{
    for (int i = 0; i < 6; ++i) lxr_set_param(base + i, values[i]);
    idle();
}
static void expect_group(int base, const int values[6])
{
    for (int i = 0; i < 6; ++i) assert(lxr_get_param(base + i) == values[i]);
}
static void dsp_snapshot(int base, float values[6])
{
    for (int i = 0; i < 6; ++i) {
        assert(parameterArray[base + i].ptr != NULL);
        values[i] = *(float *)parameterArray[base + i].ptr;
    }
}
static void expect_dsp_changed(int base, const float before[6], unsigned mask)
{
    for (int i = 0; i < 6; ++i) {
        const float now = *(float *)parameterArray[base + i].ptr;
        if ((now != before[i]) != !!(mask & (1u << i)))
            fprintf(stderr, "DSP mismatch base=%d voice=%d before=%f now=%f mask=%u\n",
                    base, i, before[i], now, mask);
        assert((now != before[i]) == !!(mask & (1u << i)));
    }
}

int main(void)
{
    assert(lxr_init(123, 128) == 0);
    lxr_idle_blocks(1600);
    set_group(PAR_FILTER_FREQ_1, seed);
    lxr_set_param(PAR_VOL1, 77);
    click(BUT_MODE1);
    click(BUT_VOICE_1);
    click(BUT_SELECT6); /* filter page; cutoff is the first encoder item */
    assert(buttonHandler_getMode() == SELECT_MODE_VOICE && menu_activePage == 0);
    float before[6];
    dsp_snapshot(PAR_FILTER_FREQ_1, before);
    encoder_click(); /* enter edit mode */
    shift(1);
    turn(1);
    const int plus[6] = {21, 41, 61, 81, 101, 121};
    expect_group(PAR_FILTER_FREQ_1, plus);
    expect_dsp_changed(PAR_FILTER_FREQ_1, before, 0x3f);

    shift(0);
    dsp_snapshot(PAR_FILTER_FREQ_1, before);
    turn(1);
    const int unshifted[6] = {22, 41, 61, 81, 101, 121};
    expect_group(PAR_FILTER_FREQ_1, unshifted);
    expect_dsp_changed(PAR_FILTER_FREQ_1, before, 0x01);

    shift(1);
    turn(-1);
    const int minus[6] = {21, 40, 60, 80, 100, 120};
    expect_group(PAR_FILTER_FREQ_1, minus);
    assert(lxr_get_param(PAR_VOL1) == 77);

    const int limits[6] = {0, 1, 126, 127, 50, 100};
    set_group(PAR_FILTER_FREQ_1, limits);
    turn(1);
    const int high[6] = {1, 2, 127, 127, 51, 101};
    expect_group(PAR_FILTER_FREQ_1, high);
    turn(-1);
    const int low[6] = {0, 1, 126, 126, 50, 100};
    expect_group(PAR_FILTER_FREQ_1, low);

    set_group(PAR_FILTER_FREQ_1, seed);
    seq_setRecordingMode(1);
    turn(1);
    const int single[6] = {21, 40, 60, 80, 100, 120};
    expect_group(PAR_FILTER_FREQ_1, single);
    seq_setRecordingMode(0);

    set_group(PAR_FILTER_FREQ_1, seed);
    seq_armAutomationStep(0, 0, 1);
    assert(seq_isAutomationStepArmed());
    turn(1);
    expect_group(PAR_FILTER_FREQ_1, single);
    assert(seq_patternSet.seq_subStepPattern[seq_activePattern][0][0].param1Nr ==
           PAR_FILTER_FREQ_1 + 1);
    assert(seq_patternSet.seq_subStepPattern[seq_activePattern][0][0].param1Val == 21);
    seq_armAutomationStep(0, 0, 0);

    shift(0);
    const int resonance_seed[6] = {10, 30, 50, 70, 90, 110};
    const int resonance_plus[6] = {11, 31, 51, 71, 91, 111};
    set_group(PAR_RESO_1, resonance_seed);
    encoder_click(); /* leave cutoff edit */
    turn(1);         /* select resonance */
    encoder_click(); /* edit resonance */
    dsp_snapshot(PAR_RESO_1, before);
    shift(1);
    turn(1);
    expect_group(PAR_RESO_1, resonance_plus);
    expect_dsp_changed(PAR_RESO_1, before, 0x3f);
    assert(lxr_get_param(PAR_VOL1) == 77);
    puts("PASS: physical-style Shift/encoder to six DSP filters, limits and automation isolation");
    return 0;
}
