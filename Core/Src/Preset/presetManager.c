/*
 * presetManager.c
 *
 * Created: 09.05.2012 16:06:19
 *  Author: Julian
 *
 * Copyright (C) 2012-2026 Erica Synths
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * This file is part of the LXR firmware. It is licensed under the GNU General
 * Public License version 3, with the additional permission and terms in
 * NOTICE.md. See LICENSE and NOTICE.md in the root of the repository.
 */ 
//#include "../config.h"
#include <frontPanelParser_ui.h>
#include "Preset/PresetManager.h"
//#include "../Hardware/SD/ff.h"
#include "ff.h"
#include <stdio.h>
#include "menu/CcNr2Text.h"
#include "menu/menu.h"
//#include <util/delay.h>
#include "lcd.h"
//#include <avr/pgmspace.h>
#include <stdlib.h>
//#include <util/atomic.h>
#include "IO/uart.h"
#include "IO/DIO.h"

#include "Soundengine/MIDI/MidiParser.h"
#include "Soundengine/Sequencer/sequencer.h"
#include "Soundengine/Soundengine.h"




//#include "../Hardware/timebase.h"

//FATFS SDFatFS;		/* File system object for the logical drive */
//FIL SDFile;		/* place to hold 1 file*/
static uint8_t morph_state = 0; //flag for non blocking morph
static uint8_t morph_index = 0;
char savefileNames[NUM_KIT_SLOTS][NUM_SAVEFILE_NAMES][8];

char currentFolder[13];
static FRESULT preset_recoverProjectFolder(uint8_t presetNr);
static uint8_t preset_projectDirectoryValid(const char *folder);

//char presetNameBuffer[8];
//char currentName[8];
//uint8_t activePresetNr = 0;

#include "Preset/JimbonesPreset.h"
static uint8_t jb_kits[NUM_KIT_SLOTS][JB_KIT_BYTES];

#define FILE_VERSION 2

/*
#define NUM_TRACKS 7
#define NUM_PATTERN 8*/
#define STEPS_PER_PATTERN NUM_STEPS //64


#define FEXT_SOUND 	0
#define FEXT_PAT 	1
#define FEXT_ALL 	2
#define FEXT_PROJ 	3
#define FEXT_SONG 	4



// fill buffer (length 9 total) with a filename. type is one of above
// eg p001.snd
static void preset_makeFileNameOld(char *buf, uint8_t num, uint8_t type);

//load uses wildcard to find a name of the pattern 00-*.ext
static FRESULT preset_makeFileNameLoad(char *buf, uint8_t num, uint8_t type);
//save uses the kit name to create a filename eg 00-NAME.ext
static void preset_makeFileNameSave(char *buf, uint8_t num, uint8_t type, char *name);

//static void preset_makeFileNameTemp(char *buf, uint8_t num, uint8_t type);


#define PRESET_SAVE_TIME 2000

uint32_t preset_kitTimer = 0;
uint32_t preset_patTimer = 0;

uint8_t preset_kitDirty = 0;
uint8_t preset_patDirty = 0;

void preset_initTmpFolder()
{
	char folderName[13];
	sprintf(folderName, "/tmp");

	FRESULT res;
	FILINFO fno;

	res = f_stat(folderName, &fno);
	if(res == FR_NO_FILE)
	{
		res = f_mkdir(folderName);
		if(res == FR_OK)
		{
			f_chdir(folderName);
		}
	}
	else
	{
		f_chdir(folderName);
	}
}

void preset_resetAllSaveTimers()
{
	preset_kitDirty = 0;
	preset_patDirty = 0;
}

FRESULT loadTmpFolderFiles()
{
#if USE_INTERRUPT_ONLY_DURING_SAVE_PROJ
	HAL_TIM_Base_Start_IT(&htim2);	//calc sample interrupt - prio 1
#else

#endif

	FRESULT res;
	//check tmp folder for *.snd and *.pat files and load them to RAM
	for(uint8_t i=0;i<NUM_KIT_SLOTS;i++)
	{

		//FR_NO_FILE just means an incomplete project and is no reason to abort!
		res = preset_loadDrumsetSD(i, 0);
		if ((res != FR_OK) && (res != FR_NO_FILE))
		{
			lcd_process_display_interrupt = 1;
			preset_initTmpFolder();
			f_close((FIL*)&SDFile);
#if USE_INTERRUPT_ONLY_DURING_SAVE_PROJ
			HAL_TIM_Base_Stop_IT(&htim2);	//calc sample interrupt - prio 1
#else

#endif
			return res;
		}
		res = preset_loadPatternSD(i);
		if ((res != FR_OK) && (res != FR_NO_FILE))
		{
			f_close((FIL*)&SDFile);
			lcd_process_display_interrupt = 1;
			preset_initTmpFolder();
#if USE_INTERRUPT_ONLY_DURING_SAVE_PROJ
			HAL_TIM_Base_Stop_IT(&htim2);	//calc sample interrupt - prio 1
#else

#endif
			return res;
		}
		res = preset_loadSongSD(i);
		if ((res != FR_OK) && (res != FR_NO_FILE))
		{
			f_close((FIL*)&SDFile);
			lcd_process_display_interrupt = 1;
			preset_initTmpFolder();
#if USE_INTERRUPT_ONLY_DURING_SAVE_PROJ
			HAL_TIM_Base_Stop_IT(&htim2);	//calc sample interrupt - prio 1
#else

#endif
			return res;
		}
	}

	return FR_OK;
}

void preset_touchTimer(PresetTimer type)
{
	if(type==PRESET_KIT_TIMER)
	{
		preset_kitTimer = HAL_GetTick();
		preset_kitDirty = 1;
	}
	else if(type==PRESET_PAT_TIMER)
	{
		preset_patTimer = HAL_GetTick();
		preset_patDirty = 1;
	}
	else if(type==PRESET_STOP_PAT_TIMER)
	{
		//abort pattern save timer, since seq is running again...
		preset_patDirty = 0;
	}

};


FRESULT preset_saveTmpKit()
{
#if USE_INTERRUPT_ONLY_DURING_SAVE_PROJ
	HAL_TIM_Base_Start_IT(&htim2);	//calc sample interrupt - prio 1
#else

#endif
	FRESULT res;
	unsigned int bytesWritten;

	//open the file
	res = f_open((FIL*)&SDFile,"tmp.snd",FA_CREATE_ALWAYS | FA_WRITE);
	if(res != FR_OK)
	{
#if USE_INTERRUPT_ONLY_DURING_SAVE_PROJ
			HAL_TIM_Base_Stop_IT(&htim2);	//calc sample interrupt - prio 1
#else

#endif
		return res;
	}

	//write current preset nr to file
	res = f_write((FIL*)&SDFile,(void*)&menu_selectedPresetNr[SAVE_TYPE_KIT],1,&bytesWritten);
	if(res != FR_OK)
	{
		f_close((FIL*)&SDFile);
#if USE_INTERRUPT_ONLY_DURING_SAVE_PROJ
			HAL_TIM_Base_Stop_IT(&htim2);	//calc sample interrupt - prio 1
#else

#endif
		return res;
	}
	//write the preset data
	f_write((FIL*)&SDFile,activeParameters,END_OF_SOUND_PARAMETERS,&bytesWritten);
 uint8_t record[JB_RECORD_BYTES];
 jb_pack(record,&jb_parameters[0][0]);
 res=f_write((FIL*)&SDFile,record,sizeof(record),&bytesWritten);
 if(res==FR_OK && bytesWritten!=sizeof(record)) res=FR_DISK_ERR;

	//close the file
	f_close((FIL*)&SDFile);
#if USE_INTERRUPT_ONLY_DURING_SAVE_PROJ
			HAL_TIM_Base_Stop_IT(&htim2);	//calc sample interrupt - prio 1
#else

#endif
	return res;
}

FRESULT preset_LoadTmpKit()
{
 uint8_t kit[JB_KIT_BYTES]; jb_defaults(kit); jb_loadKit(kit);
	FRESULT res;
	unsigned int bytesRead;
	uint8_t kitNr = 0;

	//open the file
	res = f_open((FIL*)&SDFile,"tmp.snd",FA_OPEN_EXISTING | FA_READ);
	if(res != FR_OK) return res;

	//first the preset nr
	res=f_read((FIL*)&SDFile,(void*)&kitNr,1,&bytesRead);
	if(bytesRead != 1) {
		res=FR_DISK_ERR;
		goto closeFile;
	}

	if(kitNr>=NUM_KIT_SLOTS) { res=FR_DISK_ERR; goto closeFile; }
	menu_selectedPresetNr[SAVE_TYPE_KIT] = kitNr;


	//read the file content
		bytesRead=1;
		int16_t i;
		uint8_t *para;
		bytesRead = 0;

		para=activeParameters;

		res=f_read((FIL*)&SDFile, para,END_OF_SOUND_PARAMETERS, &bytesRead);
  if(res==FR_OK) {
   uint8_t record[JB_RECORD_BYTES]; UINT got=0;
   FRESULT extra=f_read((FIL*)&SDFile,record,sizeof(record),&got);
   if(extra==FR_OK) { jb_unpack(kit,record,got); jb_loadKit(kit); }
   else res=extra;
  }
		if(res==FR_OK) {
			// set to 0 for any that were not read from the file
			if(END_OF_SOUND_PARAMETERS-bytesRead)
				memset(para+bytesRead,0,END_OF_SOUND_PARAMETERS-bytesRead);

			//special case mod targets. normalize.
			const uint8_t nmt=getNumModTargets();
			for(i=0;i<6;i++) {
				// since I've changed the meaning of these, I'll ensure that it's valid for kits saved prior to the
				// change
				if(para[PAR_VEL_DEST_1+i] >= nmt )
					para[PAR_VEL_DEST_1+i] = 0;
				if(para[PAR_TARGET_LFO1+i] >= nmt )
					para[PAR_TARGET_LFO1+i] = 0;
			}
		}


	closeFile:
		//close the file handle
		f_close((FIL*)&SDFile);

		// update the soundengine with the new parameters
		if(res==FR_OK) {
			preset_sendDrumsetParameters();
			return FR_OK;
		}

		return res;
}

FRESULT preset_LoadTmpPat()
{

	uint8_t success=1; // start off succeeding
#if USE_INTERRUPT_ONLY_DURING_SAVE_PROJ
	HAL_TIM_Base_Start_IT(&htim2);	//calc sample interrupt - prio 1
#else

#endif
	//filename in 8.3  format
		//uint8_t fileVersion = 1;
		UINT bytesRead;
		FRESULT res;



		res = f_open((FIL*)&SDFile,"tmp.pat",FA_OPEN_EXISTING | FA_READ);
		if(res != FR_OK)
		{
		#if USE_INTERRUPT_ONLY_DURING_SAVE_PROJ
					HAL_TIM_Base_Stop_IT(&htim2);	//calc sample interrupt - prio 1
		#else

		#endif
				return res;
		}

		//read current preset nr
		res = f_read((FIL*)&SDFile,(void*)&menu_playedPattern,1,&bytesRead);
		if(bytesRead != 1)
		{
			success = 0;
			goto closeFile;
		}

		if(menu_playedPattern >= NUM_PATTERN)
		{
			menu_playedPattern = 0;
			success = 0;
			goto closeFile;
		}

		menu_shownPattern = menu_playedPattern;
		seq_activePattern = menu_playedPattern;
		seq_pendingPattern = menu_playedPattern;
		menu_selectedPresetNr[SAVE_TYPE_PATTERN] = menu_playedPattern;

		frontPanel_sendData(SEQ_CC,SEQ_SET_SHOWN_PATTERN,menu_playedPattern);

		seq_copyPatternToBackupBuffer(seq_activePattern);
		//frontParser_shownPattern = menu_playedPattern;
		uint8_t patternNr = menu_playedPattern;

	//---------------- read pattern data -----------------------------

		// note that the pattern length data is no longer stored in the same way.
		// This means that loading old patterns with non-standard length, the length will be
		// ignored and a standard length used. I think it's acceptable as long as users are aware of this.

		uint8_t next=0;

		// ---------- Load step data first

		// load step data



		for(uint8_t trk=0; trk<NUM_TRACKS;trk++)
		{

			f_read((FIL*)&SDFile,(void*)&seq_patternSet.seq_subStepPattern[patternNr][trk][0],sizeof(Step)*STEPS_PER_PATTERN,&bytesRead);

			if(bytesRead!=sizeof(Step)*(STEPS_PER_PATTERN))
			{
				success=0;
				break; // file error, or we ran out of file (eof)
			}

			//Pattern V3 and up => flam added
			//if(dataVersion > 2)
			{
				f_read((FIL*)&SDFile,(void*)&seq_patternSet.seq_stepTiming[patternNr][trk][0],sizeof(uint8_t)*STEPS_PER_PATTERN,&bytesRead);

				if(bytesRead!=sizeof(uint8_t)*(STEPS_PER_PATTERN))
				{
					success=0;
					break; // file error, or we ran out of file (eof)
				}
			}

		}



		// ---------- load the main step data next
		uint16_t mainStepData;


			for(uint8_t trk=0; trk<NUM_TRACKS;trk++)
			{
				for(uint8_t bar=0;bar<NUM_BARS; bar++)
				{
					f_read((FIL*)&SDFile,(void*)&mainStepData,sizeof(uint16_t),&bytesRead);

					if( bytesRead==sizeof(uint16_t)) {
						seq_patternSet.seq_mainSteps[bar][patternNr][trk] = mainStepData;
					}
					 else {
							success=0;
							break;
						}
				}
			}



		// ------------ shuffle settings
		if(success) {
			//load the shuffle settings
			f_read((FIL*)&SDFile,(void*)&seq_patternSet.shuffle[patternNr],sizeof(uint8_t),&bytesRead);
			//and update on cortex
			if(bytesRead)
			{

				//activeParameters[PAR_SHUFFLE] = seq_patternSet.shuffle[patternNr];
				//frontPanel_sendData(SEQ_CC,SEQ_SHUFFLE,parameter_values[PAR_SHUFFLE]);

				//seq_setShuffle(activeParameters[PAR_SHUFFLE]/127.f);
			}
			else {
				success=0;
			}
		}

		// ----------- Pattern/track lengths
		// -- AS this might not exist in the saved data file, we still want success
		if(success) {

				for(uint8_t trk=0; trk<NUM_TRACKS;trk++)
				{
					f_read((FIL*)&SDFile,(void*)&next,sizeof(uint8_t),&bytesRead);
					seq_patternSet.length[patternNr][trk] = next;
				}

			success=1; // just to document that we don't consider this a failure, we just didn't read the bytes for this
		}

		//if(dataVersion > 1)
		{
			f_read((FIL*)&SDFile,(void*)&seq_patternSet.kitNr[patternNr],sizeof(uint8_t),&bytesRead);
			if(bytesRead == 0) success = 0;

		}

		if( success == 0)
		{


			f_close((FIL*)&SDFile);
#if USE_INTERRUPT_ONLY_DURING_SAVE_PROJ
			HAL_TIM_Base_Stop_IT(&htim2);	//calc sample interrupt - prio 1
#else

#endif
			return FR_INT_ERR;
		}

		success=1;
		//close the file handle



	closeFile:
		f_close((FIL*)&SDFile);

		#if USE_INTERRUPT_ONLY_DURING_SAVE_PROJ
					HAL_TIM_Base_Stop_IT(&htim2);	//calc sample interrupt - prio 1
		#else

		#endif


		// return success or failure
		if(success) return FR_OK;
		else return FR_INT_ERR;


}

