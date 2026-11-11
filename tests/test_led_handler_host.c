/* Runtime regression for the real LED handler. Host-only board and sequencer
 * boundaries are fixed inputs; button routing remains a separate source guard. */
#ifndef JB_HOST_PROBE
#error "LED host test must not enter firmware"
#endif
#include "ledHandler.h"
#include "buttonHandler.h"
#include "menu/menu.h"
#include "Soundengine/globals.h"
#include "Soundengine/Sequencer/sequencer.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

uint8_t dout_outputData[NUM_OUTS/8];
uint8_t jb_hostPageDownLed;
uint8_t activeParameters[NUM_PARAMS];
uint8_t menu_activeBar, menu_activePatternBank, menu_playedPattern;
uint8_t menu_shownPattern, menu_muteModeActive;
volatile uint32_t systick_ticks;
static uint8_t host_mode = SELECT_MODE_STEP;
static uint8_t armed[16];
extern volatile uint8_t led_originalLedState[NUM_OUTS/8];
extern volatile uint8_t led_blinkingLeds;
extern uint8_t led_blinkToggleState, led_blinkPrescaler;

uint8_t buttonHandler_getMode(void) { return host_mode; }
uint8_t menu_getActiveVoice(void) { return 0; }
uint8_t menu_getViewedPattern(void) { return 0; }
uint8_t seq_getTrackLength(uint8_t voice) { (void)voice; return 16; }
uint8_t seq_isMainStepActive(uint8_t voice, uint8_t step, uint8_t pattern)
{ (void)voice; (void)pattern; return step < 16 ? armed[step] : 0; }

static int lit(uint8_t led)
{ return !!(dout_outputData[led/8] & (1u << (led & 7))); }
static int stored(uint8_t led)
{ return !!(led_originalLedState[led/8] & (1u << (led & 7))); }

int main(void)
{
    const uint8_t step1 = led_idx2StepLed(0);
    const uint8_t step2 = led_idx2StepLed(1);
    const uint8_t step16 = led_idx2StepLed(15);
    assert(step1 == LED_STEP1 && step2 == LED_STEP2 && step16 == LED_STEP16);
    led_init();
    armed[0] = armed[1] = armed[15] = 1;
    led_updateSeqLeds();
    assert(lit(step1) && lit(step2) && lit(step16));
    assert(stored(step1) && stored(step2) && stored(step16));

    /* A duplicate blink request must not claim a second timer slot. */
    led_setBlinkLed(step1, 1);
    led_setBlinkLed(step1, 1);
    assert(led_blinkingLeds == 1u);
    for (int tick = 0; tick < 4; ++tick) led_blinkHandler();
    led_setBlinkLed(step1, 0);
    assert(led_blinkingLeds == 0u && lit(step1));
    led_clearAllBlinkLeds();
    assert(lit(step1) && lit(step2) && lit(step16));

    /* Playback chase temporarily inverts each armed step; stop restores it. */
    const uint8_t chaseSteps[] = {0, 1, 15};
    for (unsigned i = 0; i < sizeof(chaseSteps); ++i) {
        const uint8_t chaseLed = led_idx2StepLed(chaseSteps[i]);
        led_setActive_step(chaseSteps[i]);
        assert(!lit(chaseLed) && stored(chaseLed));
        led_clearActive_step();
        assert(lit(chaseLed));
    }
    assert(lit(step1) && lit(step2) && lit(step16));

    host_mode = SELECT_MODE_PERF;
    led_clearSequencerLeds();
    led_setValue(1, step16);
    armed[0] = armed[1] = armed[15] = 0;
    led_updateSeqLeds(); /* Performance display owns the LEDs. */
    assert(!lit(step1) && !lit(step2) && lit(step16));
    host_mode = SELECT_MODE_STEP;
    led_updateSeqLeds();
    assert(!lit(step1) && !lit(step2) && !lit(step16));
    for (unsigned i = 0; i < sizeof(chaseSteps); ++i) {
        const uint8_t chaseLed = led_idx2StepLed(chaseSteps[i]);
        led_setActive_step(chaseSteps[i]);
        assert(lit(chaseLed) && !stored(chaseLed));
        led_clearActive_step();
        assert(!lit(chaseLed));
    }
    armed[0] = armed[1] = armed[15] = 1;
    led_updateSeqLeds();
    assert(lit(step1) && lit(step2) && lit(step16));

    /* Reinitialisation must clear the blink phase as well as slot owners. */
    led_blinkToggleState = led_blinkPrescaler = 1;
    led_init();
    assert(led_blinkingLeds == 0u);
    assert(led_blinkToggleState == 0u && led_blinkPrescaler == 0u);
    puts("PASS: real LED handler Step 1/2/16, blink ownership, chase, mode and reinit");
    return 0;
}
