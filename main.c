// main.c


// == About This Program ==
// This program is a rogue-like game. 

/*
== Notes on Memory Usage ==
In the config.cfg file, the reserved memory is set to 0x0820, giving us 2KB 
of space below the display area, which itself takes 1KB. The space between 
APPMHI and MEMTOP should be ours to use. 

RAMTOP: 0xC0 without BASIC, 0xA0 with BASIC. Below are values with BASIC.

0x9400: Custom character set needs 128 chars * 8 bytes = 1024 bytes.
0x9800: PMGraphics needs 640 bytes (double-line sprites), but the data area 
	doesn't start until 384 bytes after PMBase, which must be on 1KB boundary.
0x9C00: Display list and screen memory have 1024 bytes allocated, so that the 
	graphics driver can do its thing. We overwrite this area with our own 
	display list, which takes up less space than the standard text screen.

Screen memory is allocated:
- Map View:
	- Display List: ?? bytes
	- Screen memory: 24x9 = 216 bytes
- Story View:
	- Display LIst: ?? bytes
	- Screen memory: 40x72 = 2,880 bytes
*/

// Includes
#include <atari.h>
#include "atari_memmap.h"
#include "graphics.h"
#include "map.h"
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
UInt8 lampStrength;
UInt8 sightDistance;

// Function prototypes
void runLoop(void);
void resetAttractMode(void);
void handleStick(void);
void handleTrigger(void);
UInt8 canMoveTo(PointU8*);

void exitToOverworld(void);
void enterDungeon(void);
void enterTown(void);
void printStatText(void);

void presentDialog(void);
void waitForAnyInput(void);



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
	lampStrength = 2;
	sightDistance = 0xFF;
	playerOverworldLocation = mapEntryPoint(OverworldMapType);

	// Load map
	exitToOverworld();
	setTextWindowColorTheme(0);
	printStatText();
	setPlayerCursorVisible(1);
	
	while (gQuit == 0) {
		//startNewGame();
		runLoop();
		resetAttractMode();
	}

	return 0; // success
}



void runLoop(void) {
	handleStick();
	handleTrigger();
}


void resetAttractMode(void) {
	POKE(ATRACT, 0);
}


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
	UInt8 trigger = PEEK (STRIG0);
	UInt8 tile;

	// Recognize trigger only when it transitions from up to down.
	if (trigger == previousTrigger) {
		return;
	}
	previousTrigger = trigger;
	if (trigger != 0) {  // trigger == 0 when it is pressed
		return;
	}

	// Get the tile without color info.
	tile = mapTileAt(&playerMapLocation) & 0x3F; 
	switch (tile) {
		case tTown:
			enterTown();
			break;
		case tVillage:
		case tCastle:
			presentDialog();
			break;
		case tMonument:
		case tCave:
			enterDungeon();
			break;
		case tHouseDoor:
			presentDialog();
			break;
		case tLadder:
			exitToOverworld();
	}
}


UInt8 canMoveTo(PointU8 *pt) {
	UInt8 tile = mapTileAt(pt) & 0x3F; // remove color data

	switch (currentMapType) {
		case OverworldMapType:
			if (tile == tShallows) { // Shallows
				return (hasBoat || hasShip);
			} else if (tile == tWater) {
				return hasShip;
			}
			return 1;
		case DungeonMapType:
			return tile != tBrick;
		case TownMapType:
			return tile == tSolid || tile == tHouseDoor;
	}

	return 1;
}


void exitToOverworld(void) {
	playerMapLocation = playerOverworldLocation;
	sightDistance = 0xFF;
	loadMap(OverworldMapType, sightDistance, &playerMapLocation);

}


void enterDungeon(void) {
	sightDistance = lampStrength;
	playerOverworldLocation = playerMapLocation;
	playerMapLocation = mapEntryPoint(DungeonMapType);
	loadMap(DungeonMapType, sightDistance, &playerMapLocation);

}

void enterTown(void) {
	sightDistance = 0xFF;
	playerOverworldLocation = playerMapLocation;
	playerMapLocation = mapEntryPoint(TownMapType);
	loadMap(TownMapType, sightDistance, &playerMapLocation);
}


void printStatText(void) {
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


// ==== Dialog ====


void presentDialog(void) {
	UInt8 dialogColorTable[] = {
		0x0F, 0x0F, 0x00, 0x00, // white/black for sprites
		0x00, 0x06, 0x0A, 0x0E, 0x94 // grayscale for playfield
	};
	UInt8 gradient[] = { 0x92, 0x94, 0x94, 0x96, 0xC2, 0xC4, 0xC6, 0xC6, 0xC8, 0xC8 };
	PointU8 sansPosition = { 80, 16 + 42 };
	const UInt8 msg1[] = "Sans: Why are graveyards so noisy?\n Because of all the *coffin*!";
	const UInt8 msg2[] = "Ellie: How are you doing today?\n That teacher was totally unfair.\n C'mon, let's go to the beach!";
	const UInt8 msg3[] = "Papyrus: Nyeh Heh Heh!";
	const UInt8 *messages[3];
	UInt8 i;

	// More messages
	// "Ellie: How are you doing today?  That teacher was totally unfair.  C'mon, let's go to the beach!";


	// Set up graphics window
	setPlayerCursorVisible(0);
	clearMapScreen();
	loadColorTable(dialogColorTable);
	setBackgroundGradient(gradient);

	// Set up text window
	clearTextWindow();
	setTextWindowColorTheme(1);

	// Add Sans
	setMegaSprite(sansMegaSprite, sansMegaSpriteLength, &sansPosition, 2);

	// Loop through messages
	messages[0] = msg1;
	messages[1] = msg2;
	messages[2] = msg3;
	for (i=0; i<3; ++i) {
		clearTextWindow();
		printStringWithLayout(messages[i], 1, 1, 5, 0);
		waitForAnyInput();
	}

	blackOutColorTable();
	hideAllSprites();
	setTextWindowColorTheme(0);
	setBackgroundGradient(NULL);
	clearMapScreen();
	drawCurrentMap(&playerMapLocation);
	//loadColorTableForCurrentMap();
	printStatText();
	setPlayerCursorVisible(1);

}

void waitForAnyInput(void) {
	UInt8 trigger;
	trigger = PEEK (STRIG0);

	// Wait for trigger to be released first.
	while (PEEK(STRIG0) == 0) {
	}

	// Then wait for trigger to be pressed
	while (PEEK(STRIG0) != 0) {
		resetAttractMode();
	}

}



