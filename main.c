// main.c


// == About This Program ==
// This program is a rogue-like game. 


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
void printDebuggingInfo(void);

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
	//printStatText();
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

void printDebuggingInfo(void) {
	UInt16 dl = PEEKW(SDLSTL);
	UInt16 screen = PEEKW(SAVMSC);

	clearTextWindow();
	print16bitValue("DL: ", dl, 1, 1);
	print16bitValue("SCR:", screen, 1, 2);

}


// ==== Dialog ====


void presentDialog(void) {
	UInt8 dialogColorTable[] = {
		0x0F, 0x0F, 0x00, 0x00, // white/black for mega sprite
		0x00, 0x06, 0x0A, 0x0E, 0x94 // grayscale for playfield
	};
	UInt8 gradient[] = { 
		12, 0x92, 
		12, 0x94,
		 6, 0x96,
		 3, 0x98,
		 2, 0x9A,
		 1, 0x9C, 
		 1, 0xC2, 
		 3, 0xC4,
		 7, 0xC6,
		21, 0xC8,
		 5, 0xCA,
		 0 };
	PointU8 sansPosition = { 81, 16 + 45 };
	const UInt8 msg1[] = "Sans: Why are graveyards so noisy?\n Because of all the *coffin*!";
	const UInt8 msg2[] = "Ellie: How are you doing today?\n That teacher was totally unfair.\n C'mon, let's go to the beach!";
	const UInt8 msg3[] = "Papyrus: Nyeh Heh Heh!";
	const UInt8 *messages[3];
	UInt8 i;

	// Set up graphics window
	hidePlayfieldAndSprites();
	setPlayerCursorVisible(0);
	loadColorTable(dialogColorTable);
	setBackgroundGradient(gradient);
	selectDisplayList(1);

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

	// Reload map
	hidePlayfieldAndSprites();
	setTextWindowColorTheme(0);
	selectDisplayList(0);
	loadMap(currentMapType, sightDistance, &playerMapLocation);

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



