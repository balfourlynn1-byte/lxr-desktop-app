/* Versioned, endian-independent optional kit trailer. SPDX-License-Identifier: GPL-3.0-only */
#include "Preset/JimbonesPreset.h"
#include <string.h>
static uint32_t checksum(const uint8_t *p,unsigned n) {
 uint32_t h=2166136261u; while(n--) h=(h^*p++)*16777619u; return h;
}
void jb_pack(uint8_t *out,const uint8_t *kit) {
 memcpy(out,"JB91",4); out[4]=2; out[5]=JB_VOICES; out[6]=JB_PARAMS; out[7]=0;
 memcpy(out+8,kit,JB_KIT_BYTES);
 uint32_t h=checksum(out,JB_RECORD_BYTES-4);
 for(unsigned i=0;i<4;i++) out[JB_RECORD_BYTES-4+i]=(uint8_t)(h>>(8*i));
}
int jb_unpack(uint8_t *kit,const uint8_t *in,unsigned size) {
 jb_defaults(kit);
 if(size<12 || memcmp(in,"JB91",4) || in[5]!=JB_VOICES || in[7]) return 0;
 unsigned stride=in[6];
 if(!((in[4]==1 && stride==28) || (in[4]==2 && stride==JB_PARAMS))) return 0;
 if(size!=12+stride*JB_VOICES) return 0;
 uint32_t h=0; for(unsigned i=0;i<4;i++) h|=(uint32_t)in[size-4+i]<<(8*i);
 if(h!=checksum(in,size-4)) return 0;
 for(unsigned i=0;i<stride*JB_VOICES;i++) if(in[8+i]>jb_limit(i%stride)) return 0;
 for(unsigned v=0;v<JB_VOICES;v++) memcpy(kit+v*JB_PARAMS,in+8+v*stride,stride);
 return 1;
}
