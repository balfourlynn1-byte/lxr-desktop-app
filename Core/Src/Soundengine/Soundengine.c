/*
 * Soundengine.c
 *
 *  Created on: Mar 24, 2020
 *      Author: julian
 *
 * Copyright (C) 2012-2026 Erica Synths
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * This file is part of the LXR firmware. It is licensed under the GNU General
 * Public License version 3, with the additional permission and terms in
 * NOTICE.md. See LICENSE and NOTICE.md in the root of the repository.
 */
#include "Soundengine/Soundengine.h"
#include "Soundengine/AudioCodecManager.h"
#include "Preset/PresetManager.h"
#include <string.h>
#include "Soundengine/USB/usb_manager.h"
#include "Soundengine/DSPAudio/Jimbones.h"
#include "Soundengine/StepFX.h"
#include "Soundengine/AudioDiagnostics.h"
void stepfx_audioBlock(void);
#include "Soundengine/MIDI/Uart.h"

/* Debugger telemetry measures rendering, not the complete ISR or MIDI load. */
volatile uint32_t jb_audioMaxCycles, jb_audioLastCycles, jb_audioOverruns;
volatile uint8_t jb_audioLoadPercent, jb_audioPeakPercent;
uint8_t soundengine_calcNextSampleBlock()
{
	if(bCurrentSampleValid!= SAMPLE_VALID)
	{
#ifndef LXR_EMU
        uint32_t cycleStart=DWT->CYCCNT;
#endif
		stepfx_audioBlock();
	#if USE_DAC2
		mixer_calcNextSampleBlock((int16_t*)&dma_buffer[bCurrentSampleValid*(OUTPUT_DMA_SIZE*2)],(int16_t*)&dma_buffer2[bCurrentSampleValid*(OUTPUT_DMA_SIZE*2)]);
	#else
		mixer_calcNextSampleBlock(&dma_buffer[bCurrentSampleValid*(OUTPUT_DMA_SIZE*2)],&dma_buffer2[(1-bCurrentSampleValid)*(OUTPUT_DMA_SIZE*2)]);
	#endif
#ifndef LXR_EMU
        jb_audioLastCycles=DWT->CYCCNT-cycleStart;
        if(jb_audioLastCycles>jb_audioMaxCycles)jb_audioMaxCycles=jb_audioLastCycles;
        /* Audio render-only budget; no claim to include other ISR and MIDI work. */
        const uint32_t budget=(uint32_t)((double)SystemCoreClock*OUTPUT_DMA_SIZE/REAL_FS);
        if(budget) {
            const uint32_t load=(uint32_t)(((uint64_t)jb_audioLastCycles*100u)/budget);
            jb_audioLoadPercent=(uint8_t)(load>255u?255u:load);
            jb_audioLoadSample(load);
            if(jb_audioLoadPercent>jb_audioPeakPercent)jb_audioPeakPercent=jb_audioLoadPercent;
            if(jb_audioLastCycles>budget)jb_audioOverruns++;
        }
#endif
		bCurrentSampleValid = SAMPLE_VALID;

		return 1;
	}

	return 0;
}

void soundengine_init()
{
#ifndef LXR_EMU
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
#endif
	stepfx_init();
	jb_init();
	mixer_init();



	// start the audio codec
//#ifndef DEBUG
	CodecInit(SYNTH_FS);
//#endif

	parameterArray_init();

	euklid_init();

	int i;
	for(i=0;i<6;i++)
	{
		modNode_init(&velocityModulators[i]);
	}

	//init the seq speed
	seq_init();
	seq_setBpm(120);

	memset(midi_MidiChannels,0,8);
//	memset(midi_NoteOverride,0,7);

	initDrumVoice();
	Snare_init();
	HiHat_init();
	Cymbal_init();

	//precalc the first dma buffer block
	soundengine_calcNextSampleBlock();

	FPU->FPDSCR |= FPU_FPDSCR_FZ_Msk ; //enable flush to zero
}

void soundengine_process()
{
#if CALC_SAMPLES_IN_INTERRUPT
	__HAL_TIM_DISABLE_IT(&htim2, TIM_IT_UPDATE);
	//HAL_TIM_Base_Stop_IT(&htim2);
	seq_tick();
	soundengine_calcNextSampleBlock();
	__HAL_TIM_SetCounter(&htim2, 1950);
	//HAL_TIM_Base_Start_IT(&htim2);
	__HAL_TIM_ENABLE_IT(&htim2, TIM_IT_UPDATE);
#endif
	//generate next sample if no valid sample is present
#if !CALC_SAMPLES_IN_INTERRUPT
#if 1//#ifndef DEBUG
		seq_tick();
		soundengine_calcNextSampleBlock();



#endif
#endif
		//process midi on midi port
		uart_processMidi();

		//process the sequencer
	//	seq_tick(); //moved to tim 14?
	usb_tick();
	//process incoming frontpanel data
		//	uart_processFront();






	preset_morph_tick();
	//preset_morph_tick();
	//preset_morph_tick();

}