FRESULT preset_saveTmpPat()
{

	//don't save invalid pattern
	if(menu_playedPattern >= NUM_PATTERN) return FR_INVALID_PARAMETER;
#if USE_INTERRUPT_ONLY_DURING_SAVE_PROJ
	HAL_TIM_Base_Start_IT(&htim2);	//calc sample interrupt - prio 1
#else

#endif

	FRESULT res;
	UINT bytesWritten;
	uint8_t presetNr = menu_playedPattern;
	//lcd_clear();
	//lcd_home();
	//lcd_string(PSTR("Saving pattern"));

	//open the file
	res = f_open((FIL*)&SDFile,"tmp.pat",FA_CREATE_ALWAYS | FA_WRITE);
	if(res != FR_OK)
	{
#if USE_INTERRUPT_ONLY_DURING_SAVE_PROJ
			HAL_TIM_Base_Stop_IT(&htim2);	//calc sample interrupt - prio 1
#else

#endif
		return res;
	}

	//write current preset nr to file
	res = f_write((FIL*)&SDFile,(void*)&menu_playedPattern,1,&bytesWritten);
	if(res != FR_OK)
	{
		f_close((FIL*)&SDFile);
#if USE_INTERRUPT_ONLY_DURING_SAVE_PROJ
			HAL_TIM_Base_Stop_IT(&htim2);	//calc sample interrupt - prio 1
#else

#endif
		return res;
	}

	//write pattern data
	//preset_writePatternData(presetNr);
#if 0
#else

		uint8_t length;

		//---------------  write the step data (note, vol, prob, autom.) ---------------
		for(uint8_t trk=0; trk<NUM_TRACKS;trk++)
		{
			f_write((FIL*)&SDFile,(const void*)&seq_patternSet.seq_subStepPattern[presetNr][trk][0],sizeof(Step)*STEPS_PER_PATTERN,&bytesWritten);

			f_write((FIL*)&SDFile,(const void*)&seq_patternSet.seq_stepTiming[presetNr][trk][0],sizeof(uint8_t)*STEPS_PER_PATTERN,&bytesWritten);
		}

		//--------------- now the main step data ---------------
		for(uint8_t trk=0; trk<NUM_TRACKS;trk++)
		{
			for(uint8_t bar=0;bar<NUM_BARS; bar++)
			{
				const uint16_t mainStepData = seq_patternSet.seq_mainSteps[bar][presetNr][trk];
				f_write((FIL*)&SDFile,(const void*)&mainStepData,sizeof(uint16_t),&bytesWritten);
			}
		}

		//----- shuffle setting ------


		//f_write((FIL*)&SDFile,(const void*)&activeParameters[PAR_SHUFFLE],sizeof(uint8_t),&bytesWritten);
		f_write((FIL*)&SDFile,(const void*)&seq_patternSet.shuffle[presetNr],sizeof(uint8_t),&bytesWritten);

		//----- pattern/track lengths ------
		// we reuse the same call from above (when saving main step data)
		// but we only use the length info retrieved. We want to store it at the end
		// to avoid breaking compatibility with save file


		for(uint8_t trk=0; trk<NUM_TRACKS;trk++)
		{
				length = seq_patternSet.length[presetNr][trk] ;//seq_patternSet.seq_mainSteps[bar][pat][trk];
				f_write((FIL*)&SDFile,(const void*)&length,sizeof(uint8_t),&bytesWritten);
		}

		//write associated kit nr
		f_write((FIL*)&SDFile,&seq_patternSet.kitNr[presetNr],sizeof(uint8_t),&bytesWritten);

#endif

	//close the file
	f_close((FIL*)&SDFile);

	//reset the lcd
//	menu_repaintAll();

#if USE_INTERRUPT_ONLY_DURING_SAVE_PROJ
			HAL_TIM_Base_Stop_IT(&htim2);	//calc sample interrupt - prio 1
#else

#endif

	return res;
}

/*------------------------------------------------------------/
/ Delete a sub-directory even if it contains any file
/-------------------------------------------------------------/
/ The delete_node() function is for R0.12+.
/ It works regardless of FF_FS_RPATH.
*/


FRESULT delete_node (
    TCHAR* path,    /* Path name buffer with the sub-directory to delete */
    UINT sz_buff,   /* Size of path name buffer (items) */
    FILINFO* fno    /* Name read buffer */
)
{
    UINT i, j;
    FRESULT fr;
    DIR dir;


    fr = f_opendir(&dir, path); /* Open the sub-directory to make it empty */
    if (fr != FR_OK) return fr;

    for (i = 0; path[i]; i++) ; /* Get current path length */
    path[i++] = _T('/');

    for (;;) {
        fr = f_readdir(&dir, fno);  /* Get a directory item */
        if (fr != FR_OK || !fno->fname[0]) break;   /* End of directory? */
        j = 0;
        do {    /* Make a path name */
            if (i + j >= sz_buff) { /* Buffer over flow? */
                fr = 100; break;    /* Fails with 100 when buffer overflow */
            }
            path[i + j] = fno->fname[j];
        } while (fno->fname[j++]);
        if (fno->fattrib & AM_DIR) {    /* Item is a sub-directory */
            fr = delete_node(path, sz_buff, fno);
        } else {                        /* Item is a file */
            fr = f_unlink(path);
        }
        if (fr != FR_OK) break;
    }

    path[--i] = 0;  /* Restore the path name */
    f_closedir(&dir);

    if (fr == FR_OK) fr = f_unlink(path);  /* Delete the empty sub-directory */
    return fr;
}

FRESULT preset_clearTmpFolder()
{
	FILINFO fno;
	FRESULT fr;
	TCHAR buff[256];

	f_chdir("/");

	/* Directory to be deleted */
	sprintf(buff, "/tmp");

	fr = delete_node(buff, sizeof buff / sizeof buff[0], &fno);

	preset_initTmpFolder();

	return fr;
}


void preset_processTimer()
{
	if(preset_patDirty == 1)
	{
		if(HAL_GetTick()-preset_patTimer >= PRESET_SAVE_TIME )
		{
			preset_patDirty = 0;
			preset_saveTmpPat();
		}
	}

	if(preset_kitDirty == 1)
	{
		if(HAL_GetTick()-preset_kitTimer >= PRESET_SAVE_TIME )
		{
			preset_kitDirty = 0;
			preset_saveTmpKit();
		}
	}
}


/*------------------------------------------------------------/
/ Remove all contents of a directory
/ This function works regardless of FF_FS_RPATH.
/------------------------------------------------------------*/




static FRESULT empty_directory(char *path, size_t capacity)
{
	FILINFO fno;
    size_t base = strlen(path);
    FRESULT fr;
    DIR dir;
	if (base + 2 > capacity) return FR_INVALID_NAME;

    fr = f_opendir(&dir, path);
    if (fr == FR_OK) {
        path[base++] = '/';
        for (;;) {
            fr = f_readdir(&dir, &fno);
            if (fr != FR_OK || !fno.fname[0]) break;
            if (_FS_RPATH && fno.fname[0] == '.') continue;
            size_t length = strlen(fno.fname);
            if (base + length + 1 > capacity) {
                fr = FR_INVALID_NAME;
                break;
            }
            memcpy(path + base, fno.fname, length + 1);
            if (fno.fattrib & AM_DIR) {
                fr = empty_directory(path, capacity);
                if (fr != FR_OK) break;
            }
            fr = f_unlink(path);
            if (fr != FR_OK) break;
        }
        path[base - 1] = '\0';
        f_closedir(&dir);
    }

    return fr;
}
//----------------------------------

uint8_t preset_isNameEmpty(char* name)
{
	uint8_t isEmpty = 1;

	if( (name[0] != 'E') && (name[0] != 0) ) isEmpty = 0;
	if( (name[1] != 'm') && (name[1] != 0) ) isEmpty = 0;
	if( (name[2] != 'p') && (name[2] != 0) ) isEmpty = 0;
	if( (name[3] != 't') && (name[3] != 0) ) isEmpty = 0;
	if( (name[4] != 'y') && (name[4] != 0) ) isEmpty = 0;
	if( (name[5] != 0) ) isEmpty = 0;
	if( (name[6] != 0) ) isEmpty = 0;
	if( (name[7] != 0) ) isEmpty = 0;


	return isEmpty;
}
//----------------------------------------------------
//load init kit
void preset_init(uint8_t presetNr)
{
	jb_defaults(jb_kits[presetNr]);
	jb_loadKit(jb_kits[presetNr]);

	//const uint8_t presetNr = menu_selectedPresetNr[SAVE_TYPE_KIT];

	//first the preset name
	memcpy(savefileNames[presetNr][KIT_NAMES],initKit,8*sizeof(uint8_t));


	if(preset_isNameEmpty(savefileNames[presetNr][KIT_NAMES]))
	{
		sprintf((void*)savefileNames[presetNr][KIT_NAMES], "Empty");
	}


	//then the data
	//read the file content

	int16_t i;
	uint8_t *para;


	para=mainParameters[presetNr];

	for(int i=8;i<END_OF_SOUND_PARAMETERS; i++)
	{
		para[i-8] = initKit[i];
	}

	//special case mod targets. normalize.
	const uint8_t nmt=getNumModTargets();
	for(i=0;i<6;i++) {
		// since I've changed the meaning of these, I'll ensure that it's valid for kits saved prior to the
		// change
		if(para[PAR_VEL_DEST_1+i] >= nmt )
			para[PAR_VEL_DEST_1+i] = 0;
		if(para[PAR_TARGET_LFO1+i] >= nmt )
			para[PAR_TARGET_LFO1+i] = 0;
	}

	//copy parameters to edit buffer
	memcpy(activeParameters, mainParameters[presetNr], sizeof(uint8_t) * END_OF_SOUND_PARAMETERS);
	preset_sendDrumsetParameters();
	//frontPanel_sendData(PRESET,PRESET_LOAD,presetNr);

}

//----------------------------------------------------
static FRESULT preset_writeChecked(const void *data, UINT size)
{
	UINT written = 0;
	FRESULT res = f_write((FIL*)&SDFile, data, size, &written);
	return res == FR_OK && written != size ? FR_DISK_ERR : res;
}

static FRESULT preset_writeDrumsetData(uint8_t presetNr, uint8_t isMorph)
{
	uint16_t i;

	if(isMorph)
	{
		for(i=0;i<END_OF_SOUND_PARAMETERS;i++)
		{
			uint8_t value;
			//Mod targets are not morphed!!!
			if( (i >= PAR_VEL_DEST_1) && (i <= PAR_VEL_DEST_6) )
			{
				value = activeParameters[i];
			} else if( (i >= PAR_TARGET_LFO1) && (i <= PAR_TARGET_LFO6) )
			{
				value = activeParameters[i];
			} else if( (i >= PAR_VOICE_LFO1) && (i <= PAR_VOICE_LFO6) )
			{
				value = activeParameters[i];
			} 
			else 
			{
				value = preset_getMorphValue(i,activeParameters[PAR_MORPH]);
			}					
			FRESULT res = preset_writeChecked(&value, 1);
			if (res != FR_OK) return res;
		}
	} else {
		for(i=0;i<END_OF_SOUND_PARAMETERS;i++)
		{
			FRESULT res = preset_writeChecked(&mainParameters[presetNr][i], 1);
			if (res != FR_OK) return res;
		}
	}
	return FR_OK;
}

//----------------------------------------------------
void preset_loadDrumsetRam(uint8_t presetNr, uint8_t isMorph, uint8_t sendImmediate)
{
	soundengine_process();
	if(isMorph)
	{
		morphParameters = mainParameters[presetNr];
	}
	else
	{
		//copy parameters to edit buffer
		memcpy(activeParameters, mainParameters[presetNr], sizeof(uint8_t) * END_OF_SOUND_PARAMETERS);
		jb_loadKit(jb_kits[presetNr]);
		//if(activeParameters[PAR_PATTERN_MODE] = 1)
		//load asociated morph kit as well

		// morphParameters = mainParameters[activeParameters[PAR_MORPH_KIT_DEST]];


	}
	soundengine_process();

	if(sendImmediate)
	{
		preset_sendDrumsetParametersImmediate();
	}
	else
	{
		//activePresetNr = menu_currentPresetNr[menu_saveOptions.category];
		preset_sendDrumsetParameters();
	}
}
//----------------------------------------------------
void preset_saveDrumsetRam(uint8_t presetNr, uint8_t isMorph)
{
	/* A morph-target save must not overwrite the native JIMBONES kit
	 * extension belonging to that slot. Only a normal kit save owns it. */
	if(isMorph)
	{
		for(uint16_t i=0;i<END_OF_SOUND_PARAMETERS;i++)
		{
			uint8_t value;
			//Mod targets are not morphed!!!
			if( (i >= PAR_VEL_DEST_1) && (i <= PAR_VEL_DEST_6) )
			{
				value = activeParameters[i];
			} else if( (i >= PAR_TARGET_LFO1) && (i <= PAR_TARGET_LFO6) )
			{
				value = activeParameters[i];
			} else if( (i >= PAR_VOICE_LFO1) && (i <= PAR_VOICE_LFO6) )
			{
				value = activeParameters[i];
			}
			else
			{
				value = preset_getMorphValue(i,activeParameters[PAR_MORPH]);
			}
			//f_write((FIL*)&SDFile,&value,1,&bytesWritten);
			mainParameters[presetNr][i] = value;
		}
	}
	else
	{
		memcpy(jb_kits[presetNr],jb_parameters,JB_KIT_BYTES);
		//copy edit buffer to parameter array
		memcpy(mainParameters[presetNr], activeParameters, sizeof(uint8_t) * END_OF_SOUND_PARAMETERS);
	}
}
//----------------------------------------------------
FRESULT preset_saveDrumsetSD(uint8_t presetNr, uint8_t isMorph)
{
#if USE_SD_CARD
	FRESULT res;
	//filename in 8.3  format
	char filename[13];
	preset_makeFileNameSave(filename,presetNr,FEXT_SOUND, savefileNames[presetNr][KIT_NAMES]);
	//sprintf(filename,"p%03d.snd",presetNr);



	//open the file
	res = f_open((FIL*)&SDFile,filename,FA_CREATE_ALWAYS | FA_WRITE);
	if(res != FR_OK) return res;
	
	//write preset name to file

	res = preset_writeChecked(savefileNames[presetNr][KIT_NAMES], 8);
	if(res != FR_OK) { f_close((FIL*)&SDFile); return res; }
	//write the preset data
	res = preset_writeDrumsetData(presetNr, isMorph);
	if(res != FR_OK) { f_close((FIL*)&SDFile); return res; }
 uint8_t record[JB_RECORD_BYTES];
 jb_pack(record,isMorph?&jb_parameters[0][0]:jb_kits[presetNr]);
 res = preset_writeChecked(record, sizeof(record));
 if(res != FR_OK) {
  f_close((FIL*)&SDFile); return res;
 }
	
	//close the file
	return f_close((FIL*)&SDFile);
#else
#endif
};

