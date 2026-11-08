/* SPDX-License-Identifier: GPL-3.0-only
 * Original screensaver module Copyright (C) 2012-2026 Erica Synths.
 * JB-26 skull animation and non-blocking wake-up changes for JIMBONES.
 *
 * LCD is a 16x2 character display with only eight 5x8 CGRAM glyph slots.
 * This implementation uses glyph IDs 4..7; glyph 0 remains BLANK so the
 * normal menu's historical NUL-character protection still works.
 */
#include "lcd.h"
#include "menu/screensaver.h"
#include "menu/JimbonesSkull.h"
#include "menu/menu.h"
#include "delay.h"
#include "Soundengine/Soundengine.h"
#include <stdint.h>

#define SCREENSAVER_TIMEOUT_MIN 2u
#define SCREENSAVER_SYSTICK (SCREENSAVER_TIMEOUT_MIN*60u*1000u)
#define JB_SKULL_FRAME_MS 170u

volatile uint32_t screensaver_timer=0u;
volatile uint8_t screensaver_Active=0u;
volatile uint32_t screensaver_Timeout=SCREENSAVER_SYSTICK;

enum SkullSaverState {
    SKULL_DRAIN_MENU=0,   /* complete an in-flight menu LCD nibble safely */
    SKULL_CLEAR,
    SKULL_PROGRAM_GLYPHS,
    SKULL_TOP_CURSOR,
    SKULL_TOP_LEFT,
    SKULL_TOP_RIGHT,
    SKULL_BOTTOM_CURSOR,
    SKULL_BOTTOM_LEFT,
    SKULL_BOTTOM_RIGHT,
    SKULL_HOLD,
    SKULL_EXIT_CLEAR
};

static uint8_t state=SKULL_DRAIN_MENU;
static uint8_t glyphs[JB_SKULL_GLYPHS][JB_SKULL_ROWS];
static uint8_t glyph_index;
static uint8_t frame_index;
static uint8_t position=7u;
static uint8_t redraw_position=1u;
static uint8_t exit_requested;
static uint32_t last_frame_ms, frames_rendered;

void screensaver_touch(void) {
    screensaver_timer=0u;
    screensaver_Timeout=SCREENSAVER_SYSTICK;
    /* Do not spin/wait on the LCD! The previous touch handler blocked
     * here and could corrupt half-written bytes in the shared controller.
     * It now requests a deferred, single-owner exit at the frame boundary. */
    if(screensaver_Active) exit_requested=1u;
}

static void enter(void) {
    screensaver_Active=1u;
    exit_requested=0u;
    state=SKULL_DRAIN_MENU;
    glyph_index=0u;
    frame_index=0u;
    frames_rendered=0u;
    position=7u;
    redraw_position=1u;
    last_frame_ms=HAL_GetTick();
    jb_skull_make_frame(frame_index,glyphs);
}

/* Called only from the UI/main loop; no allocation or audio DSP work.
 * All CGRAM and DDRAM transfers use the existing nonblocking LCD API. */
uint8_t screensaver_process(void) {
    if(!screensaver_Active) return 0u;
    switch(state) {
    case SKULL_DRAIN_MENU:
        /* menu_tick() yields LCD ownership while screensaver is active.
         * Advance, rather than busy-wait, an already started menu write. */
        if(sendDisplayBuffer()==0u)state=SKULL_CLEAR;
        break;
    case SKULL_CLEAR:
        if(lcd_clear_nonblock())state=SKULL_PROGRAM_GLYPHS;
        break;
    case SKULL_PROGRAM_GLYPHS:
        if(lcd_generatechar_nonBlock((uint8_t)(4u+glyph_index),glyphs[glyph_index])) {
            glyph_index++;
            if(glyph_index==JB_SKULL_GLYPHS) {
                glyph_index=0u;
                state=redraw_position?SKULL_TOP_CURSOR:SKULL_HOLD;
            }
        }
        break;
    case SKULL_TOP_CURSOR:
        if(lcd_setcursor_nonBlock(position,1u))state=SKULL_TOP_LEFT;
        break;
    case SKULL_TOP_LEFT:
        if(lcd_data_nonBlock(4u))state=SKULL_TOP_RIGHT;
        break;
    case SKULL_TOP_RIGHT:
        if(lcd_data_nonBlock(5u))state=SKULL_BOTTOM_CURSOR;
        break;
    case SKULL_BOTTOM_CURSOR:
        if(lcd_setcursor_nonBlock(position,2u))state=SKULL_BOTTOM_LEFT;
        break;
    case SKULL_BOTTOM_LEFT:
        if(lcd_data_nonBlock(6u))state=SKULL_BOTTOM_RIGHT;
        break;
    case SKULL_BOTTOM_RIGHT:
        if(lcd_data_nonBlock(7u)) {
            redraw_position=0u;
            state=SKULL_HOLD;
            last_frame_ms=HAL_GetTick();
        }
        break;
    case SKULL_HOLD:
        if(exit_requested){state=SKULL_EXIT_CLEAR;break;}
        if((uint32_t)(HAL_GetTick()-last_frame_ms)>=JB_SKULL_FRAME_MS) {
            last_frame_ms=HAL_GetTick();
            frame_index=(uint8_t)((frame_index+1u)%JB_SKULL_FRAMES);
            frames_rendered++;
            /* Very slow horizontal drift avoids illuminating the same
             * OLED columns throughout a long idle screensaver session. */
            if(frames_rendered%24u==0u){
                position=(uint8_t)(3u+((frames_rendered/24u)%10u));
                redraw_position=1u;
            }
            jb_skull_make_frame(frame_index,glyphs);
            glyph_index=0u;
            state=redraw_position?SKULL_CLEAR:SKULL_PROGRAM_GLYPHS;
        }
        break;
    case SKULL_EXIT_CLEAR:
        /* A complete clear removes the skull before normal menu buffer
         * ownership resumes. CGRAM slot 0 was never touched. */
        if(lcd_clear_nonblock()){
            screensaver_Active=0u;
            exit_requested=0u;
            state=SKULL_DRAIN_MENU;
            menu_repaintAll(); /* invalidate both 16-column cached rows */
        }
        break;
    default:
        state=SKULL_EXIT_CLEAR;
        break;
    }
    return screensaver_Active;
}

void screensaver_check(void) {
    if(!activeParameters[PAR_SCREENSAVER_ON_OFF]) {
        if(screensaver_Active) {
            exit_requested=1u;
            screensaver_process();
        }
        return;
    }
    if(!screensaver_Active) {
        if(screensaver_timer>=screensaver_Timeout)enter();
    }
    if(screensaver_Active)screensaver_process();
}
