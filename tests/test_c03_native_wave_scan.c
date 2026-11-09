#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include "Soundengine/DSPAudio/JimbonesNativeWaveScan.h"
int main(void) {
    int16_t waves[4][256] = {{0}};
    for (unsigned i=0; i<256; ++i) {
        waves[0][i] = 0;
        waves[1][i] = 10000;
        waves[2][i] = -10000;
        waves[3][i] = 20000;
    }
    assert(jb_wave_scan_sample(waves, 0, 0.f) == 0);
    assert(jb_wave_scan_sample(waves, 0, 1.f) == 10000);
    assert(jb_wave_scan_sample(waves, 0, 1.5f) == 0);
    assert(jb_wave_scan_sample(waves, 0, 2.5f) == 5000);
    assert(jb_wave_scan_sample(waves, 0, -1.f) == 0);
    assert(jb_wave_scan_sample(waves, 0, 999.f) == 20000);
    assert(jb_wave_scan_sample(waves, 0, 0.f/0.f) == 0);
    puts("C03 native periodic wave scan kernel PASS (isolated host)");
    return 0;
}
