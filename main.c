// main.c


// == About This Program ==
// This program is a rogue-like game. 


// Includes
#include <atari.h>
#include "atari_memmap.h"
#include "graphics.h"
#include "images.h"
#include "map.h"
#include "puff.h"
#include "string.h"
#include "text.h"
#include "tiles.h"
#include "types.h"


// Callbacks
typedef void (*cursorEventHandlerCallbackType)(UInt8 eventType);
enum CursorEventType {
	CursorClick = 1,
	CursorUp,
	CursorDown,
	CursorLeft,
	CursorRight
};


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


// Constants and macros
//#define DEBUGGING
#define RESET_ATTRACT_MODE (POKE(ATRACT, 0))
#define SCREEN_LENGTH (40 * 72)
#define SHORT_CLOCK (PEEK(20) + 256 * PEEK(19))

// Debugging
#ifdef DEBUGGING
UInt16 startTime;
UInt16 duration;
#endif

// Text functions


void printStatText(void) {
	clearTextWindow();

	// Print character statistics
	printCharaStats(0, "Alisa", 99, 123, 255);
	printCharaStats(1, "Marie", 1, 4, 8);
	printCharaStats(2, "Guy", 19, 67, 72);
	printCharaStats(3, "Nyorn", 7, 78, 180);

	// Print party statistics
	printPartyStats(987123, 21, 1325, -891);
}


// Screen functions

enum FadeOptions {
	FadeGradient = 1, FadeTextBox = 2
};

UInt8 applyFade(UInt8 color, UInt8 amount) {
	UInt8 lum = color & 0x0F;
	if (amount > lum) {
		color = 0;
	} else {
		color -= amount;
	}
	return color;
}

void fadeOut(UInt8 fadeOptions) {
	UInt8 *colors = (UInt8*)(PCOLR0);
	UInt8 count;
	UInt8 i;

	// Disable player color cycling to avoid conflicts
	setPlayerCursorColorCycling(0);

	for (count=0; count<15; ++count) {
		*VB_TIMER = 1;

		for (i=0; i<9; ++i) {
			colors[i] = applyFade(colors[i], 1);			
		}
		if (fadeOptions & FadeGradient) {
			for (i=0; i<72; ++i) {
				BG_COLOR[i] = applyFade(BG_COLOR[i], 1);				
			}
		}
		if (fadeOptions & FadeTextBox) {
			*TEXT_LUM = applyFade(*TEXT_LUM, 1);
			*TEXT_BG = applyFade(*TEXT_BG, 1);
		}

		// Delay 
		while (*VB_TIMER) {
		}
	}
}

void fadeIn(UInt8 fadeOptions, const UInt8 *colorTable) {
	UInt8 *colors = (UInt8*)(PCOLR0);
	SInt8 amount;
	UInt8 i;

	for (amount=15; amount>=0; --amount) {
		*VB_TIMER = 1;

		for (i=0; i<9; ++i) {
			colors[i] = applyFade(colorTable[i], amount);			
		}
		if (fadeOptions & FadeGradient) {
			for (i=0; i<72; ++i) {
				//BG_COLOR[i] = applyFade(BG_COLOR[i]);				
			}
		}
		if (fadeOptions & FadeTextBox) {
			// *TEXT_LUM = applyFade(*TEXT_LUM);
			// *TEXT_BG = applyFade(*TEXT_BG);
		}

		// Delay 
		while (*VB_TIMER) {
		}
	}
}

void transitionToMap(UInt8 mapType, UInt8 shouldFade) {
	const UInt8 *colorTable = colorTableForMap(mapType);

	if (shouldFade) {
		fadeOut(0);
	}

	loadMap(mapType, sightDistance, &playerMapLocation);
	
	if (shouldFade) {	
		fadeIn(0, colorTable);
	} else {
		loadColorTable(colorTable);
	}

	// Show player cursor
	setPlayerCursorVisible(1);
	setPlayerCursorColorCycling(1);
}

enum ScreenOptions {
	ScreenOff = 0x00,
	ScreenOn = 0x01,
	EnableDLI = 0x80
};

void setScreenVisible(UInt8 options) {
	UInt8 dma = 0;
	UInt8 nmi = 0x40; // enable VBI running

	if (options & ScreenOn) {
		dma = 0x2E; // various Antic DMA options
		if (options & EnableDLI) {
			nmi |= 0x80; // enable DLI
		}
	}
	POKE (SDMCTL, dma);
	ANTIC.nmien = nmi;
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
	SInt8 result;

	result = puff(screen, &screenLen, data, &length);

	if (result) {
		printDecimal16bitValue("puff() error:  ", result, 1, 3);
		waitForAnyInput();
	}
}

void clearRasterScreen(void) {
	UInt8 *screen = (UInt8 *)PEEKW(SAVMSC);
	UInt16 i;
	// Screen is made up of 72 lines * 40 bytes = 2880 bytes
	for (i=0; i<(72*40); ++i) {
		screen[i] = 0;
	}
}

