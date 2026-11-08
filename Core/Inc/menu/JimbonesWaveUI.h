/* SPDX-License-Identifier: GPL-3.0-only
 * JB-24: independently implemented, wavetable-first OSC pages.
 * Historical kit parameter IDs are unchanged for preset compatibility.
 */
#ifndef JIMBONES_WAVE_UI_H
#define JIMBONES_WAVE_UI_H
#include <stdint.h>
#include "Soundengine/DSPAudio/Jimbones.h"

/* Only controls that affect the live wave layer remain visible: three
 * four-knob banks plus native oscillator interpolation. Old preset fields
 * retain their IDs but no longer create phantom pages. */
#define JB_WAVEUI_VISIBLE 12u
#define JB_WAVEUI_BANKS 3u
#define JB_WAVEUI_INTERP 12u

uint8_t jb_waveui_param(uint8_t visible_index); /* JB_PARAMS when invalid */
uint8_t jb_waveui_col(uint8_t bank, uint8_t col); /* visible index, or INTERP */
uint8_t jb_waveui_next(uint8_t visible_index, int delta);
const char *jb_waveui_short(uint8_t visible_index);
const char *jb_waveui_long(uint8_t visible_index);
const char *jb_waveui_shape_name(uint8_t position, uint8_t interpolation);
#endif
