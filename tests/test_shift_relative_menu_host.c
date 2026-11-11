/* Host integration for the production menu's Shift filter edit and CC output.
 * Hardware button state, recording state and the UART boundary are controlled.
 * The production menu.c is included so its private encoder helper is tested. */
#ifndef JB_HOST_PROBE
#error "This test is host only"
#endif
#include "../Core/Src/menu/menu.c"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static uint8_t host_shift, host_recording, host_armed;
static struct { uint8_t status, param, value; } sent[32];
static unsigned sent_count;

uint8_t buttonHandler_getShift(void) { return host_shift; }
uint8_t seq_isAutomationStepArmed(void) { return host_armed; }
uint8_t seq_isRecordingMode(void) { return host_recording; }
void frontPanel_sendData(uint8_t status, uint8_t param, uint8_t value)
{
    assert(sent_count < sizeof(sent) / sizeof(sent[0]));
    sent[sent_count].status = status;
    sent[sent_count].param = param;
    sent[sent_count++].value = value;
}

static void seed_group(uint16_t base)
{
    const uint8_t seed[6] = {10, 40, 70, 100, 126, 0};
    for (unsigned i = 0; i < 6; ++i) activeParameters[base + i] = seed[i];
    sent_count = 0;
}

static void check_group(uint16_t base)
{
    static const uint8_t plus[6] = {15, 45, 75, 105, 127, 5};
    static const uint8_t minus[6] = {0, 25, 55, 85, 107, 0};
    seed_group(base);
    for (unsigned selected = 0; selected < 6; ++selected) {
        seed_group(base);
        assert(menu_tryShiftEditVoiceFilters(base + selected, 5));
        assert(sent_count == 6);
        for (unsigned i = 0; i < 6; ++i) {
            assert(activeParameters[base + i] == plus[i]);
            assert(sent[i].status == MIDI_CC);
            assert(sent[i].param == base + i);
            assert(sent[i].value == plus[i]);
        }
        sent_count = 0;
        assert(menu_tryShiftEditVoiceFilters(base + selected, -20));
        assert(sent_count == 6);
        for (unsigned i = 0; i < 6; ++i) {
            assert(activeParameters[base + i] == minus[i]);
            assert(sent[i].param == base + i && sent[i].value == minus[i]);
        }
    }
}

int main(void)
{
    host_shift = 1;
    activeParameters[PAR_VOL1] = 77;
    check_group(PAR_FILTER_FREQ_1);
    check_group(PAR_RESO_1);
    assert(activeParameters[PAR_VOL1] == 77);

    for (unsigned i = 0; i < 6; ++i) activeParameters[PAR_RESO_1 + i] = 127;
    sent_count = 0;
    assert(menu_tryShiftEditVoiceFilters(PAR_RESO_6, 3));
    assert(sent_count == 0);
    for (unsigned i = 0; i < 6; ++i) activeParameters[PAR_RESO_1 + i] = 0;
    assert(menu_tryShiftEditVoiceFilters(PAR_RESO_1, -3));
    assert(sent_count == 0);

    seed_group(PAR_FILTER_FREQ_1);
    host_shift = 0;
    assert(!menu_tryShiftEditVoiceFilters(PAR_FILTER_FREQ_2, 5));
    assert(sent_count == 0 && activeParameters[PAR_FILTER_FREQ_1] == 10);
    host_shift = 1;
    host_recording = 1;
    assert(!menu_tryShiftEditVoiceFilters(PAR_FILTER_FREQ_2, 5));
    host_recording = 0;
    host_armed = 1;
    assert(!menu_tryShiftEditVoiceFilters(PAR_FILTER_FREQ_2, 5));
    host_armed = 0;
    assert(!menu_tryShiftEditVoiceFilters(PAR_FILTER_FREQ_2, 0));
    assert(!menu_tryShiftEditVoiceFilters(PAR_VOL1, 5));
    assert(sent_count == 0);
    /* Every voice must remain untouched when Shift is released, automation
     * is armed/recording, the encoder delta is zero, or the target is unrelated. */
    const uint8_t unchanged[6] = {10, 40, 70, 100, 126, 0};
    for (unsigned i = 0; i < 6; ++i)
        assert(activeParameters[PAR_FILTER_FREQ_1 + i] == unchanged[i]);
    puts("PASS: production menu Shift mapping, six CCs, saturation and recording guard");
    return 0;
}
