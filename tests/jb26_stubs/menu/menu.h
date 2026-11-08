#ifndef JB26_MENU_MOCK_H
#define JB26_MENU_MOCK_H
#include <stdint.h>
#define PAR_SCREENSAVER_ON_OFF 0u
extern uint8_t activeParameters[1];
uint8_t sendDisplayBuffer(void);
void menu_repaintAll(void);
#endif
