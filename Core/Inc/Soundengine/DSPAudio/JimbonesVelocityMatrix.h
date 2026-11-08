/* JB-16: bounded velocity route, independent of persistent kit layout. */
#ifndef JIMBONES_VELOCITY_MATRIX_H
#define JIMBONES_VELOCITY_MATRIX_H
#include <stdint.h>
static inline uint8_t jb_velocity_route(uint8_t base,int8_t depth,uint8_t velocity) {
 int delta=(int)depth*((int)velocity-64)/64;
 int v=(int)base+delta;
 return (uint8_t)(v<0?0:(v>127?127:v));
}
static inline uint8_t jb_velocity_route_voice(uint8_t voice,uint8_t mask,uint8_t base,int8_t depth,uint8_t velocity) {
 return voice<6u && (mask&(1u<<voice))?jb_velocity_route(base,depth,velocity):base;
}
#endif