void presentDialog(void) {
	UInt8 temShopColorTable[] = {
		0x58, 0x58, 0x00, 0x00, // sprite
		0x26, 0x0E, 0x00, 0xFF, 0x2A // playfield
	};
	// UInt8 greyscaleColorTable[] = {
	// 	0x50, 0x54, 0x58, 0x5C,
	// 	0x00, 0x04, 0x08, 0xFF, 0x0C
	// };
	// UInt8 gradient[] = { 
	// 	12, 0x92, 
	// 	12, 0x94,
	// 	 6, 0x96,
	// 	 3, 0x98,
	// 	 2, 0x9A,
	// 	 1, 0x9C, 
	// 	 1, 0xC2, 
	// 	 3, 0xC4,
	// 	 7, 0xC6,
	// 	21, 0xC8,
	// 	 5, 0xCA,
	// 	 0 };
	// PointU8 sansPosition = { 81, 16 + 45 };
	// const UInt8 msg1[] = "Sans: Why are graveyards so noisy?\n Because of all the *coffin*!";
	// const UInt8 msg2[] = "Ellie: How are you doing today?\n That teacher was totally unfair.\n C'mon, let's go to the beach!";
	// const UInt8 msg3[] = "Papyrus: Nyeh Heh Heh!";
	// const UInt8 *messages[3];
	UInt8 i;

	// Set up graphics window
	fadeOut(FadeTextBox);
	setScreenVisible(0);
	clearTextWindow();
	clearRasterScreen();
	setPlayerCursorVisible(0);

	// Turn on screen
	loadColorTable(temShopColorTable);
	// setTextWindowColorTheme(1);
	// setBackgroundGradient(gradient);
	selectDisplayList(2);
	setScreenVisible(ScreenOn);

	// Set up sprites
	clearSpriteData(3);
	clearSpriteData(4);
	drawSprite(temFaceSprite, temFaceSpriteHeight, 3, 22+14);
	drawSprite(temFaceSprite+temFaceSpriteHeight, temFaceSpriteHeight, 4, 22+14);
	setSpriteHorizontalPosition(3, PM_LEFT_MARGIN + 72);
	setSpriteHorizontalPosition(4, PM_LEFT_MARGIN + 80);

	// Add Sans
	//setMegaSprite(sansMegaSprite, sansMegaSpriteLength, &sansPosition, 2);


	//printDecimal16bitValue("Time: ", duration, 0, 6);
	printString("* hOi!", 4, 1);
	printString("* welcom to...", 4, 3);
	printString("* da TEM SHOP!!!", 4, 5);

	for (i=0; i<7; ++i) {
		printString("|", 28, i);
	}

	printString("Buy", 32, 1);
	printString("Sell", 32, 2);
	printString("Talk", 32, 3);
	printString("Exit", 32, 4);
	printString("$901", 29, 6);
	printString("21{", 37, 6);

	// Draw background image
	drawImage(temShopImage, temShopImageLength);

	// Selection Cursor
	clearSpriteData(1);
	drawSprite(selectionCursorSprite, selectionCursorSpriteHeight, 1, 75+14);
	setSpriteHorizontalPosition(1, PM_LEFT_MARGIN + 119);
	setPlayerCursorColorCycling(1);

	waitForAnyInput();

	// Fade out
	fadeOut(FadeGradient | FadeTextBox);
	clearSpriteData(4);
	hideSprites();
	setScreenVisible(0);

	// Reload map
	transitionToMap(currentMapType, 0);

	selectDisplayList(1);
	setTextWindowColorTheme(0);
	setScreenVisible(ScreenOn | EnableDLI);

	printStatText();
}


// Map Movement functions

void exitToOverworld(void) {
	playerMapLocation = playerOverworldLocation;
	sightDistance = 0xFF;
	transitionToMap(OverworldMapType, 1);
}


void enterDungeon(void) {
	sightDistance = lampStrength;
	playerOverworldLocation = playerMapLocation;
	playerMapLocation = mapEntryPoint(DungeonMapType);
	transitionToMap(DungeonMapType, 1);
}


void enterTown(void) {
	sightDistance = 0xFF;
	playerOverworldLocation = playerMapLocation;
	playerMapLocation = mapEntryPoint(TownMapType);
	transitionToMap(TownMapType, 1);
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
	UInt8 vb_timer = *VB_TIMER;
	PointU8 oldLoc, newLoc;

	if (stick == previousStick && vb_timer != 0) { 
		// Handle changes in stick position immediately but delay repeating same moves.
		return;
	}
	*VB_TIMER = 10; // Reset stick timer
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
	
	while (gQuit == 0) {
		runLoop();
		RESET_ATTRACT_MODE;
	}

	return 0; // success
}