//----------------------------------------------------
static void preset_writeGlobalData()
{
	unsigned int bytesWritten;
	int i;
	for(i=PAR_BEGINNING_OF_GLOBALS;(i<NUM_PARAMS);i++)
	{
		f_write((FIL*)&SDFile,&activeParameters[i],1,&bytesWritten);
	}
}

//----------------------------------------------------
/* The first bytes remain the legacy globals format. New saves append a CRC32
 * so a same-length damaged primary can be rejected in favour of glo.bak. */
static uint32_t preset_globalsCrc(const uint8_t *data, UINT length)
{
	uint32_t crc = 0xffffffffUL;
	for (UINT i = 0; i < length; ++i) {
		crc ^= data[i];
		for (uint8_t bit = 0; bit < 8; ++bit)
			crc = (crc >> 1) ^ (crc & 1 ? 0xedb88320UL : 0);
	}
	return ~crc;
}

static uint8_t preset_globalsSizeValid(uint32_t size)
{
	const uint32_t legacy = NUM_PARAMS - PAR_BEGINNING_OF_GLOBALS;
	return size == legacy || size == legacy + sizeof(uint32_t);
}

static uint8_t preset_globalsFileValid(const char *path)
{
	FIL file;
	if (f_open(&file, path, FA_READ) != FR_OK) return 0;
	UINT size = f_size(&file), read = 0;
	uint8_t data[NUM_PARAMS - PAR_BEGINNING_OF_GLOBALS + sizeof(uint32_t)];
	uint8_t valid = preset_globalsSizeValid(size) &&
	                f_read(&file, data, size, &read) == FR_OK && read == size;
	if (f_close(&file) != FR_OK) valid = 0;
	if (valid && size > NUM_PARAMS - PAR_BEGINNING_OF_GLOBALS) {
		uint32_t stored;
		memcpy(&stored, data + NUM_PARAMS - PAR_BEGINNING_OF_GLOBALS, sizeof stored);
		valid = stored == preset_globalsCrc(data, NUM_PARAMS - PAR_BEGINNING_OF_GLOBALS);
	}
	return valid;
}

FRESULT preset_saveGlobals()
{
#if USE_SD_CARD
	FRESULT res;
	FRESULT closeRes;
	UINT written = 0;
	uint8_t globals[NUM_PARAMS - PAR_BEGINNING_OF_GLOBALS + sizeof(uint32_t)];
	memcpy(globals, &activeParameters[PAR_BEGINNING_OF_GLOBALS],
	       NUM_PARAMS - PAR_BEGINNING_OF_GLOBALS);
	uint32_t crc = preset_globalsCrc(globals, NUM_PARAMS - PAR_BEGINNING_OF_GLOBALS);
	memcpy(globals + NUM_PARAMS - PAR_BEGINNING_OF_GLOBALS, &crc, sizeof crc);
	FILINFO info;
	char path[sizeof(currentFolder) + sizeof("/glo.cfg")];
	char staged[sizeof(currentFolder) + sizeof("/glo.new")];
	char backup[sizeof(currentFolder) + sizeof("/glo.bak")];
/*
	res = f_mount((FATFS*)&SDFatFS, (TCHAR const*)SDPath,1);
	if( res != FR_OK)
	{
		return res;
	}
	*/
	/* Commit only after the new globals have been written and synced. Keep
	 * the previous file as a recovery copy across a failed rename or reset. */
	snprintf(path, sizeof path, "%s/glo.cfg", currentFolder);
	snprintf(staged, sizeof staged, "%s/glo.new", currentFolder);
	snprintf(backup, sizeof backup, "%s/glo.bak", currentFolder);
	res = f_open((FIL*)&SDFile,staged,FA_CREATE_ALWAYS | FA_WRITE);
	if(res != FR_OK) return res;
	res = f_write((FIL*)&SDFile, globals, sizeof globals, &written);
	if (res == FR_OK && written != sizeof globals)
		res = FR_DISK_ERR;
	if (res == FR_OK) res = f_sync((FIL*)&SDFile);
	closeRes = f_close((FIL*)&SDFile);
	if (res != FR_OK) return res;
	if (closeRes != FR_OK) return closeRes;

	/* Never remove the only valid copy after a prior interrupted commit. */
	res = f_stat(path, &info);
	if (res == FR_OK) {
		if (!preset_globalsFileValid(path)) {
			/* The primary failed its CRC. Keep the valid backup across a
			 * failed promotion instead of replacing it with corrupt data. */
			if (!preset_globalsFileValid(backup)) return FR_DISK_ERR;
			res = f_unlink(path);
			if (res != FR_OK) return res;
		} else {
			FRESULT backupRes = f_stat(backup, &info);
			if (backupRes == FR_OK) {
				res = f_unlink(backup);
				if (res != FR_OK) return res;
			} else if (backupRes != FR_NO_FILE) return backupRes;
			res = f_rename(path, backup);
			if (res != FR_OK) return res;
		}
	} else if (res != FR_NO_FILE) return res;
	return f_rename(staged, path);
#else
#endif	
}
//----------------------------------------------------
// returns 1 on success
static uint8_t preset_readGlobalData()
{
#if USE_SD_CARD

	UINT bytesRead;
#if 0
	int i;
	for(i=PAR_BEGINNING_OF_GLOBALS;(i<NUM_PARAMS) &&( bytesRead!=0);i++) {
		f_read((FIL*)&SDFile,&activeParameters[i],1,&bytesRead);
		if(!bytesRead)
			return 0;
	}
	if(!bytesRead)
		return 0;
	return 1;
#else
	const UINT numGlobals = NUM_PARAMS - PAR_BEGINNING_OF_GLOBALS;
	uint8_t globals[NUM_PARAMS - PAR_BEGINNING_OF_GLOBALS + sizeof(uint32_t)];
	UINT length = f_size((FIL*)&SDFile);
	if (!preset_globalsSizeValid(length) ||
	    f_read((FIL*)&SDFile, globals, length, &bytesRead) != FR_OK ||
	    bytesRead != length) return 0;
	if (length > numGlobals) {
		uint32_t stored;
		memcpy(&stored, globals + numGlobals, sizeof stored);
		if (stored != preset_globalsCrc(globals, numGlobals)) return 0;
	}
	memcpy(&activeParameters[PAR_BEGINNING_OF_GLOBALS], globals, numGlobals);
	return 1;

#endif
#endif
}
//----------------------------------------------------
uint8_t preset_loadGlobals()
{


	FRESULT res;
	char path[sizeof(currentFolder) + sizeof("/glo.cfg")];
	char backup[sizeof(currentFolder) + sizeof("/glo.bak")];
	if (strlen(currentFolder) == 7 && !strncmp(currentFolder, "/proj", 5) &&
	    currentFolder[5] >= '0' && currentFolder[5] <= '9' &&
	    currentFolder[6] >= '0' && currentFolder[6] <= '9') {
		uint8_t project = (uint8_t)((currentFolder[5] - '0') * 10 + currentFolder[6] - '0');
		if (preset_recoverProjectFolder(project) != FR_OK) return 0;
		/* A new/legacy card may hold only globals before its first project save. */
		char metadata[sizeof(currentFolder) + sizeof("/prj.nfo")];
		FILINFO info;
		snprintf(metadata, sizeof metadata, "%s/prj.nfo", currentFolder);
		FRESULT found = f_stat(metadata, &info);
		if (found == FR_OK && !preset_projectDirectoryValid(currentFolder)) return 0;
		if (found != FR_OK && found != FR_NO_FILE) return 0;
	}
/*
	res = f_mount((FATFS*)&SDFatFS, (TCHAR const*)SDPath,1);
	if( res != FR_OK)
	{
		return res;
	}
	*/

	//open the file
	snprintf(path, sizeof path, "%s/glo.cfg", currentFolder);
	snprintf(backup, sizeof backup, "%s/glo.bak", currentFolder);
	res = f_open((FIL*)&SDFile,path,FA_OPEN_EXISTING | FA_READ);
	if (res == FR_OK) {
		uint8_t valid = preset_readGlobalData();
		f_close((FIL*)&SDFile);
		if (valid) {
			menu_sendAllGlobals();
			return 1;
		}
	}
	/* A missing or incomplete primary can follow a reset during rename. */
	res = f_open((FIL*)&SDFile,backup,FA_OPEN_EXISTING | FA_READ);
	if (res != FR_OK) return 0;
	uint8_t valid = preset_readGlobalData();
	f_close((FIL*)&SDFile);
	if (!valid) return 0;
	menu_sendAllGlobals();
	
	//f_unmount((TCHAR const*)SDPath);

	return 1;

}




//----------------------------------------------------
void preset_loadDefaultGlobals()
{
	activeParameters[PAR_BPM] = 120;

	activeParameters[PAR_MIDI_CHAN_1] = 1;
	activeParameters[PAR_MIDI_CHAN_2] = 1;
	activeParameters[PAR_MIDI_CHAN_3] = 1;
	activeParameters[PAR_MIDI_CHAN_4] = 1;
	activeParameters[PAR_MIDI_CHAN_5] = 1;
	activeParameters[PAR_MIDI_CHAN_6] = 1;
	activeParameters[PAR_MIDI_CHAN_7] = 1;

	activeParameters[PAR_MIDI_NOTE1] = 48;
	activeParameters[PAR_MIDI_NOTE2] = 48;
	activeParameters[PAR_MIDI_NOTE3] = 48;
	activeParameters[PAR_MIDI_NOTE4] = 48;
	activeParameters[PAR_MIDI_NOTE5] = 48;
	activeParameters[PAR_MIDI_NOTE6] = 48;
	activeParameters[PAR_MIDI_NOTE7] = 48;


	activeParameters[PAR_FETCH] = 0;
	activeParameters[PAR_FOLLOW] = 0;

	activeParameters[PAR_QUANTISATION] = 0;

	activeParameters[PAR_SCREENSAVER_ON_OFF] = 1;

	activeParameters[PAR_MIDI_ROUTING] = 0;
	activeParameters[PAR_MIDI_FILT_TX] = 15;	//all
	activeParameters[PAR_MIDI_FILT_RX] = 15; //all

	activeParameters[PAR_PRESCALER_CLOCK_IN] = 0; //all
	activeParameters[PAR_PRESCALER_CLOCK_OUT1] = 0; //all
	activeParameters[PAR_PRESCALER_CLOCK_OUT2] = 0; //all
	activeParameters[PAR_TRIG_GATE_MODE] = 0; //all

	activeParameters[PAR_BAR_RESET_MODE] = 1; //all
	activeParameters[PAR_MIDI_CHAN_GLOBAL] = 0; //all

	activeParameters[PAR_PATTERN_MODE] = 1; //auto

}
//----------------------------------------------------
void preset_createGlobals()
{
	preset_loadDefaultGlobals();
	preset_saveGlobals();
}
//----------------------------------------------------
FRESULT preset_loadLastUsedProjectNrFromSD(uint8_t* projNr)
{
    FRESULT res;

    *projNr = 0;
    unsigned int bytesRead;

    res = f_open((FIL*)&SDFile,"/proj.idx",FA_OPEN_EXISTING | FA_READ);
    if(res != FR_OK)
    {
    	//close the file handle
    	f_close((FIL*)&SDFile);
    	return res;
    }

    res=f_read((FIL*)&SDFile, projNr,1, &bytesRead);
	//close the file handle
	f_close((FIL*)&SDFile);


	return res;
}
//----------------------------------------------------
FRESULT preset_saveLastUsedProjectNrToSD(uint8_t projNr)
{
#if USE_SD_CARD
    FRESULT res;

	//open the file
	res = f_open((FIL*)&SDFile,"/proj.idx",FA_CREATE_ALWAYS | FA_WRITE);
	if(res != FR_OK)
	{
		//close the file handle
		f_close((FIL*)&SDFile);
		return res;
	}

	unsigned int bytesWritten;
	res = f_write((FIL*)&SDFile,&projNr,1,&bytesWritten);
	if(res != FR_OK)
	{
		//close the file handle
		f_close((FIL*)&SDFile);
		return res;
	}

	//close the file
	f_close((FIL*)&SDFile);

	//f_unmount((TCHAR const*)SDPath);
	return res;
#else
#endif
}
//----------------------------------------------------
// read the data into either the main parameters or the morph parameters.
// fix some values, and set values to 0 that were not read in
static FRESULT preset_readDrumsetData(uint8_t presetNr, uint8_t isMorph)
{
	
#if USE_SD_CARD		
	//read the file content
	unsigned int bytesRead=1;
	int16_t i;
	uint8_t *para;
	bytesRead = 0;
	
	// TODO check: using this function for morph may be wrong
	if(isMorph)
		para=morphParameters;
	else
		para=mainParameters[presetNr];

	FRESULT res=f_read((FIL*)&SDFile, para,END_OF_SOUND_PARAMETERS, &bytesRead);
	if(res==FR_OK) {
		// set to 0 for any that were not read from the file
		if(END_OF_SOUND_PARAMETERS-bytesRead)
			memset(para+bytesRead,0,END_OF_SOUND_PARAMETERS-bytesRead);
		
		//special case mod targets. normalize.
		const uint8_t nmt=getNumModTargets();
		for(i=0;i<6;i++) {
			// since I've changed the meaning of these, I'll ensure that it's valid for kits saved prior to the
			// change
			if(para[PAR_VEL_DEST_1+i] >= nmt )
				para[PAR_VEL_DEST_1+i] = 0;
			if(para[PAR_TARGET_LFO1+i] >= nmt )
				para[PAR_TARGET_LFO1+i] = 0;
		}
	}

	return res;

#endif
}

