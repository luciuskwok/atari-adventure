// main.c


// == About This Program ==
// This program is a rogue-like game. 

/* 
	Build command on Mac:
	/Applications/Emulation/cc65-master/bin/cl65 -O -t atari -C config.cfg -m linker_map.txt main.c battle.c cursor.c dialog.c game_chara.c graphics.c info.c map.c map_data.c menu.c misc_asm.s puff.c puff_asm.s sound.s sprites.c text.c interrupts.s -o adv.xex

	Build command on Windows:
	\\cc65-snapshot-win32\\bin\\cl65 -O -t atari -C config.cfg -m linker_map.txt main.c battle.c cursor.c dialog.c game_chara.c graphics.c info.c map.c map_data.c menu.c misc_asm.s puff.c puff_asm.s sound.s sprites.c text.c interrupts.s -o adv.xex
*/

/*  == Notes on Memory Usage ==
	
	In the config.cfg file, the reserved memory is set to 0x1020, giving us 4KB 
	of space below the 1 KB display area for a total of 5KB. The space between 
	APPMHI and MEMTOP should be ours to use. 

	RAMTOP: 0xC0 without BASIC, 0xA0 with BASIC. Below are values without BASIC.

	0xB400: 3 KB: Display list and screen memory, within 4 KB boundaries.
	0xB000: 1 KB: PMGraphics needs 640 bytes (double-line sprites), but the data area 
		doesn't start until 384 (0x180) bytes after PMBase, within 1KB boundaries.
	0xAC00: 1 KB: Custom character set needs 128 chars * 8 bytes = 1024 bytes.

	Screen memory is allocated:
	- Map View:
		- Display List: 32 bytes
		- Screen memory: 24x9 = 216 bytes
	- Story View:
		- Display List: 96 bytes
		- Screen memory: 40x72 = 2,880 bytes
	- Total: about 3,008 bytes if screen memory is shared between the 2 views.
	- Shared text window: 40x7 = 280 bytes. Goes into memory hole at start of PMGraphics.

	- Display list should not cross a 1KB boundary (0x0400)
	- Screen memory should not cross a 4KB boundary (0x1000)
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

// Dialog functions

static void fadeOutScreen(void) {
	// Fade out
	stopSong();
	fadeOutColorTable();
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
	UInt8 keycode = PEEK(CH_); // was POKEY_READ.kbcode
	const UInt8 vol = 8;
	UInt8 note = 0xFF;
	UInt8 shift = keycode & 0x40;
	UInt8 control = keycode & 0x80;

	switch (keycode & 0x3F) {
		case KEY_Q:
			note = 1; 
			break;
		case KEY_2:	
			note = 2;
			break;
		case KEY_W:	
			note = 3;
			break;
		case KEY_3:	
			note = 4;
			break;
		case KEY_E:	
			note = 5;
			break;
		case KEY_R:	
			note = 6;
			break;
		case KEY_5:	
			note = 7;
			break;
		case KEY_T:	
			note = 8;
			break;
		case KEY_6:
			note = 9;
			break;
		case KEY_Y:	
			note = 10;
			break;
		case KEY_7:	
			note = 11;
			break;
		case KEY_U:	
			note = 12;
			break;
		case KEY_I:	
			note = 13;
			break;
		case KEY_Z:
			startSong(0);
			note = 0xFE;
			break;
		case KEY_X:
			startSong(4);
			note = 0xFE;
			break;
		case KEY_C:
			startSong(8);
			note = 0xFE;
			break;
		case KEY_SPACE:
			stopSong();
			note = 0xFE;
			break;
		default:
			break;
	}
	
	if (note != 0xFF) {
		if (note != 0xFE) {
			if (shift) {
				note += 12;
			}
			if (control) {
				note += 24;
			}
			//noteOn(note, 4, 7, 3, 0xE0, 3);
			noteOn(note, 1, 7, 15, 0x80, 3);
		}
		POKE(CH_, 0xFF);
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
	
	// Start new game
	initParty();
	isQuitting = 0;

	currentMapType = OverworldMapType;
	playerShipType = 0;
	playerLampStrength = 2;
	playerSightDistance = 0xFF;
	playerOverworldLocation = overworldEntryPoint;
	playerLocation = playerOverworldLocation;

	initMap();
	
	while (isQuitting == 0) {
		runLoop();
		resetAttractMode();
	}

	return 0; // success
}
