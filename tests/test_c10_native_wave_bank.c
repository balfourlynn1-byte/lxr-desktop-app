#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include "Soundengine/DSPAudio/JimbonesNativeWaveBank.h"
#include "Soundengine/DSPAudio/JimbonesNativeWaveScan.h"
int main(void) {
    for (unsigned w=0;w<4;w++) {
        int min=32767,max=-32768;
        for(unsigned i=0;i<256;i++) {
            int v=jb_builtinWaveBank[w][i];
            if(v<min)min=v;
            if(v>max)max=v;
            assert(v>=-12000 && v<=12000);
        }
        assert(min<0 && max>0);
    }
    assert(jb_wave_scan_sample(jb_builtinWaveBank,0x40000000u,0.f)!=
           jb_wave_scan_sample(jb_builtinWaveBank,0x40000000u,1.f));
    puts("PASS C10: immutable built-in wave bank linked and native scan reads it");
    return 0;
}
