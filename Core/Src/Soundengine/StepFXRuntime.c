/* SPDX-License-Identifier: GPL-3.0-only */
#include "Soundengine/StepFX.h"
#include "Soundengine/Soundengine.h"
#include "Soundengine/Sequencer/sequencer.h"
static void emit(uint8_t v,uint8_t note,uint8_t vel){
 if(seq_isTrackMuted(v))return;
 if(v<3)Drum_trigger(v,vel,note);else if(v==3)Snare_trigger(vel,note);
 else if(v==4)Cymbal_trigger(vel,note);else HiHat_trigger(vel,v==6,note);
}
void stepfx_audioBlock(void){stepfx_process(OUTPUT_DMA_SIZE,seq_getBpm(),seq_getExtSync(),emit);}
