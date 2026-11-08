#include "Soundengine/DSPAudio/JimbonesSampleRateReducer.h"
#include <assert.h>
int main(void){JbSampleHold s;jb_sample_hold_reset(&s);
 for(unsigned i=0;i<256;i++)assert(jb_sample_hold_process(&s,(float)i,1)==(float)i);
 jb_sample_hold_reset(&s);
 assert(jb_sample_hold_process(&s,1.f,4)==1.f);
 assert(jb_sample_hold_process(&s,2.f,4)==1.f);
 assert(jb_sample_hold_process(&s,3.f,4)==1.f);
 assert(jb_sample_hold_process(&s,4.f,4)==1.f);
 assert(jb_sample_hold_process(&s,5.f,4)==5.f);
 return 0;}
