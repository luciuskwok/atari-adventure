// main.c


// == About This Program ==
// This program is a rogue-like game. 


// Includes
#include "atari_memmap.h"
#include "battle.h"
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

void setUpMapMode(void) {
	printAllCharaText(0);
	printPartyStats(987123, 21, 1325);

	setScreenMode(ScreenModeMap);
	transitionToMap(currentMapType, 0, 1);
	setTextBoxColors();

	registerCursorEventHandler(mapCursorHandler);
}

// Dialog functions

void fadeOutScreen(void) {
	// Fade out
	fadeOutColorTable(FadeGradient | FadeTextBox);
	clearSpriteData(4);
	hideSprites();
}

void handleMessage(SInt8 message) {
	switch (message) {
		case MessageEnterDialog:
			initDialog();
			break;
		case MessageEnterBattle:
			initBattle();
			break;
		case MessageReturnToMap:
			fadeOutScreen();
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
