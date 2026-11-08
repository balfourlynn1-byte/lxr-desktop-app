/* JB-20 HOST STUB ONLY: mirror minimal fields of production DelayStruct.
 * Never included by firmware CMake, only via -Itests/host for tests.
 */
#ifndef JB20_HOST_DELAY_H
#define JB20_HOST_DELAY_H
#include <stdint.h>
#define FX_BUFFER_SIZE 0x7FFFu
enum DelayTypesEnum {DELAY_NORMAL,DELAY_PINGPONG};
typedef struct DelayStruct {
    uint32_t write,oldRead,newRead;
    float interpValue,delayTime,range,feedback;
    uint8_t timePending,delayType;
} Delay;
void Delay_setRange(Delay*,float);
void Delay_setDelayTime(Delay*,float);
void Delay_setFeedback(Delay*,float);
#endif
