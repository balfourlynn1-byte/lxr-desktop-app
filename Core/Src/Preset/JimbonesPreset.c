/* Versioned, endian-independent optional kit trailer. SPDX-License-Identifier: GPL-3.0-only */
#include "Preset/JimbonesPreset.h"
#include <string.h>
static uint32_t checksum(const uint8_t *p,unsigned n) {
 uint32_t h=2166136261u; while(n--) h=(h^*p++)*16777619u; return h;
}
void jb_pack(uint8_t *out,const uint8_t *kit) {
 memcpy(out,"JB91",4); out[4]=1; out[5]=JB_VOICES; out[6]=JB_PARAMS; out[7]=0;
 memcpy(out+8,kit,JB_KIT_BYTES);
 uint32_t h=checksum(out,JB_RECORD_BYTES-4);
 for(unsigned i=0;i<4;i++) out[JB_RECORD_BYTES-4+i]=(uint8_t)(h>>(8*i));
}
int jb_unpack(uint8_t *kit,const uint8_t *in,unsigned size) {
 jb_defaults(kit);
 if(size!=JB_RECORD_BYTES || memcmp(in,"JB91",4) || in[4]!=1 ||
 in[5]!=JB_VOICES || in[6]!=JB_PARAMS || in[7]) return 0;
 uint32_t h=0; for(unsigned i=0;i<4;i++) h|=(uint32_t)in[JB_RECORD_BYTES-4+i]<<(8*i);
 if(h!=checksum(in,JB_RECORD_BYTES-4)) return 0;
 for(unsigned i=0;i<JB_KIT_BYTES;i++) if(in[8+i]>jb_limit(i%JB_PARAMS)) return 0;
 memcpy(kit,in+8,JB_KIT_BYTES); return 1;
}
