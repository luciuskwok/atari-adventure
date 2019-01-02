// main.c


// == About This Program ==
// This program is a rogue-like game. 

/* Build command on Mac:
	/Applications/Emulation/cc65-master/bin/cl65 -O -t atari -C config.cfg -m linker_map.txt main.c battle.c cursor.c dialog.c game_chara.c graphics.c info.c map.c map_asm.s map_data.c menu.c puff.c puff_asm.s sprites.c text.c text_asm.s interrupts.s -o adv.xex
*/

// Includes
#include "atari_memmap.h"
#include "battle.h"
#include "cursor.h"
#include "dialog.h"
#include "game_chara.h"
#include "graphics.h"
#include "info.h"
#include "map.h"
#include "map_data.h"
#include "sound.h"
#include "sprites.h"
#include "text.h"
#include "types.h"
#include <atari.h>


// Globals
UInt8 isQuitting;

// Constants and macros
//#define DEBUGGING

// Debugging
#ifdef DEBUGGING
UInt16 startTime;
UInt16 duration;
#endif

void __fastcall__ testTriggerNote(UInt8 note);

// Dialog functions

static void fadeOutScreen(void) {
	// Fade out
	stopSound();
	fadeOutColorTable(FadeTextBox);
	clearSprite(4);
	hideSprites();
}

static void handleMessage(SInt8 message) {
	switch (message) {
		case MessageEnterDialog:
			fadeOutScreen();
			initDialog();
			break;
		case MessageEnterBattle:
			fadeOutScreen();
			initBattle();
			break;
		case MessageEnterInfo:
			fadeOutScreen();
			initInfo();
			break;		
		case MessageReturnToMap:
			fadeOutScreen();
			initMap();
			break;
	}
}

static void handleKeyboard(void) {
	static UInt8 previousKeycode = 0xFF;
	UInt8 keycode = POKEY_READ.kbcode;
	const UInt8 vol = 8;
	const UInt8 chan = 3;
	UInt8 note = 0xFF;
	UInt8 shift = keycode & 0x40;
	UInt8 control = keycode & 0x80;

	if (keycode != previousKeycode) {
		switch (keycode & 0x3F) {
			case KEY_A:
				note = 0; 
				break;
			case KEY_W:	
				note = 1;
				break;
			case KEY_S:	
				note = 2;
				break;
			case KEY_E:	
				note = 3;
				break;
			case KEY_D:	
				note = 4;
				break;
			case KEY_F:	
				note = 5;
				break;
			case KEY_T:	
				note = 6;
				break;
			case KEY_G:	
				note = 7;
				break;
			case KEY_Y:	
				note = 8;
				break;
			case KEY_H:	
				note = 9;
				break;
			case KEY_U:	
				note = 10;
				break;
			case KEY_J:	
				note = 11;
				break;
			case KEY_K:	
				note = 12;
				break;
			case KEY_Z:
				startSequence();
				break;
			case KEY_X:
				stopSound();
				break;
			default:
				break;
		}
		if (note == 0xFF) {
			noteOff(chan);
		} else {
			if (shift) {
				note += 12;
			}
			if (control) {
				note += 24;
			}
			// noteOn(note, vol, chan);
			testTriggerNote(note);
		}
		previousKeycode = keycode;
	}
}

void runLoop(void) {
	handleMessage(handleTrigger());
	handleMessage(handleStick());
	handleKeyboard();
}

int main (void) {
	// Init
	initGraphics();
	initCursor();
	initSound();
	
	// Start new game
	initParty();
	isQuitting = 0;

	currentMapType = OverworldMapType;
	mapShipType = 0;
	mapLampStrength = 2;
	mapSightDistance = 0xFF;
	mapOverworldLocation = overworldEntryPoint;
	mapCurrentLocation = mapOverworldLocation;

	initMap();
	
	while (isQuitting == 0) {
		runLoop();
		resetAttractMode();
	}

	return 0; // success
}
