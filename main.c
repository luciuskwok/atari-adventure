// main.c


// == About This Program ==
// This program is a rogue-like game. 


// Includes
#include "atari_memmap.h"
#include "cursor.h"
#include "graphics.h"
#include "images.h"
#include "image_data.h"
#include "map.h"
#include "sprites.h"
#include "text.h"
#include "types.h"
#include <atari.h>
#include <stdio.h>
#include <string.h>




// Globals
UInt8 isQuitting;



// Constants and macros
//#define DEBUGGING
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

// Dialog functions

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
	fadeOutColorTable(FadeTextBox);
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
	fadeOutColorTable(FadeGradient | FadeTextBox);
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



void runLoop(void) {
	handleTrigger();
	handleStick();
}

int main (void) {
	// Init
	initGraphics();
	initCursor();
	
	// Start new game
	isQuitting = 0;
	mapShipType = 0;
	mapLampStrength = 2;
	mapSightDistance = 0xFF;
	mapOverworldLocation = mapEntryPoint(OverworldMapType);

	// Load map
	exitToOverworld();
	setTextWindowColorTheme(0);
	printStatText();
	registerCursorEventHandler(mapCursorHandler);
	
	while (isQuitting == 0) {
		runLoop();
		resetAttractMode();
	}

	return 0; // success
}