//----------------------------------------------------
FRESULT preset_loadDrumsetSD(uint8_t presetNr, uint8_t isMorph)
{
	if(!isMorph) jb_defaults(jb_kits[presetNr]);
	//activePresetNr = presetNr;
#if USE_SD_CARD
	//filename in 8.3  format
	char filename[13];
	unsigned int bytesRead;

#if !CALC_SAMPLES_IN_INTERRUPT
	while(!soundengine_calcNextSampleBlock())
	{
		//wait till new block is calculated
	}
#endif

	//sprintf(filename,"p%03d.snd",presetNr);
	/*
	preset_makeFileNameLoad(filename,presetNr,FEXT_SOUND);

	//open the file
	FRESULT res = f_open((FIL*)&SDFile,filename,FA_OPEN_EXISTING | FA_READ);
	*/
	FRESULT res = preset_makeFileNameLoad(filename,presetNr,FEXT_SOUND);
	if(res!=FR_OK)
	{
		//try old filename
		preset_makeFileNameOld(filename,presetNr,FEXT_SOUND);
		res = f_open((FIL*)&SDFile,filename,FA_OPEN_EXISTING | FA_READ);
		if(res!=FR_OK)
			goto error; //file open error... maybe the file does not exist?
	}

#if !CALC_SAMPLES_IN_INTERRUPT
	//to prevent audio dropouts
	soundengine_calcNextSampleBlock();
#endif
	//first the preset name
	res=f_read((FIL*)&SDFile,(void*)savefileNames[presetNr][KIT_NAMES],8,&bytesRead);
	if(bytesRead != 8) {
		res=FR_DISK_ERR;
		goto closeFile;
	}
	if(preset_isNameEmpty(savefileNames[presetNr][KIT_NAMES]))
	{
		sprintf((void*)savefileNames[presetNr][KIT_NAMES], "Empty");
	}



#if !CALC_SAMPLES_IN_INTERRUPT
	//to prevent audio dropouts
	soundengine_calcNextSampleBlock();
#endif
	//then the data
	res=preset_readDrumsetData(presetNr, isMorph);
 if(res==FR_OK && !isMorph) {
  uint8_t record[JB_RECORD_BYTES]; UINT got=0;
  FRESULT extra=f_read((FIL*)&SDFile,record,sizeof(record),&got);
  if(extra==FR_OK) jb_unpack(jb_kits[presetNr],record,got);
  else res=extra;
 }
#if !CALC_SAMPLES_IN_INTERRUPT
	//to prevent audio dropouts
	soundengine_calcNextSampleBlock();
#endif
closeFile:
	//close the file handle
	f_close((FIL*)&SDFile);

	// update the soundengine with the new parameters
	if(res==FR_OK) {
		preset_sendDrumsetParameters();
		return FR_OK;
	}
error:
	return res;
#else
	frontPanel_sendData(PRESET,PRESET_LOAD,presetNr);
#endif
}
//----------------------------------------------------
//val is interpolation value between 0 and 255
//uses bankers rounding
static uint8_t interpolate(uint8_t a, uint8_t b, uint8_t x)
{
	uint16_t fixedPointValue = (uint16_t)(((a*256) + (b-a)*x));
	uint8_t result = (uint8_t)(fixedPointValue/256);

	return (uint8_t)((fixedPointValue&0xff) < 0x7f ? result : result+1);
	//return ((a*255) + (b-a)*x)/255;
}

//----------------------------------------------------
IN_ITCM_RAM void preset_sendDrumsetParametersImmediate()
{
		//special case mod targets
		for(uint8_t i=0;i<6;i++)
		{
			//**VELO load drumkit. translate to param value before sending
			// parameter_values[PAR_VEL_DEST_1+i] is an index into modTargets, we need to send
			// a parameter number
			uint8_t value = (uint8_t)pgm_read_word(&modTargets[activeParameters[PAR_VEL_DEST_1+i]].param);
			uint8_t upper,lower;
			upper = (uint8_t)(((value&0x80)>>7) | (((i)&0x3f)<<1));
			lower = value&0x7f;
			frontPanel_sendData(CC_VELO_TARGET,upper,lower);

			// ensure target voice # is valid
			if(activeParameters[PAR_VOICE_LFO1+i] < 1 || activeParameters[PAR_VOICE_LFO1+i] > 6 )
				activeParameters[PAR_VOICE_LFO1+i]=1;

			// **LFO par_target_lfo will be an index into modTargets, but we need a parameter number to send
			value = (uint8_t)pgm_read_word(&modTargets[activeParameters[PAR_TARGET_LFO1+i]].param);

			upper = (uint8_t)(((value&0x80)>>7) | (((i)&0x3f)<<1));
			lower = value&0x7f;
			frontPanel_sendData(CC_LFO_TARGET,upper,lower);
		}

		static MidiMsg msg = {0};
		//pre-send fx type to prevent clicks
		/*
		msg.status = CC_2;
		msg.data1 = PAR_FX_TYPE-128; //stupid offset only in CC not CC2
		msg.data2 = activeParameters[PAR_FX_TYPE];

		//	msg.data1 += 1;
		msg.data1 &= 0x7f;
		midiParser_ccHandler(msg,1, 0, 0.f);
		*/

		//try to prevent click by sending BD params first

		//stuff from the morph function
		for(uint32_t i=0;i<END_OF_SOUND_PARAMETERS; i++)
		{

			uint8_t paramNumber = parameterOrder[i];

			//const uint8_t morph = activeParameters[PAR_MORPH];

			const uint8_t val = activeParameters[paramNumber];//interpolate(activeParameters[i],morphParameters[i],morph);

			if(paramNumber < 128)
			{
				/* comes from cortex midi handler
				 * stupid offset!
				 * TODO fix this stupid offset			 *
				 * 		frontParser_midiMsg.data1 += 1;

						//because hh slope on front is 127 and on cortex is 0 wrap data1 at 127
						frontParser_midiMsg.data1 &= 0x7f;
				 *
				 */
				msg.status = MIDI_CC;
				msg.data1 = paramNumber;
				msg.data2 = val;

				msg.data1 += 1;
				msg.data1 &= 0x7f;
				midiParser_ccHandler(msg,1, 0, 0.f);


			}
			else
			{
				msg.status = CC_2;
				msg.data1 = paramNumber-128; //stupid offset only in CC not CC2
				msg.data2 = val;

			//	msg.data1 += 1;
				msg.data1 &= 0x7f;
				midiParser_ccHandler(msg,1, 0, 0.f);

			}
			soundengine_process();
		}
}
//----------------------------------------------------
// send loaded parameters to back
void preset_sendDrumsetParameters()
{
	uint8_t i;
	//special case mod targets
	for(i=0;i<6;i++)
	{
		//**VELO load drumkit. translate to param value before sending
		// parameter_values[PAR_VEL_DEST_1+i] is an index into modTargets, we need to send
		// a parameter number
		uint8_t value = (uint8_t)pgm_read_word(&modTargets[activeParameters[PAR_VEL_DEST_1+i]].param);
		uint8_t upper,lower;
		upper = (uint8_t)(((value&0x80)>>7) | (((i)&0x3f)<<1));
		lower = value&0x7f;
		frontPanel_sendData(CC_VELO_TARGET,upper,lower);

		// ensure target voice # is valid
		if(activeParameters[PAR_VOICE_LFO1+i] < 1 || activeParameters[PAR_VOICE_LFO1+i] > 6 )
			activeParameters[PAR_VOICE_LFO1+i]=1;

		// **LFO par_target_lfo will be an index into modTargets, but we need a parameter number to send
		value = (uint8_t)pgm_read_word(&modTargets[activeParameters[PAR_TARGET_LFO1+i]].param);

		upper = (uint8_t)(((value&0x80)>>7) | (((i)&0x3f)<<1));
		lower = value&0x7f;
		frontPanel_sendData(CC_LFO_TARGET,upper,lower);
	}

	// todo will this morph (and fuck up) our modulation targets?
	// send parameters (possibly combined with morph parameters) to back
	preset_morph(activeParameters[PAR_MORPH]);



}
//----------------------------------------------------
/*
char* preset_loadName(uint8_t presetNr, uint8_t nameBufferIdx, uint8_t what)
{
	uint8_t type;

	switch(what) {
	case SAVE_TYPE_PATTERN:
		type=FEXT_PAT;
		break;
	case SAVE_TYPE_KIT:
	case SAVE_TYPE_MORPH:
		type=FEXT_SOUND;
		break;
	case SAVE_TYPE_ALL:
		type=FEXT_ALL;
		break;
	case SAVE_TYPE_PERFORMANCE:
		type=FEXT_PERF;
		break;
	default:
		return NULL; // for glo and sample we don't load a name
	}
	

#if USE_SD_CARD		
	//filename in 8.3  format
	char filename[9];
	preset_makeFileName(filename,presetNr,type);

	//try to open the file
	FRESULT res = f_open((FIL*)&SDFile,filename,FA_OPEN_EXISTING | FA_READ);
	


	if(res!=FR_OK)
	{
		//error opening the file
		memcpy((void*)savefileNames[nameBufferIdx][presetNr],PSTR("Empty   "),8);
		return NULL;	
	}
	
	//file opened correctly -> extract name (first 8 bytes)
	unsigned int bytesRead;
	f_read((FIL*)&SDFile,(void*)savefileNames[nameBufferIdx][presetNr],8,&bytesRead);
	


	//close the file handle
	f_close((FIL*)&SDFile);
	
	return (char*)savefileNames[nameBufferIdx][presetNr];
#else

return "ToDo";
#endif
}
*/

//----------------------------------------------------
/** request step data from the cortex via uart and save it in the provided step struct*/
/*
void preset_queryStepDataFromSeq(uint16_t stepNr)
{
	frontParser_newSeqDataAvailable = 0;

	//request step data
	//the max number for 14 bit data is 16383!!!
	//current max step nr is 128*7*8 = 7168
	frontPanel_sendByte((stepNr>>7)&0x7f);		//upper nibble 7 bit
	frontPanel_sendByte(stepNr&0x7f);			//lower nibble 7 bit

	//wait until data arrives
	uint8_t newSeqDataLocal = 0;
	uint16_t now = systick_ticks;
	while((newSeqDataLocal==0))
	{
		//we have to call the uart parser to handle incoming messages from the sequencer
		uart_checkAndParse();
		
		newSeqDataLocal = frontParser_newSeqDataAvailable;
		
		if(systick_ticks-now >= 31)
		{
			//timeout
			now = systick_ticks;
			//request step again
			frontPanel_sendByte((stepNr>>7)&0x7f);		//upper nibble 7 bit
			frontPanel_sendByte(stepNr&0x7f);
		}
	}
}
*/
//----------------------------------------------------
/*
static void preset_sendMainStepDataToSeq( uint16_t mainStepData)
{
	frontPanel_sendByte(mainStepData	& 0x7f);
	frontPanel_sendByte((mainStepData>>7)	& 0x7f);
	frontPanel_sendByte((uint8_t)((mainStepData>>14)	& 0x7f));
}
*/
//----------------------------------------------------
/** send step data from SD card to the sequencer*/
/*
static void preset_sendStepDataToSeq()
{
	frontPanel_sendByte(frontParser_stepData.volume	& 0x7f);
	frontPanel_sendByte(frontParser_stepData.prob	& 0x7f);
	frontPanel_sendByte(frontParser_stepData.note	& 0x7f);

	frontPanel_sendByte(frontParser_stepData.param1Nr	& 0x7f);
	frontPanel_sendByte(frontParser_stepData.param1Val	& 0x7f);

	frontPanel_sendByte(frontParser_stepData.param2Nr	& 0x7f);
	frontPanel_sendByte(frontParser_stepData.param2Val	& 0x7f);

	//now the MSBs from all 7 values
	frontPanel_sendByte((uint8_t) 	(((frontParser_stepData.volume 	& 0x80)>>7) |
							((frontParser_stepData.prob	 	& 0x80)>>6) |
							((frontParser_stepData.note	 	& 0x80)>>5) |
							((frontParser_stepData.param1Nr	& 0x80)>>4) |
							((frontParser_stepData.param1Val	& 0x80)>>3) |
							((frontParser_stepData.param2Nr	& 0x80)>>2) |
							((frontParser_stepData.param2Val	& 0x80)>>1))
							);
}	
*/
//----------------------------------------------------
void preset_queryPatternInfoFromSeq(uint8_t patternNr, uint8_t* next, uint8_t* repeat)
{
	frontParser_newSeqDataAvailable = 0;
	//request pattern info
	frontPanel_sendByte(patternNr);	

	//wait until data arrives
	uint8_t newSeqDataLocal = 0;
	uint16_t now = systick_ticks;
	while((newSeqDataLocal==0))
	{
		//we have to call the uart parser to handle incoming messages from the sequencer
		uart_checkAndParse();
		
		newSeqDataLocal = frontParser_newSeqDataAvailable;
		
		if(systick_ticks-now >= 31)
		{
			//timeout
			now = systick_ticks;
			//request step again
			frontPanel_sendByte(patternNr);	
		}
	}
	
	//the stepdata struct is used as buffer for the data
	*next = frontParser_stepData.volume; 
	*repeat =  frontParser_stepData.prob;
}
//----------------------------------------------------
void preset_queryMainStepDataFromSeq(uint16_t stepNr, uint16_t *mainStepData, uint8_t *length)
{
	frontParser_newSeqDataAvailable = 0;

	//request step data
	//the max number for 14 bit data is 16383!!!
	//current max step nr is 7*8 = 56
	frontPanel_sendByte((stepNr>>7)&0x7f);		//upper nibble 7 bit
	frontPanel_sendByte(stepNr&0x7f);	//lower nibble 7 bit

	//wait until data arrives
	uint8_t newSeqDataLocal = 0;
	uint16_t now = systick_ticks;
	while((newSeqDataLocal==0))
	{
		//we have to call the uart parser to handle incoming messages from the sequencer
		uart_checkAndParse();
		
		newSeqDataLocal = frontParser_newSeqDataAvailable;
		
		if(systick_ticks-now >= 31)
		{
			//timeout
			now = systick_ticks;
			//request step again
			frontPanel_sendByte((stepNr>>7)&0x7f);		//upper nibble 7 bit
			frontPanel_sendByte(stepNr&0x7f);
		}
	}
	
	// we are reusing these members for purposes other than those that were originally intended
	*mainStepData =(uint16_t) ((frontParser_stepData.volume<<8) | frontParser_stepData.prob);
	*length=frontParser_stepData.note;
};

//----------------------------------------------------
static FRESULT preset_writePatternData(uint8_t patternNr)
{
#if 0
#else
		uint8_t length;
		FRESULT res;

		//---------------  write the step data (note, vol, prob, autom.) ---------------
		for(uint8_t trk=0; trk<NUM_TRACKS;trk++)
		{
			res = preset_writeChecked(&seq_patternSet.seq_subStepPattern[patternNr][trk][0], sizeof(Step)*STEPS_PER_PATTERN);
			if (res != FR_OK) return res;

			res = preset_writeChecked(&seq_patternSet.seq_stepTiming[patternNr][trk][0], sizeof(uint8_t)*STEPS_PER_PATTERN);
			if (res != FR_OK) return res;
		}

		//--------------- now the main step data ---------------
		for(uint8_t trk=0; trk<NUM_TRACKS;trk++)
		{
			for(uint8_t bar=0;bar<NUM_BARS; bar++)
			{
				const uint16_t mainStepData = seq_patternSet.seq_mainSteps[bar][patternNr][trk];
				res = preset_writeChecked(&mainStepData, sizeof(uint16_t));
				if (res != FR_OK) return res;
			}
		}

		//----- shuffle setting ------


		//f_write((FIL*)&SDFile,(const void*)&activeParameters[PAR_SHUFFLE],sizeof(uint8_t),&bytesWritten);
		res = preset_writeChecked(&seq_patternSet.shuffle[patternNr], sizeof(uint8_t));
		if (res != FR_OK) return res;

		//----- pattern/track lengths ------
		// we reuse the same call from above (when saving main step data)
		// but we only use the length info retrieved. We want to store it at the end
		// to avoid breaking compatibility with save file


		for(uint8_t trk=0; trk<NUM_TRACKS;trk++)
		{
				length = seq_patternSet.length[patternNr][trk] ;//seq_patternSet.seq_mainSteps[bar][pat][trk];
				res = preset_writeChecked(&length, sizeof(uint8_t));
				if (res != FR_OK) return res;
		}

		//write associated kit nr
		res = preset_writeChecked(&seq_patternSet.kitNr[patternNr], sizeof(uint8_t));
		if (res != FR_OK) return res;

#endif
	return FR_OK;
}

