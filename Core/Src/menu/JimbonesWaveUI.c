/* SPDX-License-Identifier: GPL-3.0-only
 * JB-24: wave-first controls on the 16x2 LCD with four knobs.
 * Written independently; no third-party UI source has been copied.
 */
#include "menu/JimbonesWaveUI.h"

/* This table defines navigation only. Keep old 32-field kit IDs/strides.
 * 6 obsolete FM-only controls and metal levels are not displayed.
 * Existing wave/noise/shape controls remain in their original parameter IDs.
 */
static const uint8_t visible[JB_WAVEUI_VISIBLE] = {
    /* PAGE 1: oscillator / waveform selection */
    JB_MODE,JB_WAVE_LEVEL,JB_POSITION,JB_MOVEMENT,
    /* PAGE 2: waveform shaping */
    JB_SHAPE,JB_CHARACTER,JB_DRIVE,JB_BITS,
    /* PAGE 3: pitch and movement */
    JB_PITCH,JB_DETUNE,JB_PITCH_ENV,JB_RANDOM,
    /* PAGE 4: dynamics */
    JB_DECAY,JB_TRANSIENT,JB_VELOCITY,JB_BLEND,
    /* PAGE 5: texture/modulation */
    JB_TEXTURE,JB_AGGRESSION,JB_VTEXTURE,JB_VAGGRESSION,
    /* PAGE 6: noise */
    JB_NOISE,JB_NOISE_LEVEL,JB_COLOUR,JB_VNOISE,
    /* PAGE 7: expression / reserved FX parameter */
    JB_VBLEND,JB_FX_AMOUNT
};
static const char *const short_names[JB_WAVEUI_VISIBLE] = {
    "Wav","Lvl","Pos","Mov", "Shp","Chr","Drv","Bit",
    "Pit","Det","PEn","Rnd", "Dec","Trn","Vel","Mix",
    "Tex","Agg","vTx","vAg", "Noi","Nvl","Col","vNo",
    "vMx","FxA"
};
static const char *const long_names[JB_WAVEUI_VISIBLE] = {
    "Wavetable layer","Wave level","Waveform select","Wave movement",
    "Waveshape amount","Wave character","Wave drive","Bit reduction",
    "Carrier pitch","Wave detune","Pitch envelope","Randomisation",
    "Amp decay","Transient length","Velocity sens","Layer blend",
    "Wave texture","Aggression","Velocity > Tex","Velocity > Agg",
    "Noise","Noise level","Noise colour","Velocity > Noise",
    "Velocity > Blend","Step FX amount"
};

uint8_t jb_waveui_param(uint8_t index) {
    return index < JB_WAVEUI_VISIBLE ? visible[index] : (uint8_t)JB_PARAMS;
}
uint8_t jb_waveui_col(uint8_t bank, uint8_t col) {
    if (bank >= JB_WAVEUI_BANKS || col >= 4u) return JB_WAVEUI_INTERP;
    const unsigned index=(unsigned)bank*4u+(unsigned)col;
    return index < JB_WAVEUI_VISIBLE ? (uint8_t)index : JB_WAVEUI_INTERP;
}
uint8_t jb_waveui_next(uint8_t index,int delta) {
    if (!delta) return index<=JB_WAVEUI_INTERP?index:0u;
    if (delta>0) return index>=JB_WAVEUI_INTERP?0u:(uint8_t)(index+1u);
    return index==0u || index>JB_WAVEUI_INTERP ?
           JB_WAVEUI_INTERP : (uint8_t)(index-1u);
}
const char *jb_waveui_short(uint8_t index) {
    return index<JB_WAVEUI_VISIBLE?short_names[index]:"";
}
const char *jb_waveui_long(uint8_t index) {
    return index<JB_WAVEUI_VISIBLE?long_names[index]:"";
}
const char *jb_waveui_shape_name(uint8_t position,uint8_t interpolation) {
    static const char *const shapes[4]={"SINE","TRI","SAW","PULSE"};
    static const char *const between[3]={"SINE>TRI","TRI>SAW","SAW>PULSE"};
    const unsigned scaled=(unsigned)position*3u;
    if (!interpolation) {
        const unsigned rounded=(scaled+63u)/127u;
        return shapes[rounded>3u?3u:rounded];
    }
    const unsigned first=scaled/127u;
    if(first>=3u || scaled%127u==0u) return shapes[first>3u?3u:first];
    return between[first];
}
