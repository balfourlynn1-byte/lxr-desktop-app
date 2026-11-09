/* Host-only JIMBONES SHIFT-relative delta proof of concept.
 * NOT wired to LXR hardware, UI, persistence or DSP. */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
static uint8_t add_clamped(uint8_t old, int delta, uint8_t lo, uint8_t hi) {
    int n = (int)old + delta;
    return (uint8_t)(n < lo ? lo : n > hi ? hi : n);
}
static void apply(uint8_t v[6], int delta, uint8_t mask, uint8_t lo, uint8_t hi) {
    for (int i = 0; i < 6; i++)
        if (mask & (1u << i)) v[i] = add_clamped(v[i], delta, lo, hi);
}
int main(void) {
    uint8_t a[6] = {20, 45, 60, 35, 75, 90};
    apply(a, 10, 63, 0, 100);
    const uint8_t expected[] = {30, 55, 70, 45, 85, 100};
    for (int i = 0; i < 6; i++) assert(a[i] == expected[i]);
    apply(a, -15, 0x3e, 0, 100); /* protect voice 1 */
    const uint8_t expected2[] = {30, 40, 55, 30, 70, 85};
    for (int i = 0; i < 6; i++) assert(a[i] == expected2[i]);
    assert(add_clamped(0, -127, 0, 127) == 0);
    assert(add_clamped(127, 127, 0, 127) == 127);
    assert(add_clamped(1, 120, 0, 100) == 100);
    assert(add_clamped(100, -120, 0, 100) == 0);
    puts("PASS: relative offsets, exclusions, positive/negative deltas, saturation");
    return 0;
}