//----------------------------------------------------
/*
void preset_savePatternTemp(uint8_t presetNr)
{

	UINT bytesWritten;
	//lcd_clear();
	//lcd_home();
	//lcd_string(PSTR("Saving pattern"));


	//filename in 8.3  format
	char filename[9];
	//sprintf(filename,"p%03d.pat",presetNr);
	preset_makeFileNameTemp(filename,presetNr,FEXT_PAT);
	//open the file
	f_open((FIL*)&SDFile,filename,FA_CREATE_ALWAYS | FA_WRITE);

	//write preset name to file
	f_write((FIL*)&SDFile,(void*)preset_currentName,8,&bytesWritten);

	preset_writePatternData();

	//close the file
	f_close((FIL*)&SDFile);

	//reset the lcd
	menu_repaintAll();


}
*/
//----------------------------------------------------
FRESULT preset_savePatternSD(uint8_t presetNr)
{

	FRESULT res;
	//lcd_clear();
	//lcd_home();
	//lcd_string(PSTR("Saving pattern"));
	
	
	//filename in 8.3  format
	char filename[13];
	//sprintf(filename,"p%03d.pat",presetNr);
	preset_makeFileNameSave(filename,presetNr,FEXT_PAT, savefileNames[presetNr][PATTERN_NAMES]);
	//open the file
	res = f_open((FIL*)&SDFile,filename,FA_CREATE_ALWAYS | FA_WRITE);
	if(res != FR_OK) return res;

	//write preset name to file
	res = preset_writeChecked(savefileNames[presetNr][PATTERN_NAMES], 8);
	if(res != FR_OK) { f_close((FIL*)&SDFile); return res; }
	//write version tag
	res = preset_writeChecked("PatV03", 6);
	if(res != FR_OK) { f_close((FIL*)&SDFile); return res; }

	//write pattern data
	res = preset_writePatternData(presetNr);
	if(res != FR_OK) { f_close((FIL*)&SDFile); return res; }

	//close the file
	res = f_close((FIL*)&SDFile);
	if(res != FR_OK) return res;
	
	//reset the lcd
	menu_repaintAll();
	
	return FR_OK;
}

//----------------------------------------------------
// returns 1 on success
static uint8_t preset_readPatternData(uint8_t patternNr, uint8_t dataVersion)
{

#if 0


	// note that the pattern length data is no longer stored in the same way.
	// This means that loading old patterns with non-standard length, the length will be
	// ignored and a standard length used. I think it's acceptable as long as users are aware of this.

	UINT bytesRead;
	uint8_t success=1; // start off succeeding
	//uint16_t i;
	//uint8_t repeat=0;
	uint8_t next=0;

	//lcd_clear();
	//lcd_home();
	//lcd_string(PSTR("Loading pattern"));

	// ---------- Load step data first



	// load step data


	for(uint8_t pat=0; pat<NUM_PATTERN;pat++)
	{
		for(uint8_t trk=0; trk<NUM_TRACKS;trk++)
		{

#if 1

			if( (pat == seq_activePattern) && seq_isRunning() )
			{
				f_read((FIL*)&SDFile,(void*)&seq_tmpPattern.seq_subStepPattern[trk][0],sizeof(Step)*STEPS_PER_PATTERN,&bytesRead);
			} else {
				f_read((FIL*)&SDFile,(void*)&seq_patternSet.seq_subStepPattern[pat][trk][0],sizeof(Step)*STEPS_PER_PATTERN,&bytesRead);
			}



			if(bytesRead!=sizeof(Step)*(STEPS_PER_PATTERN))
			{
				success=0;
				break; // file error, or we ran out of file (eof)
			}
#else
			for(uint8_t stp=0;stp<STEPS_PER_PATTERN; stp++)
			{
				f_read((FIL*)&SDFile,(void*)&frontParser_stepData,sizeof(Step),&bytesRead);
				if(bytesRead==sizeof(Step))
				{
					seq_patternSet.seq_subStepPattern[pat][trk][stp] = frontParser_stepData;
				} else {
					success=0;
					break; // file error, or we ran out of file (eof)
				}
			}
#endif
		}
	}

	// ---------- load the main step data next
	uint16_t mainStepData;

	for(uint8_t pat=0; pat<NUM_PATTERN;pat++)
	{
		for(uint8_t trk=0; trk<NUM_TRACKS;trk++)
		{
			for(uint8_t bar=0;bar<NUM_BARS; bar++)
			{
				f_read((FIL*)&SDFile,(void*)&mainStepData,sizeof(uint16_t),&bytesRead);

				if( bytesRead==sizeof(uint16_t)) {

					if( (pat == seq_activePattern) && seq_isRunning() )
					{
						seq_tmpPattern.seq_mainSteps[bar][trk] = mainStepData;
					} else {
						seq_patternSet.seq_mainSteps[bar][pat][trk] = mainStepData;
					}
				}
				 else {
						success=0;
						break;
					}
			}
		}
	}

	//---------- pattern info (next/repeat)
	/*
	if(success) {
		for(i=0;i<(NUM_PATTERN);i++)
		{
			f_read((FIL*)&preset_File,(void*)&next,sizeof(uint8_t),&bytesRead);
			if(!bytesRead) {
				success=0;
				break;
			}
			f_read((FIL*)&preset_File,(void*)&repeat,sizeof(uint8_t),&bytesRead);
			if(!bytesRead) {
				success=0;
				break;
			}

			if( (i == seq_activePattern) && seq_isRunning() )
			{
				seq_tmpPattern.seq_patternSettings.nextPattern = next;
				seq_tmpPattern.seq_patternSettings.changeBar = repeat;
			} else {
				seq_patternSet.seq_patternSettings[i].nextPattern = next;
				seq_patternSet.seq_patternSettings[i].changeBar = repeat;
			}

		}
	}
	*/

	// ------------ shuffle settings
	if(success) {
		//load the shuffle settings
		f_read((FIL*)&SDFile,(void*)&activeParameters[PAR_SHUFFLE],sizeof(uint8_t),&bytesRead);
		//and update on cortex
		if(bytesRead)
			//frontPanel_sendData(SEQ_CC,SEQ_SHUFFLE,parameter_values[PAR_SHUFFLE]);
			seq_setShuffle(activeParameters[PAR_SHUFFLE]/127.f);
		else {
			success=0;
		}
	}

	// ----------- Pattern/track lengths
	// -- AS this might not exist in the saved data file, we still want success
	if(success) {


		for(uint8_t pat=0; pat<NUM_PATTERN;pat++)
		{
			for(uint8_t trk=0; trk<NUM_TRACKS;trk++)
			{
				f_read((FIL*)&SDFile,(void*)&next,sizeof(uint8_t),&bytesRead);


				if( (pat == seq_activePattern) && seq_isRunning() )
				{
					seq_tmpPattern.length[trk] = next;
				} else {
					seq_patternSet.length[pat][trk] = next;
				}
			}
		}

		// signal new pattern after receiving all the data
		/*
		if( seq_isRunning() ) {
			seq_newPatternAvailable = 1;
		}
		*/

		success=1; // just to document that we don't consider this a failure, we just didn't read the bytes for this
	}

	return success;
#else

	// note that the pattern length data is no longer stored in the same way.
	// This means that loading old patterns with non-standard length, the length will be
	// ignored and a standard length used. I think it's acceptable as long as users are aware of this.

	UINT bytesRead;
	uint8_t success=1; // start off succeeding
	uint8_t next=0;

	// ---------- Load step data first

	// load step data



	for(uint8_t trk=0; trk<NUM_TRACKS;trk++)
	{

		f_read((FIL*)&SDFile,(void*)&seq_patternSet.seq_subStepPattern[patternNr][trk][0],sizeof(Step)*STEPS_PER_PATTERN,&bytesRead);

		if(bytesRead!=sizeof(Step)*(STEPS_PER_PATTERN))
		{
			success=0;
			break; // file error, or we ran out of file (eof)
		}

		//Pattern V3 and up => flam added
		if(dataVersion > 2)
		{
			f_read((FIL*)&SDFile,(void*)&seq_patternSet.seq_stepTiming[patternNr][trk][0],sizeof(uint8_t)*STEPS_PER_PATTERN,&bytesRead);

			if(bytesRead!=sizeof(uint8_t)*(STEPS_PER_PATTERN))
			{
				success=0;
				break; // file error, or we ran out of file (eof)
			}
		}

	}



	// ---------- load the main step data next
	uint16_t mainStepData;


		for(uint8_t trk=0; trk<NUM_TRACKS;trk++)
		{
			for(uint8_t bar=0;bar<NUM_BARS; bar++)
			{
				f_read((FIL*)&SDFile,(void*)&mainStepData,sizeof(uint16_t),&bytesRead);

				if( bytesRead==sizeof(uint16_t)) {
					seq_patternSet.seq_mainSteps[bar][patternNr][trk] = mainStepData;
				}
				 else {
						success=0;
						break;
					}
			}
		}



	// ------------ shuffle settings
	if(success) {
		//load the shuffle settings
		f_read((FIL*)&SDFile,(void*)&seq_patternSet.shuffle[patternNr],sizeof(uint8_t),&bytesRead);
		//and update on cortex
		if(bytesRead)
		{

			//activeParameters[PAR_SHUFFLE] = seq_patternSet.shuffle[patternNr];
			//frontPanel_sendData(SEQ_CC,SEQ_SHUFFLE,parameter_values[PAR_SHUFFLE]);

			//seq_setShuffle(activeParameters[PAR_SHUFFLE]/127.f);
		}
		else {
			success=0;
		}
	}

	// ----------- Pattern/track lengths
	// -- AS this might not exist in the saved data file, we still want success
	if(success) {

			for(uint8_t trk=0; trk<NUM_TRACKS;trk++)
			{
				f_read((FIL*)&SDFile,(void*)&next,sizeof(uint8_t),&bytesRead);
				seq_patternSet.length[patternNr][trk] = next;
			}

		success=1; // just to document that we don't consider this a failure, we just didn't read the bytes for this
	}

	if(dataVersion > 1)
	{
		f_read((FIL*)&SDFile,(void*)&seq_patternSet.kitNr[patternNr],sizeof(uint8_t),&bytesRead);
		if(bytesRead == 0) success = 0;

	}

	return success;
#endif

}
//----------------------------------------------------
/*
uint8_t preset_loadPatternTemp(uint8_t presetNr)
{
#if USE_SD_CARD
	//filename in 8.3  format
	char filename[9];
	UINT bytesRead;
	uint8_t success=0;
	//sprintf(filename,"p%03d.pat",presetNr);
	preset_makeFileNameTemp(filename,presetNr,FEXT_PAT);

	//open the file
	FRESULT res = f_open((FIL*)&SDFile,filename,FA_OPEN_EXISTING | FA_READ);

	if(res!=FR_OK)
	{

		return 0; //file open error... maybe the file does not exist?
	}


	//read the preset name
	f_read((FIL*)&SDFile,(void*)preset_currentName,8,&bytesRead);
	if(bytesRead != 8)
		goto closeFile;

	if(!preset_readPatternData())
		goto closeFile;

	success=1;
	//close the file handle
closeFile:
	f_close((FIL*)&SDFile);

	//force complete repaint
	menu_repaintAll();
	// return success or failure
	return success;

#else
frontPanel_sendData(PRESET,PATTERN_LOAD,presetNr);

#endif
}
*/
//----------------------------------------------------
FRESULT preset_loadPatternSD(uint8_t presetNr)
{
#if USE_SD_CARD		
	//filename in 8.3  format
	uint8_t fileVersion = 1;
	char filename[13];
	UINT bytesRead;
	uint8_t success=0;
	//sprintf(filename,"p%03d.pat",presetNr);

	/*
	preset_makeFileNameLoad(filename,presetNr,FEXT_PAT);

	//open the file
	FRESULT res = f_open((FIL*)&SDFile,filename,FA_OPEN_EXISTING | FA_READ);
	*/
	FRESULT res = preset_makeFileNameLoad(filename,presetNr,FEXT_PAT);
	if(res!=FR_OK)
	{
		//try old filename
		preset_makeFileNameOld(filename,presetNr,FEXT_PAT);
		res = f_open((FIL*)&SDFile,filename,FA_OPEN_EXISTING | FA_READ);
		if(res!=FR_OK)
			return res; //file open error... maybe the file does not exist?
	}
	
	//read the preset name
	res = f_read((FIL*)&SDFile,(void*)savefileNames[presetNr][PATTERN_NAMES],8,&bytesRead);
	if(bytesRead != 8)
		goto closeFile;
	
	char buf[7];
	res = f_read((FIL*)&SDFile,buf,6,&bytesRead);
	//zero termination
	buf[6]=0;
	fileVersion = atoi(&buf[4]);
	buf[4] = 0; // PatV\0

	if(strcmp(buf, "PatV") == 0)
	{
		//we have a new version pattern preset >V0 (V0 has no version tag)


	}
	else
	{
		//V1 preset data without version tag -> rewind read pointer
		fileVersion = 1; //re-assign since atoi() broke the 1
		f_lseek((FIL*)&SDFile, f_tell((FIL*)&SDFile) - 6);
	}
	//f_write((FIL*)&SDFile,"PatV02",6,&bytesWritten);

	if(!preset_readPatternData(presetNr, fileVersion))
	{
		goto closeFile;
	}
	
	success=1;
	//close the file handle



closeFile:
	f_close((FIL*)&SDFile);
	
	//force complete repaint
	menu_repaintAll();
	// return success or failure
	if(success) return FR_OK;
	else return FR_INT_ERR;
	
#else
frontPanel_sendData(PRESET,PATTERN_LOAD,presetNr);

#endif
}

