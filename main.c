// main.c


// == About This Program ==
// This program is a rogue-like game. 


// Includes
#include "atari_memmap.h"
#include "cursor.h"
#include "dialog.h"
#include "graphics.h"
#include "images.h"
#include "image_data.h"
#include "map.h"
#include "sprites.h"
#include "text.h"
#include "types.h"
#include <atari.h>


// Globals
UInt8 isQuitting;


// Constants and macros
//#define DEBUGGING

// Debugging
#ifdef DEBUGGING
UInt16 startTime;
UInt16 duration;
#endif

// Map Mode functions

void printStatText(void) {
	clearTextWindow();

	// Print character statistics
	printCharaStats(0, "Alisa", 99, 123, 255);
	printCharaStats(1, "Marie", 1, 1, 8);
	printCharaStats(2, "Guy", 19, 35, 36);
	printCharaStats(3, "Nyorn", 7, 1, 40);

	// Print party statistics
	printPartyStats(987123, 21, 1325, -891);
}

void setUpMapMode(void) {
	// Set text window colors
	*TEXT_LUM = 0x0E;    // white
	*TEXT_BG  = 0x02;    // gray

	printStatText();

	setScreenMode(ScreenModeMap);
	transitionToMap(currentMapType, 0, 1);

	registerCursorEventHandler(mapCursorHandler);
}

// Dialog functions


void handleMessage(SInt8 message) {
	switch (message) {
		case MessagePresentDialog:
			initDialog();
			break;
		case MessageExitDialog:
			exitDialog();
			setUpMapMode();
			break;
	}
}

void runLoop(void) {
	handleMessage(handleTrigger());
	handleMessage(handleStick());
}

int main (void) {
	// Init
	initGraphics();
	initCursor();
	
	// Start new game
	isQuitting = 0;

	currentMapType = OverworldMapType;
	mapShipType = 0;
	mapLampStrength = 2;
	mapSightDistance = 0xFF;
	mapOverworldLocation = mapEntryPoint(OverworldMapType);
	mapCurrentLocation = mapOverworldLocation;

	setUpMapMode();
	
	while (isQuitting == 0) {
		runLoop();
		resetAttractMode();
	}

	return 0; // success
}
