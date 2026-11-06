/* Regression: an interrupted long layer must not add the new attack to a stale sample. */
#include "Soundengine/DSPAudio/Jimbones.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
int main(void) {
 int worst=0;
 for(unsigned v=0;v<6;v++) for(unsigned mode=1;mode<=3;mode++) {
  jb_init(); jb_parameters[v][JB_MODE]=mode;
  jb_parameters[v][JB_DECAY]=127; jb_parameters[v][JB_POSITION]=127;
  jb_parameters[v][JB_NOISE_LEVEL]=100;
  int16_t last=0;
  for(unsigned hit=0;hit<100;hit++) {
   jb_trigger(v,20+hit%108,36+hit%60,1);
   int16_t first=0; jb_render(v,&first,1,1);
   int jump=abs((int)first-last); if(jump>worst) worst=jump;
   for(unsigned n=0;n<71;n++) { last=0; jb_render(v,&last,1,1); }
  }
 }
 printf("Largest retrigger boundary jump: %d / 32768\n",worst);
 return worst>2;
}
