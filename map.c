// map.c

#include "map.h"
#include "cursor.h"
#include "graphics.h"
#include "map_data.h"
#include "menu.h"
#include "sound.h"
#include "sprites.h"
#include "text.h"
#include "atari_memmap.h"
#include <string.h>


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
	// UInt16 startTime = SHORT_CLOCK;

	printAllCharaStats(0);
	printPartyStats();

	// debugPrint("Text:", SHORT_CLOCK - startTime, 0, 5);
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

static void drawCurrentMapOLD(void) {
	UInt16 startTime = SHORT_CLOCK; // DEBUGGING

	const UInt8 *runLenPtr = currentRunLenMap;
	UInt8 mapFrameHalfWidth = mapFrame.size.width / 2;
	UInt8 mapFrameHalfHeight = mapFrame.size.height / 2;
	UInt8 screenRow;
	UInt8 mapRow, mapCol;
	UInt8 topMargin, leftMargin, rightMargin;
	UInt8 decodeLength;
	UInt8 c;
	UInt8 buffer[SCREEN_WIDTH]; // ptr2

	// Integrity check
	if (runLenPtr == NULL) {
		return;
	}

	// Calculate number of rows to leave blank and how many to skip in the map.
	if (playerLocation.y <= mapFrameHalfHeight) {
		// In this case, the top edge of the window is above the top edge of the map
		// data, so there will be a top margin filled with the "out of bounds" tile.
		topMargin = mapFrameHalfHeight - playerLocation.y;
		mapRow = 0;
	} else {
		topMargin = 0;
		mapRow = playerLocation.y - mapFrameHalfHeight;

		// Jump to first visible row in map data.
		for (c=0; c<mapRow; ++c) {
			runLenPtr += runLenPtr[0];
		}
	}

	// Calculate number of columns to leave blank and how many to skip in the map.
	if (playerLocation.x < mapFrameHalfWidth) {
		// In this case, the left edge of the window shows tiles beyond the left edge
		// of the map data. So the left margin is increased to indicate that how many
		// "out of bounds" tiles to show.  
		leftMargin = mapFrameHalfWidth - playerLocation.x;
		mapCol = 0;
	} else {
		// In this case, the left edge of the window starts at an offset from the left
		// edge of the map data. So the mapCol is set to this offset. 
		leftMargin = 0;
		mapCol = playerLocation.x - mapFrameHalfWidth;
	}

	// Calculate the number of map tiles to decode on each line. This is the width 
	// of the window minus any left margin, which is the "out of bounds" area.
	decodeLength = mapFrame.size.width - leftMargin;
	// For narrow maps, make sure the end doesn't extend beyond the right edge
	// of the map data. 
	if (decodeLength > mapSize.width - mapCol) {
		decodeLength = mapSize.width - mapCol;
	}
	// The right margin is where the map tile drawing ends, when screenCol is 
	// equal or greater than the right margin, draw the "out of bounds" tile.
	rightMargin = mapSize.width + leftMargin - mapCol;

	// Set zeropage parameters
	POKEW(SAVADR, PEEKW(SAVMSC) + mapFrame.origin.y * SCREEN_WIDTH + mapFrame.origin.x);
	// NEWROW: height of the map window frame
	POKE(LMARGN, leftMargin); 
	POKE(RMARGN, rightMargin); 
	POKE(OLDROW, mapRow); 
	POKE(OLDCOL, mapCol); 
	// ROWCRS: screenRow
	// COLCRS: screenCol
	POKE(DELTAC, decodeLength);

	// Main Loop
	for (screenRow=0; screenRow<mapFrame.size.height; ++screenRow) {
		// Clear the sprite overlay for this row
		dliSpriteData[screenRow] = 0;


		if (screenRow < topMargin || mapRow >= mapSize.height) {
			// Beyond borders: fill with the default empty tile.
			fillMapRow(currentTileMap[0]);
		} else {
			decodeRunLenRange(buffer, runLenPtr);
			runLenPtr += runLenPtr[0]; // Next row.

			POKE(ROWCRS, screenRow); // Pass screenRow to drawMapRow
			drawMapRow(buffer);

			++mapRow;
		} // end if

		POKEW(SAVADR, PEEKW(SAVADR) + SCREEN_WIDTH);
	}

	// Debugging
	debugPrint("Map:", SHORT_CLOCK - startTime, 0, 5);
}

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
	setCursorColorCycling(1);
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
				result = MessageEnterDialog;
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
				noteOn(NoteF+Oct3, 1, 4, 15, 0x00, 3);
				playerLocation = newLoc;
				drawCurrentMap();
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
	POKE(SIZEP3, 0);

	transitionToMap(currentMapType, 0, 1);
	drawMapTextBox();
	setCursorEventHandler(mapCursorHandler);
}