//----------------------------------------------------
FRESULT preset_savePatternRAM(uint8_t presetNr)
{

	//check if current slot or new slot
	if(presetNr == seq_activePattern)
	{
		//### Current Slot ###
		//write current (changed) pattern to backup buffer, to make changes permanent
		seq_copyPatternToBackupBuffer(presetNr);
	}
	else
	{
		//### NEW SLOT ###
		//- write current (changed) pattern to new slot
		seq_copyPattern(seq_activePattern, presetNr);
		//- change active pattern to new slot (writes backup buffer back to current slot, reverting changes being made in current slot)
		seq_setNextPattern(presetNr);
	}

	return FR_OK;
}
//----------------------------------------------------
FRESULT preset_loadPatternRAM(uint8_t presetNr)
{

	//I'm unused since there is no Pattern Load page, it just uses the pattern change/select page instead
	return FR_OK;
}
//----------------------------------------------------
//called periodically to process non blocking morph
void preset_morph_tick()
{
	//morph_state = 0;
	//return;
	if(morph_state)
	{

		uint8_t morph;

		//Do not morph audio routing
		if((morph_index >= PAR_AUDIO_OUT1) && (morph_index <= PAR_AUDIO_OUT6)) //morph_index++;
		{
			//always use source kit (morph = 0) for audio routing
			morph = 0;
		}
		else
		{
			morph = activeParameters[PAR_MORPH];
		}

		static MidiMsg msg = {0};


		const uint8_t val = interpolate(activeParameters[morph_index],morphParameters[morph_index],morph);


		if(morph_index < 128)
		{
			/* comes from cortex midi handler
			 * stupid offset!
			 * TODO fix this stupid offset			 *
			 * 		frontParser_midiMsg.data1 += 1;

					//because hh slope on front is 127 and on cortex is 0 wrap data1 at 127
					frontParser_midiMsg.data1 &= 0x7f;
			 *
			 */
			msg.status = MIDI_CC;
			msg.data1 = morph_index;
			msg.data2 = val;

			msg.data1 += 1;
			msg.data1 &= 0x7f;
			midiParser_ccHandler(msg,1, 0, 0.f);


		}
		else
		{
			msg.status = CC_2;
			msg.data1 = morph_index-128; //stupid offset only in CC not CC2
			msg.data2 = val;

		//	msg.data1 += 1;
			msg.data1 &= 0x7f;
			midiParser_ccHandler(msg,1, 0, 0.f);

		}


		morph_index++;
		if(morph_index >= END_OF_SOUND_PARAMETERS)
		{
			//finished morphing
			morph_state = 0;
		}
	}
}
//----------------------------------------------------
// This will determine an interpolated value between parameters
// and morph parameters and send the data to the back
// if the morph value is 0 it will just send the parameter values
// to the back
void preset_morph(uint8_t morph)
{
#if 1
	//initialize new morph
	morph_state = 1; //start morph
	morph_index = 0; //reset index
#else
	int i;
	//TODO so far only sound parameters are interpolated, no LFOs etc

	for(i=0;i<END_OF_SOUND_PARAMETERS;i++)
	{


		uint8_t val;
		
		val = interpolate(mainParameters[i],morphParameters[i],morph);

		if(i<128) {
			frontPanel_sendData(MIDI_CC,(uint8_t)i,val);

		} else {
			frontPanel_sendData(CC_2,(uint8_t)(i-128),val);

		}		
		
			
		//to omit front panel button/LED lag we have to process din dout and uart here
		//read next button

		//update LEDs
		dio_process();
		//read uart messages from sequencer
		uart_checkAndParse();
	}		
#endif
}

//----------------------------------------------------
uint8_t preset_getMorphValue(uint16_t index, uint8_t morph)
{
	return interpolate(activeParameters[index],morphParameters[index],morph);
};
//----------------------------------------------------

FRESULT preset_createProject(uint8_t presetNr)
{


	//create default globals

	//clear all patterns

	//clear all kits

	//clear all songs

	//load init kit


	int i;

	preset_loadDefaultGlobals();
	//give active song pointer a proper value
	seq_activeSong = seq_songData[0];

	memset(seq_songData, SONG_END, sizeof(uint8_t) * MAX_SONG_LENGTH * NUM_KIT_SLOTS);
	seq_songData[0][0] = 0;


	//memset(seq_stepIndex,0,NUM_TRACKS);
//	memset(seq_lastMasterStep,0,NUM_TRACKS);


	for(i=0;i<NUM_PATTERN;i++)
	{
		//seq_patternSet.seq_patternSettings[i].changeBar 	= 0;	//default setting: zero repeats (play once then change)
		//seq_patternSet.seq_patternSettings[i].nextPattern 	= i;	//default setting: repeat same pattern
		seq_clearPattern(i); // will clear all tracks in the pattern
		preset_init(i); //set all kits to init sound
	}

	sprintf(savefileNames[presetNr][PROJECT_NAMES], "Init");

	preset_clearTmpFolder();

	return FR_OK;
}
//----------------------------------------------------
// **SAVEALL
// save all settings (global, pattern, kit)
// or save performance settings (pattern, kit)

static FRESULT preset_writeProjectFolder(uint8_t presetNr, const char *folder)
{
	FRESULT res;
	FILINFO fno;

	preset_saveDrumsetRam(menu_selectedPresetNr[SAVE_TYPE_KIT], 0);
	preset_savePatternRAM(seq_activePattern);

#if USE_INTERRUPT_ONLY_DURING_SAVE_PROJ
	HAL_TIM_Base_Start_IT(&htim2);	//calc sample interrupt - prio 1
#else

#endif

	lcd_process_display_interrupt = 0;
	while(sendDisplayBuffer() != 0);

	lcd_clear();
	lcd_home();
	lcd_string(PSTR("Saving Project"));
	lcd_setcursor(0,2);
	lcd_string(PSTR("0%"));
	/*
	MX_FATFS_ReInit();
	f_mount(0, (TCHAR const*)SDPath, 0); //un-mount
	memset((FATFS*)&SDFatFS,0,sizeof(FATFS)); //clear FS object to force reinit
	res = f_mount((FATFS*)&SDFatFS, (TCHAR const*)SDPath,1);
	if( res != FR_OK)
	{
		return res;
	}
*/
	// Build the replacement outside the active project directory.
	strcpy(currentFolder, folder);

	res = f_stat(currentFolder, &fno);
	if(res == FR_NO_FILE)
	{
		res = f_mkdir(currentFolder);
		if(res == FR_OK)
		{
			res = f_chdir(currentFolder);
			if (res != FR_OK) {
				preset_initTmpFolder();
				return res;
			}
		}
		else
		{
			lcd_process_display_interrupt = 1;
			preset_initTmpFolder();
#if USE_INTERRUPT_ONLY_DURING_SAVE_PROJ
			HAL_TIM_Base_Stop_IT(&htim2);	//calc sample interrupt - prio 1
#else

#endif


			return res;
		}
	}
	else
	{
		char work[256];
		strcpy(work, currentFolder);
		res = empty_directory(work, sizeof work);
		if(res != FR_OK)
		{
			lcd_process_display_interrupt = 1;
			preset_initTmpFolder();
#if USE_INTERRUPT_ONLY_DURING_SAVE_PROJ
			HAL_TIM_Base_Stop_IT(&htim2);	//calc sample interrupt - prio 1
#else

#endif

			return res;
		}

		res = f_chdir(currentFolder);
		if(res != FR_OK)
		{
			lcd_process_display_interrupt = 1;
			preset_initTmpFolder();
#if USE_INTERRUPT_ONLY_DURING_SAVE_PROJ
			HAL_TIM_Base_Stop_IT(&htim2);	//calc sample interrupt - prio 1
#else

#endif

			return res;
		};
	}
	//save data

#if 1



	char text[17];

	//Save Project name
	//open the file
	res = f_open((FIL*)&SDFile,"prj.nfo",FA_CREATE_ALWAYS | FA_WRITE);
	if(res != FR_OK) {
		lcd_process_display_interrupt = 1;
		preset_initTmpFolder();
#if USE_INTERRUPT_ONLY_DURING_SAVE_PROJ
			HAL_TIM_Base_Stop_IT(&htim2);	//calc sample interrupt - prio 1
#else

#endif

		return res;
	}
	unsigned int bytesWritten;
	//write preset name to file
	res = f_write((FIL*)&SDFile,(void*)savefileNames[presetNr][PROJECT_NAMES],8,&bytesWritten);
	if(res != FR_OK || bytesWritten != 8) {
		if(res == FR_OK) res = FR_DISK_ERR;
		f_close((FIL*)&SDFile);
		lcd_process_display_interrupt = 1;
		preset_initTmpFolder();
#if USE_INTERRUPT_ONLY_DURING_SAVE_PROJ
			HAL_TIM_Base_Stop_IT(&htim2);	//calc sample interrupt - prio 1
#else

#endif

		return res;
	}
	//close the file
	res = f_close((FIL*)&SDFile);
	if(res != FR_OK) { preset_initTmpFolder(); return res; }

//	soundengine_process();
	//save globals
	res = preset_saveGlobals();
	if(res != FR_OK) { preset_initTmpFolder(); return res; }
	//save all drumkits, patterns and songs
	for(uint8_t i=0;i<NUM_KIT_SLOTS;i++)
	{
		sprintf(text, "%d%%", (uint8_t)(i/(float)NUM_KIT_SLOTS * 100.f));
		lcd_setcursor(0,2);
		lcd_string(text);
	//	soundengine_process();
		res = preset_saveDrumsetSD(i, 0);
		if(res != FR_OK) {
			lcd_process_display_interrupt = 1;
			preset_initTmpFolder();
#if USE_INTERRUPT_ONLY_DURING_SAVE_PROJ
			HAL_TIM_Base_Stop_IT(&htim2);	//calc sample interrupt - prio 1
#else

#endif

			return res;
		}
	//	soundengine_process();
		res = preset_savePatternSD(i);
		if(res != FR_OK) {
			lcd_process_display_interrupt = 1;
			preset_initTmpFolder();
#if USE_INTERRUPT_ONLY_DURING_SAVE_PROJ
			HAL_TIM_Base_Stop_IT(&htim2);	//calc sample interrupt - prio 1
#else

#endif
			return res;
		}
	//	soundengine_process();
		res = preset_saveSongSD(i);
		if(res != FR_OK) {
			lcd_process_display_interrupt = 1;
			preset_initTmpFolder();
#if USE_INTERRUPT_ONLY_DURING_SAVE_PROJ
			HAL_TIM_Base_Stop_IT(&htim2);	//calc sample interrupt - prio 1
#else

#endif
			return res;
		}
	}

	lcd_process_display_interrupt = 1;
	preset_initTmpFolder();
#if USE_INTERRUPT_ONLY_DURING_SAVE_PROJ
			HAL_TIM_Base_Stop_IT(&htim2);	//calc sample interrupt - prio 1
#else

#endif

			preset_clearTmpFolder();

	return FR_OK;

#else
#if USE_SD_CARD
	uint16_t remain;
	uint8_t siz;
	//filename in 8.3  format
	char filename[GEN_BUF_LEN];
	if(isAll)
		preset_makeFileName(filename,presetNr,FEXT_ALL);
	else
		preset_makeFileName(filename,presetNr,FEXT_PROJ);
	//open the file
	f_open((FIL*)&SDFile,filename,FA_CREATE_ALWAYS | FA_WRITE);


	unsigned int bytesWritten;
	//write preset name to file
	f_write((FIL*)&SDFile,(void*)savefileNames[KIT_NAMES][presetNr],8,&bytesWritten);

	//write the version
	filename[0]=FILE_VERSION;
	f_write((FIL*)&SDFile, filename, 1, &bytesWritten);

	memset(filename,0xFF,GEN_BUF_LEN); // reuse this as our buffer for padding

	if(isAll) {
		// write global settings
		preset_writeGlobalData();

		// write some padding (we'll allocate 64 bytes for globals, right now we have +/- 18) so that
		// we don't need to change the file format later
		remain=	64- (NUM_PARAMS - PAR_BEGINNING_OF_GLOBALS);

	} else {
		// write the bpm
		f_write((FIL*)&SDFile, &activeParameters[PAR_BPM], 1, &bytesWritten);
		// bar reset mode
		f_write((FIL*)&SDFile, &activeParameters[PAR_BAR_RESET_MODE], 1, &bytesWritten);

		remain=	64 - (1+1);
	}

	// write padding after globals/perf data
	while(remain){
		if(remain < GEN_BUF_LEN)
			siz=(uint8_t)remain;
		else
			siz=GEN_BUF_LEN;
		f_write((FIL*)&SDFile, filename, siz, &bytesWritten);
		remain -=siz;
	}

	// save kit
	preset_writeDrumsetData(0);

	// write some more padding (we'll allocate 512 bytes for kit data. This should be more than
	// we'll ever need. Right now we use about 228 bytes).
	remain = 512 - (END_OF_SOUND_PARAMETERS);
	while(remain){
		if(remain < GEN_BUF_LEN)
			siz=(uint8_t)remain;
		else
			siz=GEN_BUF_LEN;
		f_write((FIL*)&SDFile, filename, siz, &bytesWritten);
		remain -=siz;
	}

	// write pattern data
	preset_writePatternData(0);

	//close the file
	f_close((FIL*)&SDFile);
#else
#endif
#endif
}

//----------------------------------------------------
/* tx.ok is separate from the legacy kit/pattern/song formats. Its checksum
 * covers file names and contents; an interrupted promotion must not turn a
 * partially written directory into a loadable project. */
#define PROJECT_MARKER_MAGIC 0x3142584aUL /* JXB1, little endian */
#define PROJECT_FILE_COUNT (2u + 3u * NUM_KIT_SLOTS)

static uint8_t preset_nameEndsWith(const char *name, const char *suffix)
{
	size_t length = strlen(name), suffixLength = strlen(suffix);
	if (length < suffixLength) return 0;
	name += length - suffixLength;
	for (size_t i = 0; i < suffixLength; ++i) {
		char a = name[i], b = suffix[i];
		if (a >= 'A' && a <= 'Z') a += 'a' - 'A';
		if (a != b) return 0;
	}
	return 1;
}

