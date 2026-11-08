#include "Soundengine/DSPAudio/Jimbones.h"
#include <math.h>
#include <stdio.h>
static int16_t dry[22050],textured[22050];
static void sound(unsigned mode,unsigned texture,int16_t *out) {
 jb_init(); jb_parameters[0][JB_MODE]=mode; jb_parameters[0][JB_TEXTURE]=texture;
 jb_parameters[0][JB_DECAY]=127; jb_trigger(0,127,48,0);
 for(unsigned i=0;i<22050;i++) { out[i]=0; jb_render(0,out+i,1,1); }
}
int main(void) {
 int failed=0;
 for(unsigned mode=2;mode<=3;mode++) {
  sound(mode,0,dry);sound(mode,127,textured);
  double delta=0,reference=0;
  for(unsigned i=0;i<22050;i++) { double d=textured[i]-dry[i];delta+=d*d;reference+=(double)dry[i]*dry[i]; }
  double ratio=sqrt(delta/reference);printf("Texture %s relative RMS change: %.3f\n",mode==2?"wave":"legacy-both-now-wave",ratio);
  if(ratio<.7) failed=1;
 }
 /* New noise must exist before the native SVF, even with no metallic wet layer. */
 jb_init();jb_parameters[0][JB_MODE]=2;jb_parameters[0][JB_NOISE]=127;
 jb_parameters[0][JB_COLOUR]=64;jb_parameters[0][JB_DECAY]=127;
 jb_trigger(0,127,48,0);
 double power=0; int16_t last=0; unsigned changes=0;
 for(unsigned i=0;i<22050;i++) {
  int16_t n=0;
  jb_preFilterNoise(0,&n,1);
  if(i>500) { power+=(double)n*n; if(n!=last) changes++; }
  last=n;
 }
 printf("Pre-filter noise power %.0f, transitions %u\\n",power,changes);
 if(power<1000000.0 || changes<1000) failed=1;
 jb_parameters[0][JB_MODE]=1; /* legacy FM-only now no prefilter noise */
 int16_t buffer=12345; jb_preFilterNoise(0,&buffer,1);
 if(buffer!=12345) failed=1;
 return failed;
}
