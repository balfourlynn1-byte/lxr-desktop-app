#include "Soundengine/DSPAudio/JimbonesTransientShaper.h"
#include <assert.h>
#include <math.h>
int main(void) {
 JbTransientState a,b,c; jb_transient_reset(&a);jb_transient_reset(&b);jb_transient_reset(&c);
 float boost=0.f,cut=0.f,baseline=0.f;
 for(unsigned i=0;i<4096;i++) {
  const float x=(i%128<32)?.5f:0.f;
  float y0=jb_transient_process(&a,x,0.f,0.f);
  float y1=jb_transient_process(&b,x,1.f,0.f);
  float y2=jb_transient_process(&c,x,-1.f,0.f);
  assert(y0==x); assert(isfinite(y1)&&isfinite(y2));
  if(i%128==0) {baseline+=fabsf(y0);boost+=fabsf(y1);cut+=fabsf(y2);}
 }
 assert(boost>baseline);assert(cut<baseline);
 jb_transient_reset(&a);assert(a.fast==0.f && a.slow==0.f);
 return 0;
}
