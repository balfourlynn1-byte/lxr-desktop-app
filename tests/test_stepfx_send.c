/* JB-19A: deterministic send-gate tests with no STM32 or audio engine. */
#include "Soundengine/StepFxSend.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

static uint16_t sample(uint8_t v) {
    return stepfx_send_next_gain_q15(v);
}
static uint16_t advance(uint8_t v, unsigned n) {
    uint16_t last = 0;
    while (n--) last = sample(v);
    return last;
}
static void block(uint8_t v) { stepfx_send_begin_block(v); }

int main(void) {
    stepfx_send_init();
    /* Global bypass even if a send gate is requested; all six lanes silent. */
    stepfx_send_gate(3, 400u);
    for (unsigned v = 0; v < 6; ++v) {
        block((uint8_t)v);
        assert(advance((uint8_t)v, 500) == 0);
    }

    stepfx_send_init();
    stepfx_send_set_enabled(1);
    /* Default kick protected; another voice has an audible short snippet. */
    stepfx_send_gate(0, 500u);
    stepfx_send_gate(3, 500u);
    block(0);
    block(3);
    assert(sample(0) == 0u);
    const uint16_t attack1 = sample(3);
    assert(attack1 > 0u);
    const uint16_t attack2 = sample(3);
    assert(attack2 > attack1);
    const uint16_t peak = advance(3, 220);
    assert(peak > 30000u);
    assert(advance(3, 600) == 0u);

    /* A second send request extends the gate without a reset/click. */
    stepfx_send_gate(3, 200u);
    block(3);
    advance(3, 120);
    const uint16_t before = sample(3);
    stepfx_send_gate(3, 400u);
    block(3);
    const uint16_t after = sample(3);
    assert(after >= before);
    assert(after - before < 1000u);

    /* A close command fades out (does not touch delay feedback). */
    stepfx_send_gate(3, 0u);
    block(3);
    assert(advance(3, 10) > 0u);
    assert(advance(3, 300) == 0u);

    /* Mask is independent of legacy Step FX selection / note triggering. */
    stepfx_send_gate(0, 400u);
    stepfx_send_set_mask(0x3fu);
    block(0);
    assert(advance(0, 150) > 0);
    stepfx_send_set_mask(0x3eu);
    block(0);
    assert(sample(0) == 0u);

    /* Zero send level is exact bypass, and dynamic level smoothing works. */
    stepfx_send_init();
    stepfx_send_set_enabled(1);
    stepfx_send_set_level(3, 0);
    stepfx_send_gate(3, 1500u);
    block(3);
    assert(advance(3, 200) == 0u);
    stepfx_send_set_level(3, 127);
    block(3);
    const uint16_t low = sample(3);
    assert(low > 0u && low < 3000u);
    const uint16_t high = advance(3, 300);
    assert(high > low && high > 30000u);
    stepfx_send_set_enabled(0);
    block(3);
    assert(sample(3) == 0u);

    /* Four independently gated lanes; excluded kick remains dry-only. */
    stepfx_send_init();
    stepfx_send_set_enabled(1);
    for (unsigned v = 1; v <= 4; ++v) {
        stepfx_send_gate((uint8_t)v, 250u);
        block((uint8_t)v);
    }
    block(0);
    for (unsigned i = 0; i < 100; ++i)
        for (unsigned v = 1; v <= 4; ++v)
            assert(sample((uint8_t)v) > 0u);
    assert(sample(0) == 0u);
    for (unsigned v = 1; v <= 4; ++v)
        assert(advance((uint8_t)v, 450) == 0u);

    /* Bounds: invalid indices and excessively long request stay safe. */
    stepfx_send_gate(7, 50);
    stepfx_send_set_level(7, 127);
    stepfx_send_begin_block(7);
    assert(stepfx_send_next_gain_q15(7) == 0);
    stepfx_send_gate(5, UINT32_MAX);
    block(5);
    assert(advance(5, 100) > 0u);

    puts("PASS JB-19A: bypass, protected kick, attack/release, retrigger, close, mask, level, four lanes, bounds");
    return 0;
}
