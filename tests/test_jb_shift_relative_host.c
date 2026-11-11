/* Host test for the firmware's signed, saturating Shift edit helper. */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include "menu/JimbonesRelativeEdit.h"

int main(void) {
    uint8_t a[6] = {20, 45, 60, 35, 75, 90};
    for (int i = 0; i < 6; ++i) a[i] = jb_relativeEditClamp127(a[i], 10);
    const uint8_t expected[] = {30, 55, 70, 45, 85, 100};
    for (int i = 0; i < 6; i++) assert(a[i] == expected[i]);
    for (int i = 0; i < 6; ++i) a[i] = jb_relativeEditClamp127(a[i], -15);
    const uint8_t expected2[] = {15, 40, 55, 30, 70, 85};
    for (int i = 0; i < 6; i++) assert(a[i] == expected2[i]);
    assert(jb_relativeEditClamp127(0, -127) == 0);
    assert(jb_relativeEditClamp127(127, 127) == 127);
    assert(jb_relativeEditClamp127(1, 120) == 121);
    assert(jb_relativeEditClamp127(100, -120) == 0);
    puts("PASS: firmware Shift delta preserves voice offsets and saturates at 0/127");
    return 0;
}
