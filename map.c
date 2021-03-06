// map.c

#include "map.h"
#include <atari.h>
#include <string.h>
#include "cursor.h"
#include "graphics.h"
#include "map_data.h"
#include "menu.h"
#include "sound.h"
#include "sprites.h"
#include "text.h"
#include "atari_memmap.h"


// Globals
UInt8 currentMapType;
UInt8 playerShipType;
UInt8 playerLampStrength;
UInt8 playerSightDistance;
PointU8 playerOverworldLocation;
PointU8 playerLocation;


// Private Globals
const UInt8 *currentRunLenMap;
const UInt8 *currentTileMap;
SizeU8 mapSize;
RectU8 mapFrame;


// Assembly routines
void drawCurrentMap(void);
void fillMapRow(UInt8 c);
void drawMapRow(UInt8 *buffer);
void decodeRunLenRange(UInt8 *outData, const UInt8 *runLenData);


#define SCREEN_WIDTH (24)

// Menu

static void drawMapTextBox(void) {
	// UInt16 startTime = Clock16;

	printAllCharaStats(0);
	printPartyStats();

	// debugPrint("Text:", Clock16 - startTime, 0, 5);
}

static void exitMapMenu(void) {
	POKE(TXTBKG, overworldColorTable[10]);
	setPlayerCursorVisible(1);
	zeroOut16(TEXT_WINDOW, 7*SCREEN_ROW_BYTES);
	drawMapTextBox();
	setCursorEventHandler(mapCursorHandler);
}

static SInt8 handleMenuClick(UInt8 index) {
	switch (index) {
		case 0: // Heal

			break;

		case 1: // Info
			return MessageEnterInfo;

		case 2: // Save

			break;

		case 3: // Done
			exitMapMenu();
			break;

	}
	return MessageNone;
}


static void initMapMenu(void) {
	const UInt8 bottomRow = 5;

	zeroOut8(TEXT_WINDOW + (bottomRow * SCREEN_ROW_BYTES), 40);
	printStringAtXY("* Heal  * Info  * Save  * Done", 5, 5);
	POKE(TXTBKG, 0x34);
	
	setPlayerCursorVisible(0);

	initMenu();
	menuOrigin.x = 6 + 5 * 4;
	menuOrigin.y = 2 + 23 * 4;
	menuItemCount = 4;
	menuItemSpacing = 8 * 4;
	menuIsHorizontal = 1;
	setMenuClickHandler(handleMenuClick);
	setMenuCursor(MediumHeartCursor);
	setMenuSelectedIndex(0);
}

static UInt8 mapTileAt(UInt8 x, UInt8 y) {
	const UInt8 *runLenPtr = currentRunLenMap;
	UInt8 tile, i;

	// Skip to row
	for (i=0; i<y; ++i) {
		runLenPtr += runLenPtr[0];
	}

	// Get tile
	POKE(OLDCOL, x); // skip count
	POKE(DELTAC, 1); // decode length
	decodeRunLenRange(&tile, runLenPtr);

	// Convert to character value
	return currentTileMap[tile];
}

static UInt8 canMoveTo(UInt8 x, UInt8 y) {
	UInt8 tile = mapTileAt(x, y) & 0x3F; // remove color data

	switch (currentMapType) {
		case OverworldMapType:
			if (tile == tShallows) { // Shallows
				return playerShipType >= 1;
			} else if (tile == tWater) {
				return playerShipType >= 2;
			}
			return 1;
		case DungeonMapType:
			return tile != tBrick;
		case TownMapType:
			return tile == tSolid || tile == tHouseDoor;
	}

	return 1;
}

// Map Drawing

static void loadMap(UInt8 mapType, UInt8 playerSightDistance) {
	const UInt8 *colorTable;

	clearMapScreen();
	
	switch (mapType) {
	case OverworldMapType: 
		currentRunLenMap = overworldRleMap;
		mapSize = overworldMapSize;
		currentTileMap = overworldTileMap;
		colorTable = overworldColorTable;
		break;
	case DungeonMapType: 
		currentRunLenMap = dungeonRleMap;
		mapSize = dungeonMapSize;
		currentTileMap = dungeonTileMap;
		colorTable = dungeonColorTable;
		break;
	case TownMapType: 
		currentRunLenMap = townRleMap;
		mapSize = townMapSize;
		currentTileMap = townTileMap;
		colorTable = townColorTable;
		break;
	}
	currentMapType = mapType;

	layoutCurrentMap(playerSightDistance);
	drawCurrentMap();
}

const UInt8 *colorTableForMap(UInt8 mapType) {
	switch (mapType) {
	case OverworldMapType: 
		return overworldColorTable;
	case DungeonMapType: 
		return dungeonColorTable;
	case TownMapType: 
		return townColorTable;
	}
	return overworldColorTable;
}

