/* JIMBONES WaveShape isolated-layer sound demos: no copyrighted source/audio. */
#include "Soundengine/DSPAudio/Jimbones.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define FS 44140u
#define SAMPLES (FS*2u)
static int16_t audio[SAMPLES];
static void u16(FILE *f,uint16_t x) {fputc(x&255,f);fputc(x>>8,f);}
static void u32(FILE *f,uint32_t x) {for(unsigned j=0;j<4;j++)fputc((x>>(j*8))&255,f);}
static int wav(const char *path) {
 FILE *f=fopen(path,"wb");if(!f){perror(path);return 1;}
 const uint32_t sz=SAMPLES*2u;
 fwrite("RIFF",1,4,f);u32(f,36u+sz);
 fwrite("WAVEfmt ",1,8,f);u32(f,16);u16(f,1);u16(f,1);
 u32(f,FS);u32(f,FS*2u);u16(f,2);u16(f,16);
 fwrite("data",1,4,f);u32(f,sz);
 for(unsigned i=0;i<SAMPLES;i++)u16(f,(uint16_t)audio[i]);
 return fclose(f)!=0;
}
static void render(unsigned mode,unsigned shape,unsigned aggressive) {
 jb_init();
 jb_parameters[1][JB_MODE]=mode;
 jb_parameters[1][JB_CHARACTER]=shape;
 jb_parameters[1][JB_AGGRESSION]=aggressive;
 jb_parameters[1][JB_WAVE_LEVEL]=112;
 jb_parameters[1][JB_METAL_LEVEL]=102;
 jb_parameters[1][JB_DECAY]=104;
 jb_parameters[1][JB_PITCH]=50;
 jb_parameters[1][JB_POSITION]=64;
 jb_parameters[1][JB_MOVEMENT]=40;
 jb_parameters[1][JB_TEXTURE]=10;
 jb_parameters[1][JB_DRIVE]=0;
 jb_parameters[1][JB_BITS]=0;
 jb_trigger(1,116,48,0);
 for(unsigned i=0;i<SAMPLES;i++) {
  audio[i]=0;
  jb_render(1,&audio[i],1,.7f);
 }
}
int main(int argc,char **argv) {
 if(argc!=2){fprintf(stderr,"usage: %s DIRECTORY\n",argv[0]);return 2;}
 const struct {unsigned mode,shape,aggressive;const char *file;} cases[]={
   {2,0,0,"wave_neutral.wav"},
   {2,127,0,"wave_phase_max.wav"},
   {2,127,127,"wave_phase_and_fold.wav"},
   {3,0,0,"both_neutral.wav"},
   {3,127,0,"both_phase_max.wav"},
   {3,127,127,"both_phase_and_fold.wav"} };
 for(unsigned i=0;i<sizeof(cases)/sizeof(cases[0]);i++){
  char file[1024];snprintf(file,sizeof(file),"%s/%s",argv[1],cases[i].file);
  render(cases[i].mode,cases[i].shape,cases[i].aggressive);
  if(wav(file))return 1;
  printf("Rendered %s (mode %u, shape %u, aggression %u)\n",
    file,cases[i].mode,cases[i].shape,cases[i].aggressive);
 }
 return 0;
}