static FRESULT preset_projectFingerprint(const char *folder, uint32_t result[5])
{
	DIR dir;
	FILINFO info;
	FIL file;
	char path[256];
	uint8_t buffer[128];
	/* _FS_LOCK is one: close the directory before opening any file. */
	static char names[PROJECT_FILE_COUNT][13];
	static uint32_t sizes[PROJECT_FILE_COUNT];
	uint64_t slots[3] = {0, 0, 0};
	uint8_t metadata = 0, globals = 0;
	uint32_t count = 0, sum = 0, xorHash = 0, total = 0;
	char primary[sizeof(currentFolder) + sizeof("/glo.cfg")];
	snprintf(primary, sizeof primary, "%s/glo.cfg", folder);
	FILINFO primaryInfo;
	FRESULT primaryResult = f_stat(primary, &primaryInfo);
	if (primaryResult != FR_OK && primaryResult != FR_NO_FILE) return primaryResult;
	uint8_t hasPrimary = primaryResult == FR_OK &&
	                     preset_globalsSizeValid(primaryInfo.fsize);
	FRESULT res = f_opendir(&dir, folder);
	if (res != FR_OK) return res;
	for (;;) {
		res = f_readdir(&dir, &info);
		if (res != FR_OK || !info.fname[0]) break;
		if (!strcmp(info.fname, ".") || !strcmp(info.fname, "..") ||
		    preset_nameEndsWith(info.fname, "tx.ok") ||
		    (!hasPrimary && preset_nameEndsWith(info.fname, "glo.cfg")) ||
		    (hasPrimary && preset_nameEndsWith(info.fname, "glo.bak")) ||
		    preset_nameEndsWith(info.fname, "glo.new")) continue;
		if ((info.fattrib & AM_DIR) || count >= PROJECT_FILE_COUNT ||
		    strlen(info.fname) >= sizeof names[0]) { res = FR_DISK_ERR; break; }
		int kind = -1;
		if (preset_nameEndsWith(info.fname, ".snd")) kind = 0;
		else if (preset_nameEndsWith(info.fname, ".pat")) kind = 1;
		else if (preset_nameEndsWith(info.fname, ".sng")) kind = 2;
		if (kind >= 0) {
			if (info.fname[0] < '0' || info.fname[0] > '9' ||
			    info.fname[1] < '0' || info.fname[1] > '9' ||
			    info.fname[2] != '-' || info.fsize < 8) { res = FR_DISK_ERR; break; }
			unsigned slot = (unsigned)(info.fname[0] - '0') * 10u +
			                (unsigned)(info.fname[1] - '0');
			if (slot >= NUM_KIT_SLOTS || (slots[kind] & (1ULL << slot))) {
				res = FR_DISK_ERR; break;
			}
			slots[kind] |= 1ULL << slot;
		} else if (preset_nameEndsWith(info.fname, "prj.nfo")) {
			if (metadata || info.fsize != 8) { res = FR_DISK_ERR; break; }
			metadata = 1;
		} else if (preset_nameEndsWith(info.fname, "glo.cfg") ||
		           (!hasPrimary && preset_nameEndsWith(info.fname, "glo.bak"))) {
			if (globals || !preset_globalsSizeValid(info.fsize)) {
				res = FR_DISK_ERR; break;
			}
			globals = 1;
		} else { res = FR_DISK_ERR; break; }
		strcpy(names[count], info.fname);
		sizes[count] = info.fsize;
		++count;
	}
	FRESULT closeRes = f_closedir(&dir);
	if (res == FR_OK) res = closeRes;
	if (res != FR_OK) return res;
	if (!metadata || !globals || count != PROJECT_FILE_COUNT) return FR_DISK_ERR;
#if NUM_KIT_SLOTS == 64
	uint64_t expected = ~(uint64_t)0;
#else
	uint64_t expected = (1ULL << NUM_KIT_SLOTS) - 1ULL;
#endif
	if (slots[0] != expected || slots[1] != expected || slots[2] != expected)
		return FR_DISK_ERR;
	for (uint32_t entry = 0; entry < count; ++entry) {
		int n = snprintf(path, sizeof path, "%s/%s", folder, names[entry]);
		if (n < 0 || (size_t)n >= sizeof path) return FR_INVALID_NAME;
		res = f_open(&file, path, FA_READ);
		if (res != FR_OK) return res;
		uint32_t hash = 2166136261UL;
		const char *hashName = preset_nameEndsWith(names[entry], "glo.bak") ?
		                       "glo.cfg" : names[entry];
		for (const char *p = hashName; *p; ++p) {
			char c = *p >= 'A' && *p <= 'Z' ? *p + ('a' - 'A') : *p;
			hash = (hash ^ (uint8_t)c) * 16777619UL;
		}
		uint32_t remaining = sizes[entry];
		while (remaining) {
			UINT amount = remaining < sizeof buffer ? (UINT)remaining : (UINT)sizeof buffer;
			UINT read = 0;
			res = f_read(&file, buffer, amount, &read);
			if (res != FR_OK || read != amount) {
				if (res == FR_OK) res = FR_DISK_ERR;
				break;
			}
			/* Globals have their own staged save and backup protocol. They
			 * may change without a whole-project replacement. */
			if (!preset_nameEndsWith(names[entry], "glo.cfg") &&
			    !preset_nameEndsWith(names[entry], "glo.bak"))
				for (UINT i = 0; i < read; ++i)
					hash = (hash ^ buffer[i]) * 16777619UL;
			remaining -= read;
		}
		closeRes = f_close(&file);
		if (res == FR_OK) res = closeRes;
		if (res != FR_OK) return res;
		uint8_t isGlobals = preset_nameEndsWith(names[entry], "glo.cfg") ||
		                    preset_nameEndsWith(names[entry], "glo.bak");
		uint32_t logicalSize = isGlobals ?
		    NUM_PARAMS - PAR_BEGINNING_OF_GLOBALS : sizes[entry];
		hash = (hash ^ logicalSize) * 16777619UL;
		sum += hash;
		xorHash ^= hash;
		total += logicalSize;
	}
	result[0] = PROJECT_MARKER_MAGIC;
	result[1] = count;
	result[2] = total;
	result[3] = sum;
	result[4] = xorHash;
	return FR_OK;
}

static FRESULT preset_sealProjectFolder(const char *folder)
{
	uint32_t marker[5];
	char path[sizeof(currentFolder) + sizeof("/tx.ok")];
	FRESULT res = preset_projectFingerprint(folder, marker);
	if (res != FR_OK) return res;
	snprintf(path, sizeof path, "%s/tx.ok", folder);
	res = f_open((FIL*)&SDFile, path, FA_CREATE_ALWAYS | FA_WRITE);
	if (res != FR_OK) return res;
	res = preset_writeChecked(marker, sizeof marker);
	if (res == FR_OK) res = f_sync((FIL*)&SDFile);
	FRESULT closeRes = f_close((FIL*)&SDFile);
	return res == FR_OK ? closeRes : res;
}

static uint8_t preset_projectDirectoryValid(const char *folder)
{
	char path[sizeof(currentFolder) + sizeof("/glo.cfg")];
	FILINFO info;
	if (f_stat(folder, &info) != FR_OK || !(info.fattrib & AM_DIR)) return 0;
	snprintf(path, sizeof path, "%s/tx.ok", folder);
	FRESULT markerResult = f_stat(path, &info);
	if (markerResult == FR_OK) {
		if (info.fsize != 5 * sizeof(uint32_t)) return 0;
		uint32_t expected[5], actual[5];
		FIL marker;
		UINT read = 0;
		if (f_open(&marker, path, FA_READ) != FR_OK) return 0;
		FRESULT res = f_read(&marker, expected, sizeof expected, &read);
		FRESULT closeRes = f_close(&marker);
		if (res != FR_OK || closeRes != FR_OK || read != sizeof expected) return 0;
		if (preset_projectFingerprint(folder, actual) != FR_OK ||
		    memcmp(expected, actual, sizeof expected) != 0) return 0;
		snprintf(path, sizeof path, "%s/glo.cfg", folder);
		if (preset_globalsFileValid(path)) return 1;
		snprintf(path, sizeof path, "%s/glo.bak", folder);
		return preset_globalsFileValid(path);
	}
	if (markerResult != FR_NO_FILE) return 0;
	/* Legacy projects have no marker. Keep their original file formats readable. */
	snprintf(path, sizeof path, "%s/prj.nfo", folder);
	if (f_stat(path, &info) != FR_OK || info.fsize < 8) return 0;
	snprintf(path, sizeof path, "%s/glo.cfg", folder);
	if (preset_globalsFileValid(path)) return 1;
	snprintf(path, sizeof path, "%s/glo.bak", folder);
	return preset_globalsFileValid(path);
}

static FRESULT preset_projectRename(const char *source, const char *destination)
{
#if LXR_EMU
	/* Host-only fault injection. The target always calls the real FatFs rename. */
	extern uint8_t emu_projectRenameFailBefore(void);
	extern uint8_t emu_projectRenameFailAfter(void);
	if (emu_projectRenameFailBefore()) return FR_DISK_ERR;
#endif
	FRESULT res = f_rename(source, destination);
#if LXR_EMU
	if (res == FR_OK && emu_projectRenameFailAfter()) return FR_DISK_ERR;
#endif
	return res;
}

/* A reset can land between moving the old project aside and promoting the
 * staged directory. Restore the old directory before the normal loader. */
static FRESULT preset_recoverProjectFolder(uint8_t presetNr)
{
	char target[13], backup[13];
	FILINFO info;
	snprintf(target, sizeof target, "/proj%02u", presetNr);
	snprintf(backup, sizeof backup, "/bak%02u", presetNr);
	FRESULT res = f_stat(target, &info);
	if (res == FR_OK && preset_projectDirectoryValid(target)) {
		/* A backup means a replacement was underway. A target without its
		 * marker may be an incomplete promotion, not a legacy project. */
		char marker[sizeof target + sizeof("/tx.ok")];
		snprintf(marker, sizeof marker, "%s/tx.ok", target);
		if (f_stat(marker, &info) == FR_OK ||
		    !preset_projectDirectoryValid(backup)) return FR_OK;
	}
	if (res != FR_OK && res != FR_NO_FILE) return res;
	if (!preset_projectDirectoryValid(backup)) return FR_OK;
	if (res == FR_OK) {
		char work[256];
		/* Boot may currently be inside the damaged project directory. */
		FRESULT leave = f_chdir("/");
		if (leave != FR_OK) return leave;
		strcpy(work, target);
		FRESULT cleanup = empty_directory(work, sizeof work);
		if (cleanup != FR_OK) return cleanup;
		cleanup = f_unlink(target);
		if (cleanup != FR_OK) return cleanup;
	}
	return preset_projectRename(backup, target);
}

FRESULT preset_saveProject(uint8_t presetNr)
{
	char target[13], stage[13], backup[13];
	FILINFO info;
	snprintf(target, sizeof target, "/proj%02u", presetNr);
	snprintf(stage, sizeof stage, "/stg%02u", presetNr);
	snprintf(backup, sizeof backup, "/bak%02u", presetNr);
	FRESULT res = preset_recoverProjectFolder(presetNr);
	if (res != FR_OK) return res;
	res = preset_writeProjectFolder(presetNr, stage);
	strcpy(currentFolder, target);
	if (res != FR_OK) return res;
	/* Never move the known-good project aside unless the staged generation
	 * has a synced marker covering every required project file. */
	res = preset_sealProjectFolder(stage);
	if (res != FR_OK || !preset_projectDirectoryValid(stage))
		return res == FR_OK ? FR_DISK_ERR : res;

	res = f_stat(target, &info);
	if (res == FR_OK) {
		FRESULT backupRes = f_stat(backup, &info);
		if (backupRes == FR_OK) {
			char work[256];
			strcpy(work, backup);
#if LXR_EMU
			extern void emu_projectBackupCleanupStart(void);
			emu_projectBackupCleanupStart();
#endif
			res = empty_directory(work, sizeof work);
			if (res != FR_OK) return res;
			res = f_unlink(backup);
			if (res != FR_OK) return res;
#if LXR_EMU
			extern void emu_projectBackupCleanupEnd(void);
			emu_projectBackupCleanupEnd();
#endif
		} else if (backupRes != FR_NO_FILE) return backupRes;
		res = preset_projectRename(target, backup);
		if (res != FR_OK) return res;
	} else if (res != FR_NO_FILE) return res;
	res = preset_projectRename(stage, target);
	if (res != FR_OK) preset_recoverProjectFolder(presetNr);
	return res;
}

//----------------------------------------------------
FRESULT preset_loadSongSD(uint8_t songNr)
{
	FRESULT res;
	char filename[13];
	UINT bytesRead;

	/*
	preset_makeFileNameLoad(filename,songNr,FEXT_SONG);
	//open the file
	res = f_open((FIL*)&SDFile,filename,FA_OPEN_EXISTING | FA_READ);
	*/
	res = preset_makeFileNameLoad(filename,songNr,FEXT_SONG);

	if(res!=FR_OK)
	{
		//try old filename
		preset_makeFileNameOld(filename,songNr,FEXT_SONG);
		res = f_open((FIL*)&SDFile,filename,FA_OPEN_EXISTING | FA_READ);
		if(res!=FR_OK)
			return res; //file open error... maybe the file does not exist?
	}

	//first the song name
	res = f_read((FIL*)&SDFile,(void*)savefileNames[songNr][SONG_NAMES],8,&bytesRead);

	if((!bytesRead) || (res != FR_OK))
		goto closeFile;

	if(preset_isNameEmpty(savefileNames[songNr][SONG_NAMES]))
	{
		sprintf((void*)savefileNames[songNr][SONG_NAMES], "Empty");
	}

	//read song data
	res = f_read((FIL*)&SDFile, seq_songData[songNr], MAX_SONG_LENGTH, &bytesRead);

	closeFile:
		//close the file handle
		f_close((FIL*)&SDFile);
		return res;
}
//----------------------------------------------------
FRESULT preset_saveSongSD(uint8_t songNr)
{
	FRESULT res;
	char filename[13];
	preset_makeFileNameSave(filename,songNr,FEXT_SONG, savefileNames[songNr][SONG_NAMES]);
	//open the file
	res = f_open((FIL*)&SDFile,filename,FA_CREATE_ALWAYS | FA_WRITE);
	if(res!=FR_OK)
		return res; //file open error... maybe the file does not exist?
	//write name
	res = preset_writeChecked(savefileNames[songNr][SONG_NAMES], 8);
	if (res != FR_OK)
	{
		f_close((FIL*)&SDFile);
		return res;
	}

	//write song data
	res = preset_writeChecked(seq_songData[songNr], MAX_SONG_LENGTH);
	if (res != FR_OK)
	{
		f_close((FIL*)&SDFile);
		return res;
	}

	//close the file handle
	res = f_close((FIL*)&SDFile);
	return res;
}


