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
  menu_switchSubPage(2,1); assert(!jb_screen && menuIndex==16);
  menu_switchSubPage(2,1); assert(menuIndex==20);
  menu_switchSubPage(2,1); assert(menuIndex==16);
 }
 menu_activePage=MENU_MIDI_PAGE; menuIndex=0; menu_switchSubPage(0,1); assert(!jb_screen);
 puts("PASS actual menu transition: first OSC original, second macros, all layer pages, exit, six voices plus open-hat page, legacy second page/global behavior");
}
'''
out=root/'build/host/menu_fixture.c'; out.write_text(fixture+fn+test)
subprocess.run([os.environ.get('CC','cc'),'-std=c11','-Wall','-Wextra','-Werror','-I'+str(root/'Core/Inc'),str(out),'-o',str(out.with_suffix(''))],check=True)
subprocess.run([str(out.with_suffix(''))],check=True)
