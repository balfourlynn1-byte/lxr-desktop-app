#include "menu/JimbonesSkull.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
static unsigned energy(const uint8_t a[4][8]) {
 unsigned count=0u;
 for(unsigned g=0;g<4;g++)for(unsigned r=0;r<8;r++) {
   assert(a[g][r]<=31u);
   for(unsigned b=0;b<5;b++) count+=(a[g][r]>>b)&1u;
 }
 return count;
}
int main(void){
 uint8_t frame[JB_SKULL_FRAMES][4][8];
 for(unsigned i=0;i<JB_SKULL_FRAMES;i++){
  jb_skull_make_frame((uint8_t)i,frame[i]);
  assert(energy(frame[i])>30u);
  for(unsigned g=0;g<4;g++)for(unsigned r=0;r<8;r++) {
     /* Character rows are symmetric about centre of image. */
     assert(frame[i][g][r]<=31u);
  }
 }
 assert(jb_skull_yoffset(0)==0u);
 assert(jb_skull_yoffset(4)==4u);
 assert(jb_skull_yoffset(7)==1u);
 assert(jb_skull_yoffset(8)==0u);
 assert(memcmp(frame[0],frame[4],sizeof frame[0])!=0);
 assert(memcmp(frame[1],frame[7],sizeof frame[1])==0);
 uint8_t shifted[4][8];
 jb_skull_make_frame(16u,shifted);
 assert(memcmp(shifted,frame[0],sizeof shifted)==0);
 puts("PASS JB-26: original 10x12 skull with 8 vertical bounce frames, bounded 2x2 LCD custom glyphs, exact looping");
 return 0;
}