//----------------------------------------------------
void preset_loadProjectNames(uint8_t projectNr)
{
	FRESULT res;
	FILINFO fno;

	//change project folder
	sprintf(currentFolder, "/proj%02d", projectNr);
	res = preset_recoverProjectFolder(projectNr);
	if (res != FR_OK) {
		sprintf((void*)savefileNames[projectNr][PROJECT_NAMES], "Empty");
		return;
	}

	res = f_stat(currentFolder, &fno);
	if(res == FR_NO_FILE)
	{
		sprintf((void*)savefileNames[projectNr][PROJECT_NAMES], "Empty");
		return;
	}
	else
	{
		f_chdir(currentFolder);
	}

	//Load Project name

		UINT bytesRead;

		//open the file
		res = f_open((FIL*)&SDFile,"prj.nfo",FA_OPEN_EXISTING | FA_READ);
		if (res != FR_OK) {
			sprintf((void*)savefileNames[projectNr][PROJECT_NAMES], "Empty");
			preset_initTmpFolder();
			return;
		}

		//read preset name
		//first the preset name
		res=f_read((FIL*)&SDFile,(void*)savefileNames[projectNr][PROJECT_NAMES],8,&bytesRead);
		if(bytesRead != 8) {
			res=FR_DISK_ERR;
		}
		if(preset_isNameEmpty(savefileNames[projectNr][PROJECT_NAMES]))
		{
			sprintf((void*)savefileNames[projectNr][PROJECT_NAMES], "Empty");
		}

		f_close((FIL*)&SDFile);
		preset_initTmpFolder();

}
//----------------------------------------------------
FRESULT preset_loadProject(uint8_t presetNr, uint8_t clearTmp)
{
	FRESULT res;
	FILINFO fno;
	char text[17];

	lcd_process_display_interrupt = 0;
	while(sendDisplayBuffer() != 0);

#if USE_INTERRUPT_ONLY_DURING_SAVE_PROJ
	HAL_TIM_Base_Start_IT(&htim2);	//calc sample interrupt - prio 1
#else

#endif

	if(clearTmp)
	{
		preset_clearTmpFolder();
	}
	/*
	MX_FATFS_ReInit();
	f_mount(0, (TCHAR const*)SDPath, 0); //un-mount
	memset((FATFS*)&SDFatFS,0,sizeof(FATFS)); //clear FS object to force reinit

	res = f_mount((FATFS*)&SDFatFS, (TCHAR const*)SDPath,1);
	if( res != FR_OK)
	{
		return res;
	}
*/

	lcd_clear();
	lcd_home();
	lcd_string(PSTR("Loading Project"));
	lcd_setcursor(0,2);
	lcd_string(PSTR("0%"));

	//change project folder
	sprintf(currentFolder, "/proj%02d", presetNr);
	res = preset_recoverProjectFolder(presetNr);
	if (res != FR_OK) return res;

	res = f_stat(currentFolder, &fno);
	/* A missing project is handled by the existing create path below.
	 * Only reject an existing, incomplete or corrupt generation. */
	if (res == FR_OK && !preset_projectDirectoryValid(currentFolder)) {
		lcd_process_display_interrupt = 1;
		preset_initTmpFolder();
#if USE_INTERRUPT_ONLY_DURING_SAVE_PROJ
		HAL_TIM_Base_Stop_IT(&htim2);
#endif
		return FR_DISK_ERR;
	}
	if(res == FR_NO_FILE)
	{
		res = f_mkdir(currentFolder);
		if(res == FR_OK)
		{
			f_chdir(currentFolder);
		}
		else
		{
			lcd_process_display_interrupt = 1;
			preset_initTmpFolder();
#if USE_INTERRUPT_ONLY_DURING_SAVE_PROJ
			HAL_TIM_Base_Stop_IT(&htim2);	//calc sample interrupt - prio 1
#else

#endif
			return res;
		}
	}
	else
	{
		res = f_chdir(currentFolder);
		if (res != FR_OK)
		{
			lcd_process_display_interrupt = 1;
			preset_initTmpFolder();
#if USE_INTERRUPT_ONLY_DURING_SAVE_PROJ
			HAL_TIM_Base_Stop_IT(&htim2);	//calc sample interrupt - prio 1
#else

#endif
			return res;
		}
	}
	//load data

#if 1

	//Load Project name

	UINT bytesRead;

	//open the file
	res = f_open((FIL*)&SDFile,"prj.nfo",FA_OPEN_EXISTING | FA_READ);
	if (res != FR_OK)
	{
		lcd_process_display_interrupt = 1;
		preset_initTmpFolder();
#if USE_INTERRUPT_ONLY_DURING_SAVE_PROJ
			HAL_TIM_Base_Stop_IT(&htim2);	//calc sample interrupt - prio 1
#else

#endif
		return res;
	}
	//read preset name
	//first the preset name
	res = f_read((FIL*)&SDFile,(void*)savefileNames[presetNr][PROJECT_NAMES],8,&bytesRead);
	if (res != FR_OK)
	{
		lcd_process_display_interrupt = 1;
		preset_initTmpFolder();
#if USE_INTERRUPT_ONLY_DURING_SAVE_PROJ
			HAL_TIM_Base_Stop_IT(&htim2);	//calc sample interrupt - prio 1
#else

#endif
		f_close((FIL*)&SDFile);
		return res;
	}

	if(bytesRead != 8) {
		sprintf((void*)savefileNames[presetNr][PROJECT_NAMES], "Empty");
		/*
		//res=FR_DISK_ERR;
		lcd_process_display_interrupt = 1;
#if USE_INTERRUPT_ONLY_DURING_SAVE_PROJ
			HAL_TIM_Base_Stop_IT(&htim2);	//calc sample interrupt - prio 1
#else

#endif
		f_close((FIL*)&SDFile);
		return res;
		*/
	}

	else if(preset_isNameEmpty(savefileNames[presetNr][PROJECT_NAMES]))
	{
		sprintf((void*)savefileNames[presetNr][PROJECT_NAMES], "Empty");
	}
	//close the file
	f_close((FIL*)&SDFile);

	//load globals
	preset_loadGlobals();
	//load all drumkits, patterns and songs
	for(uint8_t i=0;i<NUM_KIT_SLOTS;i++)
	{

		sprintf(text, "%d%%", (uint8_t)(i/(float)NUM_KIT_SLOTS * 100.f));
		lcd_setcursor(0,2);
		lcd_string(text);

		//FR_NO_FILE just means an incomplete project and is no reason to abort!
		res = preset_loadDrumsetSD(i, 0);
		if ((res != FR_OK) && (res != FR_NO_FILE))
		{
			lcd_process_display_interrupt = 1;
			preset_initTmpFolder();
			f_close((FIL*)&SDFile);
#if USE_INTERRUPT_ONLY_DURING_SAVE_PROJ
			HAL_TIM_Base_Stop_IT(&htim2);	//calc sample interrupt - prio 1
#else

#endif
			return res;
		}
		res = preset_loadPatternSD(i);
		if ((res != FR_OK) && (res != FR_NO_FILE))
		{
			f_close((FIL*)&SDFile);
			lcd_process_display_interrupt = 1;
			preset_initTmpFolder();
#if USE_INTERRUPT_ONLY_DURING_SAVE_PROJ
			HAL_TIM_Base_Stop_IT(&htim2);	//calc sample interrupt - prio 1
#else

#endif
			return res;
		}
		res = preset_loadSongSD(i);
		if ((res != FR_OK) && (res != FR_NO_FILE))
		{
			f_close((FIL*)&SDFile);
			lcd_process_display_interrupt = 1;
			preset_initTmpFolder();
#if USE_INTERRUPT_ONLY_DURING_SAVE_PROJ
			HAL_TIM_Base_Stop_IT(&htim2);	//calc sample interrupt - prio 1
#else

#endif
			return res;
		}
	}

	res = FR_OK;

	//init morph target to peset 0
	morphParameters = mainParameters[0];


	//if pattern change mode is 'auto' load corresponding kit
	if(activeParameters[PAR_PATTERN_MODE] >= 1)
	{
		activeParameters[PAR_PATTERN_MODE] = 1; //prevent a value of 2 (skip next autoload) being saved with the project
		preset_loadDrumsetRam(seq_patternSet.kitNr[seq_activePattern], 0, 1);
		menu_selectedPresetNr[SAVE_TYPE_KIT] = seq_patternSet.kitNr[seq_activePattern];
		menu_loadedPresetNr[CURRENT_KIT] = seq_patternSet.kitNr[seq_activePattern];
		//refresh backup buffer with new kit, to prevent corruption when changin patterns with PCM=auto
		seq_copyPatternToBackupBuffer(seq_activePattern);
	}
	else
	{
		//init to preset 1
		//TODO maybe init to current active preset nr???
		preset_loadDrumsetRam(0,0, 1);
		menu_selectedPresetNr[SAVE_TYPE_KIT] = 0;
		menu_loadedPresetNr[CURRENT_KIT] = 0;
		seq_copyPatternToBackupBuffer(seq_activePattern);

	}

	preset_saveLastUsedProjectNrToSD(presetNr);

	//change pattern to pat 1
	seq_setNextPattern(0);

	lcd_process_display_interrupt = 1;
	preset_initTmpFolder();
#if USE_INTERRUPT_ONLY_DURING_SAVE_PROJ
			HAL_TIM_Base_Stop_IT(&htim2);	//calc sample interrupt - prio 1
#else

#endif

	return res;
#else
#if USE_SD_CARD
	//filename in 8.3  format
	char filename[GEN_BUF_LEN];
	UINT bytesRead;
	uint16_t remain;
	uint8_t siz;
	uint8_t version=0;
	//sprintf(filename,"p%03d.snd",presetNr);
	if(isAll)
		preset_makeFileName(filename,presetNr,FEXT_ALL);
	else
		preset_makeFileName(filename,presetNr,FEXT_PROJ);

	//open the file
	FRESULT res = f_open((FIL*)&SDFile,filename,FA_OPEN_EXISTING | FA_READ);

	if(res!=FR_OK)
		return; //file open error... maybe the file does not exist?

	//first the preset name
	f_read((FIL*)&SDFile,(void*)savefileNames[KIT_NAMES][presetNr],8,&bytesRead);
	if(!bytesRead)
		goto closeFile;

	// read version number and make sure its valid
	f_read((FIL*)&SDFile,&version,1,&bytesRead);
	if(!bytesRead || version > FILE_VERSION)
		goto closeFile;

	if(isAll){
		// read global data
		if(!preset_readGlobalData())
			goto closeFile;
		// size of padding after globals
		remain=	64- (NUM_PARAMS - PAR_BEGINNING_OF_GLOBALS);

	} else {
		// read bpm
		f_read((FIL*)&SDFile,&activeParameters[PAR_BPM],1,&bytesRead);
		if(!bytesRead)
			goto closeFile;

		// size of padding after these
		if(version>1) {
			// read pat reset bit
			f_read((FIL*)&SDFile,&activeParameters[PAR_BAR_RESET_MODE],1,&bytesRead);
			if(!bytesRead)
				goto closeFile;
			remain=	64- (1+1);
		} else
			remain=0; // version 1 didn't have padding for performances
	}

	while(remain){
		if(remain < GEN_BUF_LEN)
			siz=(uint8_t)remain;
		else
			siz=GEN_BUF_LEN;
		f_read((FIL*)&SDFile, filename, siz, &bytesRead);
		if(bytesRead !=siz)
			goto closeFile;
		remain -=siz;
	}



	// read kit data
	res=preset_readDrumsetData(presetNr, 0);
	if(res!=FR_OK)
		goto closeFile;

	// read padding
	remain = 512 - (END_OF_SOUND_PARAMETERS);
	while(remain){
		if(remain < GEN_BUF_LEN)
			siz=(uint8_t)remain;
		else
			siz=GEN_BUF_LEN;
		f_read((FIL*)&SDFile, filename, siz, &bytesRead);
		if(bytesRead !=siz)
			goto closeFile;
		remain -=siz;
	}


	// read pattern data
	/*//temporary removed
	if(!preset_readPatternData())
		goto closeFile;
		*/



closeFile:
	//close the file handle
	f_close((FIL*)&SDFile);

	// send drumset parameters to back
	preset_sendDrumsetParameters();

	// send global params
	menu_sendAllGlobals();

	//force complete repaint
	menu_repaintAll();

#else
	frontPanel_sendData(PRESET,PRESET_LOAD,presetNr);
#endif
#endif
}
//----------------------------------------------------

static void preset_makeFileNameOld(char *buf, uint8_t num, uint8_t type)
{

	const char *ext;
	buf[0]='p';
	numtostrpu(&buf[1],num,'0');
	switch(type) {
	case FEXT_SOUND:
		ext=PSTR(".snd");
		break;
	case FEXT_PAT:
		ext=PSTR(".pat");
		break;
	case FEXT_ALL:
		ext=PSTR(".all");
		break;
	default:
	case FEXT_PROJ:
		ext=PSTR(".prj");
		break;

	case FEXT_SONG:
		ext=PSTR(".sng");
		break;
	}
	strcpy(&buf[4],ext);

};


//load uses wildcard to find a name of the pattern 00-*.ext
static FRESULT preset_makeFileNameLoad(char *buf, uint8_t num, uint8_t type)
{

	FRESULT fr;     /* Return value */
    DIR dj;         /* Directory object */
    FILINFO fno;    /* File information */

	const char *ext;
	numtostrpu_2Bytes(&buf[0],num,'0');
	buf[2] = '-';
	buf[3] = '*';

	switch(type) {
	case FEXT_SOUND:
		ext=PSTR(".snd");
		break;
	case FEXT_PAT:
		ext=PSTR(".pat");
		break;
	case FEXT_ALL:
		ext=PSTR(".all");
		break;
	default:
	case FEXT_PROJ:
		ext=PSTR(".prj");
		break;

	case FEXT_SONG:
		ext=PSTR(".sng");
		break;
	}
	strcpy(&buf[4],ext);
	buf[8] = 0;


    fr = f_findfirst(&dj, &fno, "", buf);  /* Start to search for preset files */
    f_closedir(&dj);
    if(fr == FR_OK && fno.fname[0])
    {
    	fr = f_open((FIL*)&SDFile,fno.fname,FA_OPEN_EXISTING | FA_READ);
    	return fr;
    }

    return FR_NO_FILE;
}

void clearIllegalCharacters(char *input, char* output, uint8_t sz)
{
	for(uint8_t i=0; i<sz; i++)
	{
		if((input[i] >= 'a') && (input[i] <= 'z')) output[i] = input[i];
		else if((input[i] >= 'A') && (input[i] <= 'Z')) output[i] = input[i];
		else if((input[i] >= '0') && (input[i] <= '9')) output[i] = input[i];
		//! # $ % & ' ( ) - @ ^ _ ` { } ~
		else if(input[i] == ' ')  output[i] = '_';
		else if(input[i] == '!')  output[i] = input[i];
		else if(input[i] == '#')  output[i] = input[i];
		else if(input[i] == '$')  output[i] = input[i];
		else if(input[i] == '%')  output[i] = input[i];
		else if(input[i] == '&')  output[i] = input[i];
		else if(input[i] == '(')  output[i] = input[i];
		else if(input[i] == ')')  output[i] = input[i];
		else if(input[i] == '-')  output[i] = input[i];
		else if(input[i] == '@')  output[i] = input[i];
		else if(input[i] == '^')  output[i] = input[i];
		else if(input[i] == '_')  output[i] = input[i];
		else if(input[i] == '`')  output[i] = input[i];
		else if(input[i] == '{')  output[i] = input[i];
		else if(input[i] == '}')  output[i] = input[i];
		else if(input[i] == '~')  output[i] = input[i];
		else if(input[i] == 0)  output[i] = input[i];
		else output[i] = '_';
	}

}
//save uses the kit name to create a filename eg 00-NAME.ext
static void preset_makeFileNameSave(char *buf, uint8_t num, uint8_t type, char *name)
{

	char clearedName[9];
	clearedName[8] = 0;
	clearIllegalCharacters(name , clearedName, 8);
	//name scheme: 01-NAME.ext
	const char *ext;
	numtostrpu_2Bytes(&buf[0],num,'0');
	buf[2] = '-';

	//copy the first 5 characters of the name to the filename if not empty
	uint8_t i=0;
	for( ; i<5; i++)
	{
		if(clearedName[i] != 0)
		{
			if(clearedName[i] == ' ')
				buf[3+i] = '_';
			else
				buf[3+i] = clearedName[i];
		}
		else
		{
			break;
		}
	}

	switch(type) {
	case FEXT_SOUND:
		ext=PSTR(".snd");
		break;
	case FEXT_PAT:
		ext=PSTR(".pat");
		break;
	case FEXT_ALL:
		ext=PSTR(".all");
		break;
	default:
	case FEXT_PROJ:
		ext=PSTR(".prj");
		break;

	case FEXT_SONG:
		ext=PSTR(".sng");
		break;
	}
	strcpy(&buf[3+i],ext);
}

//----------------------------------------------------
/*
static void preset_makeFileNameTemp(char *buf, uint8_t num, uint8_t type)
{
	const char *ext;
	buf[0]='t';
	numtostrpu(&buf[1],num,'0');
	switch(type) {
	case 0:
		ext=PSTR(".snd");
		break;
	case 1:
		ext=PSTR(".pat");
		break;
	case 2:
		ext=PSTR(".all");
		break;
	default:
	case 3:
		ext=PSTR(".prf");
		break;
	}
	strcpy(&buf[4],ext);
}
*/
