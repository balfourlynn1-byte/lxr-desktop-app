/* Production screensaver.c driven with mock LCD, no actual device claims. */
#include "lcd.h"
#include "menu/menu.h"
#include "menu/screensaver.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
uint8_t activeParameters[1]={1u};
static uint32_t now;
static unsigned flushed,clear_calls,glyph_calls,cursor_calls,data_calls,repains;
static uint8_t glyph_mask;
uint32_t HAL_GetTick(void){return now;}
uint8_t sendDisplayBuffer(void){
    flushed++;
    return flushed<5u?1u:0u;
}
void menu_repaintAll(void){repains++;}
uint8_t lcd_clear_nonblock(void){
    assert(flushed>=5u);clear_calls++;return 1u;
}
uint8_t lcd_generatechar_nonBlock(uint8_t code,const uint8_t *data){
    assert(clear_calls>0u);
    assert(code>=4u&&code<=7u);
    for(unsigned row=0;row<8u;row++)assert(data[row]<=31u);
    glyph_mask|=(uint8_t)(1u<<(code-4u));
    glyph_calls++;
    return 1u;
}
uint8_t lcd_setcursor_nonBlock(uint8_t col,uint8_t row){
    assert(col<=14u);assert(row==1u||row==2u);
    cursor_calls++;return 1u;
}
uint8_t lcd_data_nonBlock(uint8_t data){
    assert(data>=4u && data<=7u);
    data_calls++;return 1u;
}
int main(void){
    screensaver_timer=120000u;
    screensaver_check();
    assert(screensaver_Active==1u);
    assert(clear_calls==0u);
    for(unsigned i=0;i<120u;i++)screensaver_check();
    assert(flushed==5u); /* wait for in-progress normal menu transfer */
    assert(clear_calls==1u);
    assert(glyph_mask==0x0fu);
    assert(data_calls==4u && cursor_calls==2u);
    /* Subsequent vertical bounce rewrites CGRAM, not DDRAM, until drift. */
    now+=200u;
    for(unsigned i=0;i<100u;i++)screensaver_check();
    assert(glyph_calls>=8u);
    assert(data_calls==4u); /* no repeated full OLED redraw */
    screensaver_touch();
    assert(screensaver_Active); /* touch doesn't block waiting for LCD */
    for(unsigned i=0;i<60u && screensaver_Active;i++)screensaver_check();
    assert(!screensaver_Active);
    assert(repains==1u);
    assert(clear_calls==2u);
    assert(screensaver_timer==0u);
    /* Next idle activation should reuse a clean menu-to-skull handoff. */
    screensaver_timer=120000u;
    screensaver_check();
    assert(screensaver_Active);
    printf("PASS JB-26: normal menu drain -> 2x2 skull -> bouncing CGRAM -> nonblocking wake -> full redraw -> retrigger\n");
    return 0;
}
