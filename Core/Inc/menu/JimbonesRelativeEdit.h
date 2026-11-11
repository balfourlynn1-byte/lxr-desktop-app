/* Signed, saturating delta for the six-voice Shift edit. */
#ifndef JIMBONES_RELATIVE_EDIT_H
#define JIMBONES_RELATIVE_EDIT_H

#include <stdint.h>

static inline uint8_t jb_relativeEditClamp127(uint8_t oldValue, int8_t delta)
{
    int value = (int)oldValue + (int)delta;
    if (value < 0) value = 0;
    if (value > 127) value = 127;
    return (uint8_t)value;
}

#endif
