// main.c

/* Build commands:
/Applications/Emulation/cc65-master/bin/cl65 -O -t atari -C config.cfg main.c graphics.c text.c tiles.c interrupts.s -o adv.xex
open adv.xex
*/

// == About This Program ==
// This program is a rogue-like game. 

// == Notes on Memory Usage ==
// In the config.cfg file, the reserved memory is set to 0x0820, giving us 2KB of space below the display area, which itself takes 1KB. The space between APPMHI and MEMTOP should be ours to use. 
// Calculate 3KB from RAMTOP, which is 12*256 bytes.
// RAMTOP: 0xC0 without BASIC, 0xA0 with BASIC. Below are values with BASIC.
// 0x9400: Custom character set needs 128 chars * 8 bytes = 1024 bytes.
// 0x9800: PMGraphics needs 640 bytes (double-line sprites), but the data area doesn't start until 384 bytes after PMBase, which must be on 1KB boundary.
// 0x9C00: Display list and screen memory has 1024 bytes allocated, so that the graphics driver can do its thing. We overwrite this area with our own display list, which takes up less space than the standard text screen.


// Includes
#include <atari.h>
#include <conio.h>
#include "atari_memmap.h"
#include "graphics.h"
#include "text.h"
#include "tiles.h"
#include "types.h"


// Globals
UInt8 gQuit;
UInt8 gMapX, gMapY;


// Constants
const UInt8 mapHeight = 11;
const UInt8 mapWidth = 11;


// Function prototypes
void runLoop(void);
void handleStick(void);


// == main() == 
int main (void) {
	// Graphics
	initGraphics();
	
	// Put something on screen
	fillScreen();

	// Start new game
	gQuit = 0;
	gMapX = 5;
	gMapY = 5;
	
	while (gQuit == 0) {
		//startNewGame();
		runLoop();
		POKE(ATRACT, 0);
	}

	return 0; // success
}

// == runLoop() ==
void runLoop(void) {
	handleStick();
}

// == handleStick() ==
void handleStick() {
	// Only allow moves in 4 cardinal directions and not diagonals.
	UInt8 stick = PEEK (STICK0);
	UInt8 trigger = PEEK (STRIG0);
	UInt8 oldX = gMapX;
	UInt8 oldY = gMapY;
	
	switch (stick) {
		case 0x0E: // up
			if (gMapY > 0) {
				--gMapY;
			}
			break;
		case 0x0D: // down
			if (gMapY < mapHeight - 1) {
				++gMapY;
			}
			break;
		case 0x0B: // left
			if (gMapX > 0) {
				--gMapX;
			}
			break;
		case 0x07: // right
			if (gMapX < mapWidth - 1) {
				++gMapX;
			}
			break;
		default:
			break;
	}
	
	if (oldX != gMapX || oldY != gMapY) {
		drawMap(sampleMap, mapWidth, mapHeight, gMapX, gMapY);
	}

}

