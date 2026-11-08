/* SPDX-License-Identifier: GPL-3.0-only
 * JB-19B host regression: tests production gated stereo FX tap.
 */
#include "Soundengine/StepFxSend.h"
#include "Soundengine/StepFxSendMix.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define FRAMES 64u
static int16_t voice[FRAMES];
static int16_t fx[FRAMES*2u];

static void fill_voice(int16_t value) {
    for (unsigned i=0;i<FRAMES;i++) voice[i]=value;
}
static void fill_fx(int16_t value) {
    for (unsigned i=0;i<FRAMES*2u;i++) fx[i]=value;
}
static void render(uint8_t v, uint8_t routed_to_fx, uint8_t fx_on,
                   float pan_l, float pan_r) {
    stepfx_send_mix_voice(v,routed_to_fx,fx_on,
                         voice,FRAMES,pan_l,pan_r,fx);
}
static int nonzero(void) {
    for (unsigned i=0;i<FRAMES*2u;i++) if(fx[i])return 1;
    return 0;
}

int main(void) {
    fill_voice(12000);
    int16_t original[FRAMES];
    memcpy(original,voice,sizeof original);

    /* Default OFF must leave nonzero tail data in FX input unchanged.
       No writes to the dry voice buffer whatsoever. */
    stepfx_send_init();
    stepfx_send_gate(3,300u);
    fill_fx(321);
    int16_t before[FRAMES*2u];
    memcpy(before,fx,sizeof before);
    render(3,0,1,0.8f,0.6f);
    assert(memcmp(before,fx,sizeof fx)==0);
    assert(memcmp(original,voice,sizeof voice)==0);

    /* Enabled snare: same dry samples, additional stereo send copy. */
    stepfx_send_init();
    stepfx_send_set_enabled(1);
    stepfx_send_gate(3,350u);
    fill_fx(0);
    render(3,0,1,0.8f,0.6f);
    assert(nonzero());
    for (unsigned i=0;i<FRAMES;i++) {
        assert(fx[2u*i] >= fx[2u*i+1u]);
        assert(fx[2u*i]>=0 && fx[2u*i+1u]>=0);
    }
    assert(memcmp(original,voice,sizeof voice)==0);

    /* A closed send fades out while existing FX accumulator is preserved. */
    stepfx_send_gate(3,0);
    for(unsigned k=0;k<5u;k++) {
        fill_fx(41);
        render(3,0,1,1.0f,1.0f);
    }
    for(unsigned i=0;i<FRAMES*2u;i++)assert(fx[i]==41);

    /* Legacy OUT=FX must remain exactly single-routed: no second copy.
       The hold/release frames still advance while output is FX-only. */
    stepfx_send_init();
    stepfx_send_set_enabled(1);
    stepfx_send_gate(3,60);
    for(unsigned k=0;k<5u;k++) {
        fill_fx(0);
        render(3,1,1,1.0f,1.0f);
        assert(!nonzero());
    }
    render(3,0,1,1.0f,1.0f);
    assert(!nonzero());

    /* Global FX OFF is a pass-through in original code; no doubled dry. */
    stepfx_send_init();
    stepfx_send_set_enabled(1);
    stepfx_send_gate(3,60);
    for(unsigned k=0;k<5u;k++) {
        fill_fx(0);
        render(3,0,0,1.0f,1.0f);
        assert(!nonzero());
    }
    render(3,0,1,1.0f,1.0f);
    assert(!nonzero());

    /* Default kick protection, then explicit opt-in. */
    stepfx_send_init();
    stepfx_send_set_enabled(1);
    stepfx_send_gate(0,500u);
    fill_fx(0);render(0,0,1,1.0f,0.0f);
    assert(!nonzero());
    stepfx_send_set_mask(0x3fu);
    stepfx_send_gate(0,500u);
    render(0,0,1,1.0f,0.0f);
    assert(nonzero());
    for(unsigned i=0;i<FRAMES;i++)assert(fx[2u*i+1u]==0);

    /* Zero-level requests consume their hold and cannot resurrect later. */
    stepfx_send_init();
    stepfx_send_set_enabled(1);
    stepfx_send_set_level(2,0);
    stepfx_send_gate(2,40u);
    for(unsigned k=0;k<5u;k++){
        fill_fx(0);render(2,0,1,1.0f,1.0f);
        assert(!nonzero());
    }
    stepfx_send_set_level(2,127);
    fill_fx(0);render(2,0,1,1.0f,1.0f);
    assert(!nonzero());

    /* Four simultaneous gated tom/snare/cymbal copies saturate safely. */
    stepfx_send_init();
    stepfx_send_set_enabled(1);
    fill_voice(INT16_MAX);
    for(unsigned v=1;v<=4;v++)stepfx_send_gate((uint8_t)v,1500u);
    for(unsigned k=0;k<3u;k++){
        fill_fx(30000);
        for(unsigned v=1;v<=4;v++)render((uint8_t)v,0,1,1.0f,1.0f);
        for(unsigned i=0;i<FRAMES*2u;i++)assert(fx[i]>=30000);
    }
    assert(fx[2u*FRAMES-2u]==INT16_MAX);

    /* Negative saturation, pan clamping and invalid voice index. */
    stepfx_send_init();
    stepfx_send_set_enabled(1);
    fill_voice(INT16_MIN);
    fill_fx(-30000);
    for(unsigned v=1;v<=4;v++)stepfx_send_gate((uint8_t)v,1000u);
    for(unsigned k=0;k<3u;k++)
        for(unsigned v=1;v<=4;v++)render((uint8_t)v,0,1,2.0f,2.0f);
    assert(fx[2u*FRAMES-2u]==INT16_MIN);
    int16_t tail[FRAMES*2u];memcpy(tail,fx,sizeof tail);
    render(7,0,1,1.0f,1.0f);
    assert(memcmp(tail,fx,sizeof fx)==0);

    puts("PASS JB-19B: bypass, dry parity, additive stereo sends, gate release, FX-only and FX-OFF suppression, kick, level zero, four-voice saturation");
    return 0;
}
