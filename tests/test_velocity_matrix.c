#include "Soundengine/DSPAudio/JimbonesVelocityMatrix.h"
#include <assert.h>
int main(void) {
 assert(jb_velocity_route(64,0,127)==64);
 assert(jb_velocity_route(64,64,127)>64);
 assert(jb_velocity_route(64,64,1)<64);
 assert(jb_velocity_route(0,-127,127)==0);
 assert(jb_velocity_route(127,127,127)==127);
 assert(jb_velocity_route_voice(0,0x3e,64,64,127)==64);
 assert(jb_velocity_route_voice(1,0x3e,64,64,127)>64);
 return 0;
}
