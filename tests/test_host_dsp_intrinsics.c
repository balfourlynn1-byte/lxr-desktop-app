/* Known packed-lane saturation cases for the host CMSIS replacements. */
#include "host_dsp_intrinsics.h"

int main(void)
{
    if (__QADD16(0x7fff8000u, 0x0001ffffu) != 0x7fff8000u) return 1;
    if (__QADD16(0x00010002u, 0xfffefffeu) != 0xffff0000u) return 2;
    if (__QSUB16(0x80007fffu, 0x0001ffffu) != 0x80007fffu) return 3;
    if (__QSUB16(0x00010002u, 0xfffe0003u) != 0x0003ffffu) return 4;
    return 0;
}
