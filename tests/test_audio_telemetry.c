#include "Soundengine/DSPAudio/JimbonesAudioTelemetry.h"
#include <assert.h>
#include <stdio.h>
int main(void) {
 jb_audio_telemetry m;
 jb_audio_telemetry_init(&m,216000000u,96u,44100u);
 assert(m.budget_cycles==470204u); /* floor(216M*96/44100) */
 jb_audio_telemetry_record(&m,0xfffffff0u,0x10u);
 assert(m.last_cycles==32 && m.blocks==1 && m.over_budget==0);
 jb_audio_telemetry_record(&m,0,500000u);
 assert(m.peak_cycles==500000 && m.over_budget==1 && jb_audio_telemetry_load(&m)==100);
 jb_audio_telemetry_queue(&m,2,2);
 jb_audio_telemetry_queue(&m,1,2);
 jb_audio_telemetry_queue(&m,0,2);
 assert(m.peak_pressure_percent==100 && m.pressure_sum==150 && m.pressure_samples==3);
 jb_audio_telemetry_init(&m,0,0,0);
 assert(m.budget_cycles==0 && jb_audio_telemetry_load(&m)==0);
 puts("PASS cycle wrap, deadline, peak, queue pressure and zero budget");
 return 0;
}
