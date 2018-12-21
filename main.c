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
UInt8 previousStick;
UInt8 previousTrigger;

PointU8 playerOverworldLocation;
PointU8 playerMapLocation;
UInt8 hasBoat;
UInt8 hasShip;
UInt8 hasLamp;
UInt8 hasCrystal;
UInt8 sightDistance;

// Function prototypes
void runLoop(void);
void handleStick(void);
void handleTrigger(void);
UInt8 canMoveTo(PointU8*);
UInt8 mapTileAt(PointU8 *pt);

void exitToOverworld(void);
void enterDungeon(PointU8 *location);

void updateStatText(void);

// Constants
#define STICK_TIMER (0x0601)



// == main() == 
int main (void) {
	// Graphics
	initGraphics();
	
	// Start new game
	gQuit = 0;
	previousStick = 0x0F;
	previousTrigger = 1;
	hasBoat = 0;
	hasShip = 0;
	hasLamp = 1;
	hasCrystal = 1;
	sightDistance = 0xFF;
	playerOverworldLocation = overworldEntryPoint;

	// Load map
	exitToOverworld();

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
	handleTrigger();
}

// == handleStick() ==
void handleStick() {
	// Only allow moves in 4 cardinal directions and not diagonals.
	UInt8 stick = PEEK (STICK0);
	UInt8 stick_timer = PEEK (STICK_TIMER);
	PointU8 oldLoc, newLoc;

	if (stick == previousStick && stick_timer != 0) { 
		// Handle changes in stick position immediately but delay repeating same moves.
		return;
	}
	POKE(STICK_TIMER, 10); // Reset stick timer
	previousStick = stick;

	oldLoc = playerMapLocation;
	newLoc = oldLoc;
	
	switch (stick) {
		case 0x0E: --newLoc.y; break; // up
		case 0x0D: ++newLoc.y; break; // down
		case 0x0B: --newLoc.x; break; // left
		case 0x07: ++newLoc.x; break; // right
		default: break;
	}
	
	if (oldLoc != newLoc) {
		// Check map bounds. Because newLoc is unsigned, it wraps around from 0 to 255.
		if (newLoc.x < currentMapSize.width && newLoc.y < currentMapSize.height) {
			if (canMoveTo(&newLoc)) {
				playerMapLocation = newLoc;
				drawCurrentMap(&playerMapLocation);
			}
		} else {
			// Handle moving off the map for towns
			if (currentMapType == TownMapType) {
				exitToOverworld();
			}
		}
	}

}

// == handleTrigger() ==
void handleTrigger(void) {
	// Recognize trigger only when it transitions from up to down.
	UInt8 trigger = PEEK (STRIG0);
	UInt8 tile;
	if (trigger != previousTrigger) {
		if (trigger == 0) { // trigger down
			tile = mapTileAt(&playerMapLocation);
			switch (currentMapType) {
				case OverworldMapType:
					if (tile >= 9) { // Castle or higher
						enterDungeon(&playerMapLocation);
					}
					break;
				case DungeonMapType:
					if (tile == 3) { // Ladder
						exitToOverworld();
					} else if (tile == 4) { // Chest
						// TODO
					}
					break;
				case TownMapType:
					if (tile == 13) { // House door
						// TODO
					}
					break;
			}

		}
	}
	previousTrigger = trigger;

}

// == canMoveTo() ==
UInt8 canMoveTo(PointU8 *pt) {
	UInt8 tile = mapTileAt(pt);

	switch (currentMapType) {
		case OverworldMapType:
			if (tile == 5) { // Shallows
				return (hasBoat || hasShip);
			} else if (tile == 6) {
				return hasShip;
			}
			return 1;
		case DungeonMapType:
			return tile > 1;
		case TownMapType:
			return tile == 1 || tile == 3 || tile == 13;
	}

	return 1;
}

// == mapTileAt() ==
UInt8 mapTileAt(PointU8 *pt) {
	//return currentRawMap[pt->x + currentMapSize.width * pt->y];
	// TODO: rewrite to use currentRunLenMap 
	return 0;
}

// == exitToOverworld() ==
void exitToOverworld(void) {
	loadColorTable(NULL);
	clearMapScreen();
	loadMap(OverworldMapType, 0);
	playerMapLocation = playerOverworldLocation;
	sightDistance = 0xFF;
	layoutCurrentMap(sightDistance);
	drawCurrentMap(&playerMapLocation);
	loadColorTable(overworldColorTable);
}

// == enterDungeon() == 
void enterDungeon(PointU8 *location) {
	UInt8 mapType;
	const UInt8 *colorTable;

	// Also for entering towns.
	if (currentMapType != OverworldMapType) {
		return;
	}

	// Save player location in overworld to be restored when exiting.
	playerOverworldLocation = playerMapLocation;

	// TODO: determine which dungeon/town to load based on y location,
	// since each special tile should only exist 1 per map line.

	if (location->x > 8) {
		mapType = DungeonMapType;
		colorTable = dungeonColorTable;
		playerMapLocation = dungeonEntryPoint;
		sightDistance = 0;
		if (hasLamp) {
			++sightDistance;
		}
		if (hasCrystal) {
			++sightDistance;
		}
	} else {
		mapType = TownMapType;
		colorTable = townColorTable;
		playerMapLocation = townEntryPoint;
		sightDistance = 0xFF;
	}

	loadColorTable(NULL);
	clearMapScreen();
	loadMap(mapType, 0);
	layoutCurrentMap(sightDistance);
	drawCurrentMap(&playerMapLocation);
	loadColorTable(colorTable);
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