void clearMapScreen(void) {
	zeroOut8(GRAPHICS_WINDOW, 9*SCREEN_WIDTH);
	zeroOut8(dliSpriteData, dliSpriteDataLength);
}

void layoutCurrentMap(UInt8 playerSightDistance) {
	UInt8 x, halfWidth, halfHeight;

	if (playerSightDistance > 3) {
		mapFrame.size.width = 21;
		mapFrame.size.height = 9;
	} else {
		x = playerSightDistance * 2 + 1;
		mapFrame.size.width = x;
		mapFrame.size.height = x;
	}
	halfWidth = mapFrame.size.width / 2;
	halfHeight = mapFrame.size.height / 2;
	// Map screen size is 24 wide by 9 high.
	mapFrame.origin.x = 11 - halfWidth;
	mapFrame.origin.y = 4 - halfHeight;

	// Clear out the sprite overlays
	zeroOut8(dliSpriteData, dliSpriteDataLength);
}



// Map Movement

void transitionToMap(UInt8 mapType, UInt8 shouldFadeOut, UInt8 shouldFadeIn) {
	const UInt8 *colorTable = colorTableForMap(mapType);

	if (shouldFadeOut) {
		fadeOutColorTable();
	} else {
		loadColorTable(NULL);
	}

	loadMap(mapType, playerSightDistance);
	
	if (shouldFadeIn) {	
		fadeInColorTable(colorTable);
	} else {
		loadColorTable(colorTable);
	}

	// Show player cursor
	setPlayerCursorVisible(1);
	colorCyclingEnable = 1;
}

void exitToOverworld(void) {
	stopSong();
	playerLocation = playerOverworldLocation;
	playerSightDistance = 0xFF;
	transitionToMap(OverworldMapType, 1, 1);
}

void enterDungeon(void) {
	playerSightDistance = playerLampStrength;
	playerOverworldLocation = playerLocation;
	playerLocation = dungeonEntryPoint;
	transitionToMap(DungeonMapType, 1, 1);
}

void enterTown(void) {
	startSong(4);
	playerSightDistance = 0xFF;
	playerOverworldLocation = playerLocation;
	playerLocation = townEntryPoint;
	transitionToMap(TownMapType, 1, 1);
}

SInt8 mapCursorHandler(UInt8 event) {
	SInt8 result = MessageNone;
	PointU8 oldLoc, newLoc;
	UInt8 tile;

	oldLoc = playerLocation;
	newLoc = oldLoc;

	switch (event) {
	case CursorClick:
		// Get the tile without color info.
		tile = mapTileAt(oldLoc.x, oldLoc.y) & 0x3F; 
		switch (tile) {
			case tTown:
				enterTown();
				break;
			case tVillage:
			case tCastle:
				result = MessageEnterDialog;
				break;
			case tMonument:
				enterDungeon();
				break;
			case tCave:
				result = MessageEnterBattle;
				break;
			case tHouseDoor:
				result = MessageEnterShop;
				break;
			case tLadder:
				exitToOverworld();
				break;
			default:
				initMapMenu();
				break;
		}
		break;

	case CursorUp:    --newLoc.y; break; // up
	case CursorDown:  ++newLoc.y; break; // down
	case CursorLeft:  --newLoc.x; break; // left
	case CursorRight: ++newLoc.x; break; // right
	}

	if (newLoc != oldLoc) {
		// Check map bounds. Because newLoc is unsigned, it wraps around from 0 to 255.
		if (newLoc.x < mapSize.width && newLoc.y < mapSize.height) {
			if (canMoveTo(newLoc.x, newLoc.y)) {
				UInt16 startTime = Clock16;

				noteOn(NoteF+Oct3, 1, 4, 15, 0x00, 3);
				playerLocation = newLoc;
				drawCurrentMap();

				debugPrint("Map:", Clock16-startTime, 0, 5);
			}
		} else {
			// Handle moving off the map for towns
			if (currentMapType == TownMapType) {
				noteOn(NoteC+Oct4, 1, 4, 15, 0x00, 3);
				exitToOverworld();
			}
		}
	}

	return result;
}

void initMap(void) {
	hideSprites();
	setScreenMode(ScreenModeMap);
	clearMapScreen();
	zeroOut16(TEXT_WINDOW, 7*SCREEN_ROW_BYTES);

	// Set up missiles for use by sound sequencer for debuggging
	clearSprite(0);
	fillSprite(0, 0xFF, 0, 10);
	// setSpriteOriginX(5, 56);
	// setSpriteOriginX(6, 72);

	// Set player 3 sprite size
	GTIA_WRITE.sizep3 = 0;

	transitionToMap(currentMapType, 0, 1);
	drawMapTextBox();
	setCursorEventHandler(mapCursorHandler);
}
