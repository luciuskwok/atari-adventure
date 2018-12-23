// main.c


// == About This Program ==
// This program is a rogue-like game. 


// Includes
#include <atari.h>
#include "atari_memmap.h"
#include "graphics.h"
#include "map.h"
#include "puff.h"
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

const UInt8 testImage[] = {
0xED, 0xD6, 0xD3, 0xA2, 0x10, 0x06, 0x00, 0x00, 0xD0, 0x9B, 0xED, 0x65, 
0xBB, 0x2D, 0xDB, 0xE6, 0x32, 0x96, 0x6D, 0xDB, 0xB6, 0x5D, 0xCB, 0x36, 
0xB6, 0x6C, 0x6C, 0xD9, 0xB6, 0x6D, 0xDB, 0x7C, 0xEA, 0xFC, 0x40, 0xAF, 
0x9D, 0xAF, 0x38, 0x01, 0x01, 0x3F, 0x04, 0x22, 0x30, 0x41, 0x08, 0x4A, 
0x30, 0x82, 0x13, 0x82, 0x90, 0x84, 0x22, 0x34, 0x61, 0x08, 0x4B, 0x38, 
0xC2, 0x13, 0x81, 0x88, 0x44, 0x22, 0x32, 0x51, 0xF8, 0x8D, 0xA8, 0x44, 
0x23, 0x3A, 0x31, 0x88, 0x49, 0x2C, 0x62, 0x13, 0x87, 0xB8, 0xC4, 0x23, 
0x3E, 0x09, 0x48, 0x48, 0x22, 0x12, 0x93, 0x84, 0xA4, 0x24, 0x23, 0x39, 
0x29, 0xF8, 0x9D, 0x3F, 0x48, 0x49, 0x2A, 0x52, 0x93, 0x86, 0xB4, 0xA4, 
0x23, 0x3D, 0x19, 0xC8, 0x48, 0x26, 0x32, 0x93, 0x85, 0xAC, 0x64, 0x23, 
0x3B, 0x39, 0xC8, 0x49, 0x2E, 0x72, 0x93, 0x87, 0xBC, 0xE4, 0x23, 0x3F, 
0x05, 0x28, 0x48, 0x21, 0x0A, 0x53, 0x84, 0xA2, 0x14, 0xE3, 0x4F, 0x8A, 
0x53, 0x82, 0x92, 0x94, 0xA2, 0x34, 0x65, 0x28, 0x4B, 0x39, 0xCA, 0x53, 
0x81, 0xBF, 0xA8, 0x48, 0x25, 0x2A, 0x53, 0x85, 0xAA, 0x54, 0xA3, 0x3A, 
0x35, 0xA8, 0x49, 0x2D, 0x6A, 0x53, 0x87, 0xBA, 0xD4, 0xA3, 0x3E, 0x0D, 
0x68, 0x48, 0x23, 0x1A, 0xD3, 0x84, 0xA6, 0x34, 0xA3, 0x39, 0x2D, 0x68, 
0x49, 0x2B, 0x5A, 0xD3, 0x86, 0xB6, 0xB4, 0xA3, 0x3D, 0x1D, 0xE8, 0x48, 
0x27, 0x3A, 0xD3, 0x85, 0xAE, 0x74, 0xA3, 0x3B, 0x3D, 0xE8, 0x49, 0x2F, 
0x7A, 0xD3, 0x87, 0xBE, 0xF4, 0xA3, 0x3F, 0x03, 0x18, 0xC8, 0x20, 0x06, 
0x33, 0x84, 0xA1, 0x0C, 0x63, 0x38, 0x23, 0x18, 0xC9, 0x28, 0x46, 0x33, 
0x86, 0xB1, 0x8C, 0x63, 0x3C, 0x7F, 0x33, 0x81, 0x89, 0x4C, 0x62, 0x32, 
0x53, 0x98, 0xCA, 0x34, 0xA6, 0x33, 0x83, 0x99, 0xCC, 0x62, 0x36, 0x73, 
0x98, 0xCB, 0x3C, 0xE6, 0xB3, 0x80, 0x85, 0x2C, 0x62, 0x31, 0x4B, 0x58, 
0xCA, 0x3F, 0xFC, 0xCB, 0x32, 0x96, 0xB3, 0x82, 0x95, 0xAC, 0x62, 0x35, 
0x6B, 0x58, 0xCB, 0x3A, 0xD6, 0xB3, 0x81, 0x8D, 0x6C, 0x62, 0x33, 0x5B, 
0xD8, 0xCA, 0x7F, 0xFC, 0xCF, 0x36, 0xB6, 0xB3, 0x83, 0x9D, 0xEC, 0x62, 
0x37, 0x7B, 0xD8, 0xCB, 0x3E, 0xF6, 0x73, 0x80, 0x83, 0x1C, 0xE2, 0x30, 
0x47, 0x38, 0xCA, 0x31, 0x8E, 0x73, 0x82, 0x93, 0x9C, 0xE2, 0x34, 0x67, 
0x38, 0xCB, 0x39, 0xCE, 0x73, 0x81, 0x8B, 0x5C, 0xE2, 0x32, 0x57, 0xB8, 
0xCA, 0x35, 0xAE, 0x73, 0x83, 0x9B, 0xDC, 0xE2, 0x36, 0x77, 0xB8, 0xCB, 
0x3D, 0xEE, 0xF3, 0x80, 0x87, 0x3C, 0xE2, 0x31, 0x4F, 0x78, 0xCA, 0x33, 
0x9E, 0xF3, 0x82, 0x97, 0xBC, 0xE2, 0x35, 0x6F, 0x78, 0xCB, 0x3B, 0xDE, 
0xF3, 0x81, 0x8F, 0x7C, 0xE2, 0x33, 0x5F, 0xF8, 0xCA, 0x37, 0x02, 0x7E, 
0xFD, 0xEF, 0xA7, 0xFE, 0xF7, 0x1D, 
};
const UInt16 testImageLength = 402;


// Constants and macros
#define STICK_TIMER (0x0601)
#define RESET_ATTRACT_MODE (POKE(ATRACT, 0))
#define SCREEN_LENGTH (40 * 72)
#define SHORT_CLOCK (PEEK(20) + 256 * PEEK(19))






// Text functions

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


// Dialog functions


void waitForAnyInput(void) {
	// Wait for trigger to be released first.
	while (PEEK(STRIG0) == 0) {
	}

	// Then wait for trigger to be pressed
	while (PEEK(STRIG0) != 0) {
		RESET_ATTRACT_MODE;
	}
}


void drawImage(const UInt8 *data, UInt16 length) {
	UInt8 *screen = (UInt8 *)PEEKW(SAVMSC);
	UInt16 screenLen = SCREEN_LENGTH;
	UInt16 startTime = SHORT_CLOCK;
	UInt16 duration;
	SInt8 result;

	result = puff(screen, &screenLen, data, &length);

	duration = SHORT_CLOCK - startTime;
	print16bitValue("Timing: ", duration, 1, 1);
	print8bitValue("Result: ", result, 1, 2);
	printString("Press fire to continue.", 1, 3);
	waitForAnyInput();

}


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

	clearTextWindow();
	setTextWindowColorTheme(1);

	drawImage(testImage, testImageLength);

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


// Movement functions


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


void runLoop(void) {
	handleStick();
	handleTrigger();
}


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
		runLoop();
		RESET_ATTRACT_MODE;
	}

	return 0; // success
}
