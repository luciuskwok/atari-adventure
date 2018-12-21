// main.c

/* Build commands:
/Applications/Emulation/cc65-master/bin/cl65 -O -t atari -C config.cfg main.c graphics.c text.c tiles.c interrupts.s -o adv.xex
open adv.xex
*/

// == About This Program ==
// This program is a rogue-like game. 

/*
== Notes on Memory Usage ==
In the config.cfg file, the reserved memory is set to 0x0820, giving us 2KB 
of space below the display area, which itself takes 1KB. The space between 
APPMHI and MEMTOP should be ours to use. 

Calculate 3KB from RAMTOP, which is 12*256 bytes.
RAMTOP: 0xC0 without BASIC, 0xA0 with BASIC. Below are values with BASIC.
0x9400: Custom character set needs 128 chars * 8 bytes = 1024 bytes.
0x9800: PMGraphics needs 640 bytes (double-line sprites), but the data area 
	doesn't start until 384 bytes after PMBase, which must be on 1KB boundary.
0x9C00: Display list and screen memory have 1024 bytes allocated, so that the 
	graphics driver can do its thing. We overwrite this area with our own 
	display list, which takes up less space than the standard text screen.
*/

// Includes
#include <atari.h>
#include "atari_memmap.h"
#include "graphics.h"
#include "text.h"
#include "tiles.h"
#include "types.h"


// Globals
UInt8 gQuit;
PointU8 playerOverworldLocation;
PointU8 playerMapLocation;
UInt8 isPlayerInOverworld;

// Function prototypes
void runLoop(void);
void handleStick(void);
void updateStatText(void);


// == main() == 
int main (void) {
	// Graphics
	initGraphics();
	
	// Start new game
	gQuit = 0;
	isPlayerInOverworld = 0;
	playerOverworldLocation = overworldEntryPoint;

	// Load map
	//loadMap(OverworldMap);
	loadMap(TownMap);
	playerMapLocation = townEntryPoint;
	drawCurrentMap(&playerMapLocation, 0);

	// Print some text
	updateStatText();
	//printAllTiles();

	
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
	UInt8 oldX = playerMapLocation.x;
	UInt8 oldY = playerMapLocation.y;
	UInt8 newX = oldX;
	UInt8 newY = oldY;
	UInt8 mapWidth = currentMapSize.width;
	UInt8 mapHeight = currentMapSize.height;
	
	switch (stick) {
		case 0x0E: // up
			if (newY > 0) {
				--newY;
			}
			break;
		case 0x0D: // down
			if (newY < mapHeight - 1) {
				++newY;
			}
			break;
		case 0x0B: // left
			if (newX > 0) {
				--newX;
			}
			break;
		case 0x07: // right
			if (newX < mapWidth - 1) {
				++newX;
			}
			break;
		default:
			break;
	}
	
	if (oldX != newX || oldY != newY) {
		playerMapLocation.x = newX;
		playerMapLocation.y = newY;
		drawCurrentMap(&playerMapLocation, 0);
	}
}

// == updateStatText() ==
void updateStatText(void) {
	clearTextWindow();

	// Print header column
	printString("HP:", 0, 2);

	// Print character statistics
	printCharaStats(0, "Alisa", 99, 123, 255);
	printCharaStats(1, "Marie", 1, 4, 8);
	printCharaStats(2, "Guy", 19, 67, 72);
	printCharaStats(3, "Nyorn", 7, 78, 180);

	// Print party statistics
	printPartyStats(987123, 21, 1325, -891);

}

