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
#include "tem_shop.h"
#include "text.h"
#include "types.h"
#include <atari.h>


// Globals
UInt8 isQuitting;
UInt8 soundChannel; 
UInt8 soundOctave;

// Dialog functions

static void fadeOutScreen(void) {
	// Fade out
	stopSong();
	fadeOutColorTable();
	clearSprite(4);
	hideSprites();
	setDliColorTable(0);
}

static void handleMessage(SInt8 message) {
	switch (message) {
		case MessageEnterDialog:
			fadeOutScreen();
			initDialog();
			break;
		case MessageEnterShop:
			fadeOutScreen();
			initTemShop();
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

static void keyDown(UInt8 keycode) {
	/* Musical keyboard:
		Use keys on the bottom row from Z to M and the comma key for the white keys.
		Use the home rome for black keys.
		Q: switches to wavetable synth for bass notes (-2 octaves)
		W: switches to normal square-wave tone.
		Keys 1-4: switches octaves.
		Keys 8, 9, 0: starts a song.
		Space: stops the song.
	*/
	UInt8 shift = keycode & 0x40;
	UInt8 control = keycode & 0x80;
	UInt8 note = 0xFF;
	const UInt8 vol = 8;

	switch (keycode & 0x3F) {
		case KEY_Z:
			note = 1; 
			break;
		case KEY_S:	
			note = 2;
			break;
		case KEY_X:	
			note = 3;
			break;
		case KEY_D:	
			note = 4;
			break;
		case KEY_C:	
			note = 5;
			break;
		case KEY_V:	
			note = 6;
			break;
		case KEY_G:	
			note = 7;
			break;
		case KEY_B:	
			note = 8;
			break;
		case KEY_H:
			note = 9;
			break;
		case KEY_N:	
			note = 10;
			break;
		case KEY_J:	
			note = 11;
			break;
		case KEY_M:	
			note = 12;
			break;
		case KEY_COMMA:	
			note = 13;
			break;

		case KEY_1:
			soundOctave = 0;
			break;
		case KEY_2:
			soundOctave = 12;
			break;
		case KEY_3:
			soundOctave = 24;
			break;
		case KEY_4:
			soundOctave = 36;
			break;

		case KEY_Q:
			soundChannel = 0;
			break;
		case KEY_W:
			soundChannel = 1;
			break;

		case KEY_8:
			startSong(0);
			break;
		case KEY_9:
			startSong(4);
			break;
		case KEY_0:
			startSong(8);
			break;
		case KEY_SPACE:
			stopSong();
			break;
		default:
			break;
	}
	if (note != 0xFF) {
		if (shift) {
			note += 12;
		}
		if (control) {
			note += 24;
		}
		note += soundOctave;

		if (soundChannel == 0) {
			// Bass
			noteOn(note, 32, 15, 3, 0xE0, soundChannel);
		} else {
			noteOn(note, 32, vol, 4, 0xE0, soundChannel);
		}
	}
}

static void keyUp(UInt8 /* keycode */) {
	noteOff(soundChannel);
}

static void handleKeyboard(void) {
	static UInt8 previousKeycode = 0xFF;
	static UInt8 previousKeydown = 0;

	UInt8 isDown = (POKEY_READ.skstat & 0x04) == 0;
	UInt8 keycode = POKEY_READ.kbcode; // was POKEY_READ.kbcode

	if (keycode != previousKeycode) {
		keyUp(previousKeycode);
		keyDown(keycode);
	} else if (previousKeydown == 0 && isDown != 0) {
		keyDown(keycode);
	} else if (previousKeydown != 0 && isDown == 0) {
		keyUp(keycode);
	}
	previousKeydown = isDown;
	previousKeycode = keycode;
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
	soundChannel = 1;
	soundOctave = 12;

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
