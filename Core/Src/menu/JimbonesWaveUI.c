/* SPDX-License-Identifier: GPL-3.0-only
 * JB-28: compact, functioning OSC extension. Original native oscillator
 * waveform / LFO parameters stay on the standard OSC pages. Only actual
 * wave-layer controls remain here; ALL obsolete Metallic FM, unused FX
 * amount, extra noise/pitch and duplicated pages are removed from UI.
 * Historical kit data/parameter IDs are unchanged for compatibility.
 */
#include "menu/JimbonesWaveUI.h"

static const uint8_t visible[JB_WAVEUI_VISIBLE] = {
    /* Wave layer ON/OFF, level, waveform position, movement */
    JB_MODE,JB_WAVE_LEVEL,JB_POSITION,JB_MOVEMENT,
    /* Waveshaping (active only with wave layer ON) */
    JB_SHAPE,JB_CHARACTER,JB_DRIVE,JB_BITS,
    /* Envelope, blend, modulation and grit (active wave layer) */
    JB_DECAY,JB_BLEND,JB_TEXTURE,JB_AGGRESSION
};
static const char *const short_names[JB_WAVEUI_VISIBLE] = {
    "Wav","Lvl","Pos","Mov",
    "Shp","Chr","Drv","Bit",
    "Dec","Mix","Tex","Agg"
};
static const char *const long_names[JB_WAVEUI_VISIBLE] = {
    "Wave layer ON/OFF","Wave level","Waveform position","Wave movement",
    "Wave shape","Wave character","Wave drive","Bit reduction",
    "Wave decay","Wave dry mix","Wave texture","Wave aggression"
};

uint8_t jb_waveui_param(uint8_t index) {
    return index<JB_WAVEUI_VISIBLE?visible[index]:(uint8_t)JB_PARAMS;
}
uint8_t jb_waveui_col(uint8_t bank,uint8_t col) {
    if(bank>=JB_WAVEUI_BANKS || col>=4u)return JB_WAVEUI_INTERP;
    return (uint8_t)(bank*4u+col);
}
uint8_t jb_waveui_next(uint8_t index,int delta) {
    if(!delta)return index<=JB_WAVEUI_INTERP?index:0u;
    if(delta>0)return index>=JB_WAVEUI_INTERP?0u:(uint8_t)(index+1u);
    return !index || index>JB_WAVEUI_INTERP?
           JB_WAVEUI_INTERP:(uint8_t)(index-1u);
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
    if(!interpolation) {
        const unsigned rounded=(scaled+63u)/127u;
        return shapes[rounded>3u?3u:rounded];
    }
    const unsigned first=scaled/127u;
    if(first>=3u || scaled%127u==0u)return shapes[first>3u?3u:first];
    return between[first];
}
