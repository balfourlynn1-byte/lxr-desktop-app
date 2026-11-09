#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include "Soundengine/DSPAudio/JimbonesNativeWaveScan.h"
int main(void) {
    int16_t waves[4][256] = {{0}};
    for (unsigned i = 0; i < 256; ++i) {
        waves[0][i] = 0; waves[1][i] = 10000;
        waves[2][i] = 20000; waves[3][i] = 30000;
    }
    const uint8_t velocities[] = {20,50,80,110,127};
    int16_t last = -1;
    for (unsigned i = 0; i < sizeof(velocities); ++i) {
        int16_t value = jb_wave_scan_velocity_sample(waves, 0, 0.f, 3.f, velocities[i]);
        assert(value > last);
        last = value;
    }
    assert(jb_wave_scan_velocity_sample(waves,0,1.5f,0.f,20) ==
           jb_wave_scan_velocity_sample(waves,0,1.5f,0.f,127));
    assert(jb_wave_scan_velocity_sample(waves,0,1.5f,0.f,20) ==
           jb_wave_scan_sample(waves,0,1.5f));
    assert(jb_wave_velocity_position(2.5f,3.f,127) == 3.f);
    assert(jb_wave_velocity_position(0.5f,-3.f,127) == 0.f);
    assert(jb_wave_velocity_position(1.f,0.f,127) == 1.f);
    puts("C04 PASS: note-on velocity mapping 20/50/80/110/127, OFF bypass, bounds");
    return 0;
}
