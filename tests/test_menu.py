#!/usr/bin/env python3
"""Compile the real menu transition function against a minimal legacy-menu fixture."""
from pathlib import Path
import subprocess, os
root=Path(__file__).resolve().parents[1]
s=(root/'Core/Src/menu/menu.c').read_text(encoding='latin1')
start=s.index('void menu_switchSubPage(uint8_t subPageNr, uint8_t toggle)')
end=s.index('\n}\n',start)+3
fn=s[start:end]
fixture=r'''
#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include "Soundengine/DSPAudio/Jimbones.h"
#define MASK_PARAMETER 7
#define MASK_PAGE 0xf8
#define PAGE_SHIFT 3
#define NUM_SUB_PAGES 8
#define MENU_MIDI_PAGE 7
#define SEQ_PAGE 10
#define TEXT_EMPTY 0
#define pgm_read_byte(p) (*(p))
static struct { uint8_t top1; } menuPages[20][8];
static uint8_t menuIndex,menu_activePage,editModeActive;
static uint8_t jb_screen,jb_selected=JB_METAL,jb_oscArmed;
enum { JB_EXT_NONE=0, JB_EXT_MORPH, JB_EXT_HAT, JB_EXT_SNARE, JB_EXT_VELOCITY, JB_EXT_FMDRUM, JB_EXT_CLICK, JB_EXT_BASS, JB_EXT_COMB, JB_EXT_COMB_ROUTE, JB_EXT_TAPE, JB_EXT_LOFI, JB_EXT_CLUSTER, JB_EXT_METAL, JB_EXT_WAVE };
static uint8_t jb_extPage,jb_extArmed,jb_extSlot;
static void jb_extSyncKit(void) {}
static uint8_t jb_extCount(uint8_t n) { if(n==0||n==5)return 3; if(n==2)return 4; if(n==7)return 2; return (n==3||n==4)?1:0; }
static uint8_t jb_extSelect(uint8_t n,unsigned slot) { static const uint8_t mod[]={JB_EXT_MORPH,JB_EXT_HAT,JB_EXT_SNARE,JB_EXT_VELOCITY}; static const uint8_t filter[]={JB_EXT_BASS,JB_EXT_COMB,JB_EXT_COMB_ROUTE}; static const uint8_t mix[]={JB_EXT_TAPE,JB_EXT_LOFI}; static const uint8_t osc[]={JB_EXT_METAL,JB_EXT_WAVE,JB_EXT_CLUSTER}; if(n==0)return osc[slot%3u];if(n==2)return mod[slot%4u];if(n==3)return JB_EXT_FMDRUM;if(n==4)return JB_EXT_CLICK;if(n==5)return filter[slot%3u];if(n==7)return mix[slot%2u];return JB_EXT_NONE; }
static uint8_t menu_getSubPage(void) { return (menuIndex&MASK_PAGE)>>PAGE_SHIFT; }
static int has2ndPage(unsigned p) { return p==2; }
'''
test=r'''
int main(void) {
 for(unsigned voice=0;voice<7;voice++) {
  menu_activePage=voice; menuIndex=3<<3; jb_screen=jb_oscArmed=0;
  menu_switchSubPage(0,1); assert(!jb_screen && menuIndex==0);
  menu_switchSubPage(0,1); assert(jb_screen && jb_selected==JB_MODE);
  for(unsigned page=1;page<JB_PARAMS/4;page++) { menu_switchSubPage(0,1); assert(jb_screen && jb_selected==page*4); }
  menu_switchSubPage(0,1); assert(jb_screen && jb_selected==JB_PARAMS); /* ninth wave interpolation page */
  menu_switchSubPage(0,1); assert(!jb_screen);
  menu_switchSubPage(0,1); assert(jb_screen && jb_selected==JB_MODE);
  menu_switchSubPage(1,1); assert(!jb_screen && !jb_oscArmed && menu_getSubPage()==1);
  menu_switchSubPage(0,1); assert(!jb_screen && menuIndex==0);
  menu_switchSubPage(0,1); assert(jb_screen);
  menu_switchSubPage(2,1); assert(!jb_screen && menuIndex==16 && !jb_extPage);
  menu_switchSubPage(2,1); assert(jb_extPage==JB_EXT_MORPH);
  menu_switchSubPage(2,1); assert(jb_extPage==JB_EXT_HAT);
  menu_switchSubPage(2,1); assert(jb_extPage==JB_EXT_SNARE);
  menu_switchSubPage(2,1); assert(jb_extPage==JB_EXT_VELOCITY);
  menu_switchSubPage(2,1); assert(!jb_extPage);
  menu_switchSubPage(5,1); assert(menu_getSubPage()==5 && !jb_extPage);
  menu_switchSubPage(5,1); assert(jb_extPage==JB_EXT_BASS);
  menu_switchSubPage(5,1); assert(jb_extPage==JB_EXT_COMB);
  menu_switchSubPage(5,1); assert(jb_extPage==JB_EXT_COMB_ROUTE);
  menu_switchSubPage(5,1); assert(!jb_extPage);
  menu_switchSubPage(3,1); assert(menu_getSubPage()==3 && !jb_extPage);
  menu_switchSubPage(3,1); assert(jb_extPage==JB_EXT_FMDRUM);
  menu_switchSubPage(3,1); assert(!jb_extPage);
  menu_switchSubPage(4,1); assert(menu_getSubPage()==4 && !jb_extPage);
  menu_switchSubPage(4,1); assert(jb_extPage==JB_EXT_CLICK);
  menu_switchSubPage(4,1); assert(!jb_extPage);
  menu_switchSubPage(7,1); assert(menu_getSubPage()==7 && !jb_extPage);
  menu_switchSubPage(7,1); assert(jb_extPage==JB_EXT_TAPE);
  menu_switchSubPage(7,1); assert(jb_extPage==JB_EXT_LOFI);
  menu_switchSubPage(7,1); assert(!jb_extPage);
 }
 menu_activePage=MENU_MIDI_PAGE; menuIndex=0; menu_switchSubPage(0,1); assert(!jb_screen);
 puts("PASS native agent submenu router: first OSC original, eight macro banks plus interpolation bank, exit, six voices plus open-hat page, MOD/FM/CLICK/FILTER/MIX extension cycles and global behavior");
}
'''
out=root/'build/host/menu_fixture.c'; out.write_text(fixture+fn+test)
subprocess.run([os.environ.get('CC','cc'),'-std=c11','-Wall','-Wextra','-Werror','-I'+str(root/'Core/Inc'),str(out),'-o',str(out.with_suffix(''))],check=True)
subprocess.run([str(out.with_suffix(''))],check=True)
