/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef JIMBONES_PRESET_H
#define JIMBONES_PRESET_H
#include "Soundengine/DSPAudio/Jimbones.h"
#define JB_RECORD_BYTES (12+JB_KIT_BYTES)
void jb_pack(uint8_t *out,const uint8_t *kit);
int jb_unpack(uint8_t *kit,const uint8_t *in,unsigned size);
#endif
