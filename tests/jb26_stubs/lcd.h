#ifndef JB26_LCD_MOCK_H
#define JB26_LCD_MOCK_H
#include <stdint.h>
uint32_t HAL_GetTick(void);
uint8_t lcd_clear_nonblock(void);
uint8_t lcd_generatechar_nonBlock(uint8_t code,const uint8_t *data);
uint8_t lcd_setcursor_nonBlock(uint8_t col,uint8_t row);
uint8_t lcd_data_nonBlock(uint8_t data);
#endif
