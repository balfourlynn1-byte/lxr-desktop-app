/* SPDX-License-Identifier: GPL-3.0-only
 * JB-26: 10x12 self-contained, low-CPU 2x2 character LCD skull sprite.
 * No pixel-framebuffer/DMA work in the audio path. */
#ifndef JIMBONES_SKULL_H
#define JIMBONES_SKULL_H
#include <stdint.h>

#define JB_SKULL_FRAMES 8u
#define JB_SKULL_GLYPHS 4u
#define JB_SKULL_ROWS 8u
/* Produces four 5x8 character bitmaps, top-left/top-right/bottom-left/bottom-right.
 * vertical phase cycles 0,1,2,3,4,3,2,1 and is bounded to 16 pixels. */
void jb_skull_make_frame(uint8_t frame, uint8_t out[JB_SKULL_GLYPHS][JB_SKULL_ROWS]);
uint8_t jb_skull_yoffset(uint8_t frame);
#endif
