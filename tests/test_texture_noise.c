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
 for(unsigned mode=1;mode<=2;mode++) {
  sound(mode,0,dry);sound(mode,127,textured);
  double delta=0,reference=0;
  for(unsigned i=0;i<22050;i++) { double d=textured[i]-dry[i];delta+=d*d;reference+=(double)dry[i]*dry[i]; }
  double ratio=sqrt(delta/reference);printf("Texture %s relative RMS change: %.3f\n",mode==1?"metal":"wave",ratio);
  if(ratio<.7) failed=1;
 }
 jb_init();jb_parameters[0][JB_MODE]=1;jb_parameters[0][JB_NOISE]=127;
 jb_parameters[0][JB_COLOUR]=64;jb_parameters[0][JB_DECAY]=127;
 jb_trigger(0,127,48,0); double correlation=0,power=0;int16_t last=0;
 for(unsigned i=0;i<22050;i++) { int16_t n=0;jb_render(0,&n,1,1); if(i>500) {correlation+=(double)last*n;power+=(double)n*n;}last=n; }
 double lag=correlation/power;printf("Full-noise, neutral-colour lag-1 correlation: %.4f\n",lag);
 if(fabs(lag)>.05) failed=1;
 return failed;
}
