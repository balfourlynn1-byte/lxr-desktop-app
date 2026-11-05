/* JIMBONES experimental layers. SPDX-License-Identifier: GPL-3.0-only */
#ifndef JIMBONES_H
#define JIMBONES_H
#include <stdint.h>
enum { JB_MODE, JB_METAL_LEVEL, JB_WAVE_LEVEL, JB_DECAY,
 JB_METAL, JB_NOISE, JB_AGGRESSION, JB_TEXTURE,
 JB_VMETAL, JB_VNOISE, JB_VAGGRESSION, JB_VTEXTURE,
 JB_PITCH, JB_RATIO, JB_DEPTH, JB_FEEDBACK,
 JB_DETUNE, JB_NOISE_LEVEL, JB_COLOUR, JB_TRANSIENT,
 JB_DRIVE, JB_CHARACTER, JB_BITS, JB_VELOCITY,
 JB_POSITION, JB_MOVEMENT, JB_RANDOM, JB_PITCH_ENV, JB_PARAMS };
#define JB_VOICES 6
#define JB_KIT_BYTES (JB_VOICES * JB_PARAMS)
extern uint8_t jb_parameters[JB_VOICES][JB_PARAMS];
void jb_defaults(uint8_t *kit);
void jb_init(void);
void jb_loadKit(const uint8_t *kit);
void jb_silence(void);
void jb_trigger(uint8_t voice, uint8_t velocity, uint8_t note, uint8_t open);
void jb_render(uint8_t voice, int16_t *buffer, unsigned size, float gain);
uint8_t jb_limit(unsigned parameter);
void jb_adjust(unsigned voice, unsigned parameter, int delta);
#endif
