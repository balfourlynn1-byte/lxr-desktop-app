/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef STEP_FX_H
#define STEP_FX_H
#include <stdint.h>
typedef void (*StepFxEmit)(uint8_t voice,uint8_t note,uint8_t velocity);
void stepfx_init(void);
void stepfx_select(uint8_t slot);
void stepfx_rate(int delta);
void stepfx_probability(int delta);
void stepfx_include(uint8_t voice,uint8_t yes);
uint8_t stepfx_slot(void);
uint8_t stepfx_divisor(void);
uint8_t stepfx_chance(void);
uint8_t stepfx_mask(void);
const char *stepfx_name(uint8_t slot);
void stepfx_live(uint8_t voice,uint8_t note,uint8_t velocity);
void stepfx_process(uint32_t frames,uint16_t bpm,uint8_t external,StepFxEmit emit);
void stepfx_clock(void);
void stepfx_stop(void);
unsigned stepfx_pending(void);
/* Dub send controller only: routing to the native FX bus is separately gated.
 * Voice 0 (kick) excluded by default; no audio-path changes in this revision. */
void stepfx_dubEnable(uint8_t enabled);
void stepfx_dubVoice(uint8_t voice,uint8_t enabled);
void stepfx_dubThrow(uint8_t voice,uint8_t enabled);
uint8_t stepfx_dubEnabled(void);
uint8_t stepfx_dubSendMask(void);
uint8_t stepfx_dubThrowMask(void);
uint8_t stepfx_dubEffectiveMask(void);
#endif
