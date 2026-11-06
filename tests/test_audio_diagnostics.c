#include "Soundengine/AudioDiagnostics.h"
#include <stdint.h>
#include <assert.h>
#include <stdio.h>
volatile uint32_t jb_audioMaxCycles, jb_audioLastCycles, jb_audioOverruns;
volatile uint8_t jb_audioLoadPercent, jb_audioPeakPercent;
volatile uint32_t jb_layerSaturationHits;
int main(void) {
 int16_t a[128]={0},b[128]={0};
 a[0]=32759; a[1]=-32759; a[2]=32760; a[3]=-32760;
 b[0]=32767; b[1]=-32768; b[2]=5;
 jb_audioDiagScan(a,b,64);
 assert(jb_audioNearRailHits==4);
 jb_audioDiagScan(a,b,64);
 assert(jb_audioNearRailHits==8);
 jb_audioLastCycles=101000;
 jb_audioMaxCycles=221000;
 jb_audioLoadPercent=51;
 jb_audioPeakPercent=90;
 jb_audioOverruns=7;
 jb_layerSaturationHits=99;
 jb_audioDiagReset();
 assert(jb_audioLastCycles==101000 && jb_audioLoadPercent==51);
 assert(jb_audioMaxCycles==0 && jb_audioPeakPercent==0);
 assert(jb_audioOverruns==0 && jb_layerSaturationHits==0);
 assert(jb_audioNearRailHits==0);
 puts("PASS: four-channel near-rail counter + reset; current DSP load preserved");
}
