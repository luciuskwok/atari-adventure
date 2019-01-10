// map.c

#include "map.h"
#include "cursor.h"
#include "graphics.h"
#include "map_data.h"
#include "menu.h"
#include "misc_asm.h"
#include "sound.h"
#include "sprites.h"
#include "text.h"
#include "atari_memmap.h"
#include <string.h>


// Globals
UInt8 currentMapType;
UInt8 mapShipType;
UInt8 mapLampStrength;
UInt8 mapSightDistance;
PointU8 mapOverworldLocation;
PointU8 mapCurrentLocation;


// Private Globals
const UInt8 *currentRunLenMap;
const UInt8 *currentTileMap;
SizeU8 currentMapSize;
RectU8 mapFrame;

#define SCREEN_WIDTH (24)

// Menu

static void drawMapTextBox(void) {
	UInt16 startTime = SHORT_CLOCK;
	UInt16 duration;
	UInt8 s[10] = "Text:";

	printAllCharaText(0);
	printPartyStats();

	duration = SHORT_CLOCK - startTime;
	uint16toString(s+5, duration);
	printStringAtXY(s, 0, 5);
}

static void exitMapMenu(void) {
	POKE(TXTBKG, overworldColorTable[10]);
	setPlayerCursorVisible(1);
	zeroOut16(textWindow, 7*SCREEN_ROW_BYTES);
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
	RectU8 r = { { 0, 5 }, { 40, 1 } };

	setPlayerCursorVisible(0);
	clearTextRect(&r);
	printStringAtXY("* Heal  * Info  * Save  * Done", 5, 5);
	POKE(TXTBKG, 0x34);

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
	decodeRunLenRange(&tile, x, 1, runLenPtr);

	// Convert to character value
	return currentTileMap[tile];
}

static UInt8 canMoveTo(UInt8 x, UInt8 y) {
	UInt8 tile = mapTileAt(x, y) & 0x3F; // remove color data

	switch (currentMapType) {
		case OverworldMapType:
			if (tile == tShallows) { // Shallows
				return mapShipType >= 1;
			} else if (tile == tWater) {
				return mapShipType >= 2;
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

void loadMap(UInt8 mapType, UInt8 mapSightDistance, UInt8 x, UInt8 y) {
	const UInt8 *colorTable;

	clearMapScreen();
	
	switch (mapType) {
	case OverworldMapType: 
		currentRunLenMap = overworldRleMap;
		currentMapSize = overworldMapSize;
		currentTileMap = overworldTileMap;
		colorTable = overworldColorTable;
		break;
	case DungeonMapType: 
		currentRunLenMap = dungeonRleMap;
		currentMapSize = dungeonMapSize;
		currentTileMap = dungeonTileMap;
		colorTable = dungeonColorTable;
		break;
	case TownMapType: 
		currentRunLenMap = townRleMap;
		currentMapSize = townMapSize;
		currentTileMap = townTileMap;
		colorTable = townColorTable;
		break;
	}
	currentMapType = mapType;

	layoutCurrentMap(mapSightDistance);
	drawCurrentMap(x, y);
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
	zeroOut8(graphicsWindow, 9*SCREEN_WIDTH);
	zeroOut8(P3_XPOS, 13);
}

void layoutCurrentMap(UInt8 mapSightDistance) {
	UInt8 x, halfWidth, halfHeight;

	if (mapSightDistance > 3) {
		mapFrame.size.width = 21;
		mapFrame.size.height = 9;
	} else {
		x = mapSightDistance * 2 + 1;
		mapFrame.size.width = x;
		mapFrame.size.height = x;
	}
	halfWidth = mapFrame.size.width / 2;
	halfHeight = mapFrame.size.height / 2;
	// Map screen size is 24 wide by 9 high.
	mapFrame.origin.x = 11 - halfWidth;
	mapFrame.origin.y = 4 - halfHeight;

	// Clear out the sprite overlays
	zeroOut8(P3_XPOS, 13);
}

void drawCurrentMap(UInt8 x, UInt8 y) {
	UInt8 *screen = graphicsWindow;
	const UInt8 *runLenPtr = currentRunLenMap;
	UInt8 screenRowSkip = SCREEN_WIDTH - mapFrame.size.width;
	UInt8 screenIndex = mapFrame.origin.x + SCREEN_WIDTH * mapFrame.origin.y;
	UInt8 mapFrameHalfWidth = mapFrame.size.width / 2;
	UInt8 mapFrameHalfHeight = mapFrame.size.height / 2;
	UInt8 row, col;
	UInt8 colMax;
	UInt8 leftBlank, leftSkip, decodeLength, topBlank, topSkip;
	UInt8 c, low, hasSpriteOverlay;
	UInt8 buffer[SCREEN_WIDTH];

	// Debugging
	// UInt16 startTime = SHORT_CLOCK;
	// UInt16 duration;

	// Integrity check
	if (runLenPtr == NULL) {
		return;
	}

	// Calculate number of rows to leave blank and how many to skip in the map.
	if (y <= mapFrameHalfHeight) {
		topBlank = mapFrameHalfHeight - y;
		topSkip = 0;
	} else {
		topBlank = 0;
		topSkip = y - mapFrameHalfHeight;

		// Adjust runLenPtr to skip lines within run-len data.
		for (c=0; c<topSkip; ++c) {
			runLenPtr += runLenPtr[0];
		}
	}

	// Calculate number of columns to leave blank and how many to skip in the map.
	if (x < mapFrameHalfWidth) {
		leftBlank = mapFrameHalfWidth - x;
		leftSkip = 0;
	} else {
		leftBlank = 0;
		leftSkip = x - mapFrameHalfWidth;
	}
	decodeLength = mapFrame.size.width - leftBlank;
	if (decodeLength > currentMapSize.width - leftSkip) {
		decodeLength = currentMapSize.width - leftSkip;
	}
	colMax = currentMapSize.width + leftBlank - leftSkip;

	// Main Loop
	for (row=0; row<mapFrame.size.height; ++row) {
		hasSpriteOverlay = 0;

		if (row < topBlank || row + topSkip >= currentMapSize.height + topBlank) {
			// Beyond borders: fill with the default empty tile.
			for (col=0; col<mapFrame.size.width; ++col) {
				screen[screenIndex] = currentTileMap[0];
				++screenIndex;
			}
		} else {
			decodeRunLenRange(buffer, leftSkip, decodeLength, runLenPtr);
			runLenPtr += runLenPtr[0]; // Next row.
			for (col=0; col<mapFrame.size.width; ++col) {
				if (col < leftBlank || col >= colMax) {
					c = 0; // Tiles outside map bounds are set to default blank tile.
				} else {
					c = buffer[col - leftBlank];
				}

				// Convert decoded value to character value
				c = currentTileMap[c];

				// Add sprite overlay for special characters
				low = (c & 0x3F);
				if (low >= tCastle) {
					low -= tCastle;
					setTileSprite(low, col, row);
					hasSpriteOverlay = 1;
				}

				screen[screenIndex] = c;
				++screenIndex;
			} // end for(col)
		} // end if
			
		if (hasSpriteOverlay == 0) {
			// Clear the sprite overlay for this row
			P3_XPOS[row] = 0;
		}
		screenIndex += screenRowSkip;
	}

	// Debugging
	// {
	// 	UInt8 s[6];
	// 	duration = SHORT_CLOCK - startTime;
	// 	uint16toString(s, duration);
	// 	printStringAtXY(s, 0, 4);
	// }
}

// Map Movement

void transitionToMap(UInt8 mapType, UInt8 shouldFadeOut, UInt8 shouldFadeIn) {
	const UInt8 *colorTable = colorTableForMap(mapType);

	if (shouldFadeOut) {
		fadeOutColorTable();
	} else {
		loadColorTable(NULL);
	}

	loadMap(mapType, mapSightDistance, mapCurrentLocation.x, mapCurrentLocation.y);
	
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
	mapCurrentLocation = mapOverworldLocation;
	mapSightDistance = 0xFF;
	transitionToMap(OverworldMapType, 1, 1);
}

void enterDungeon(void) {
	mapSightDistance = mapLampStrength;
	mapOverworldLocation = mapCurrentLocation;
	mapCurrentLocation = dungeonEntryPoint;
	transitionToMap(DungeonMapType, 1, 1);
}

void enterTown(void) {
	startSong(4);
	mapSightDistance = 0xFF;
	mapOverworldLocation = mapCurrentLocation;
	mapCurrentLocation = townEntryPoint;
	transitionToMap(TownMapType, 1, 1);
}

SInt8 mapCursorHandler(UInt8 event) {
	SInt8 result = MessageNone;
	PointU8 oldLoc, newLoc;
	UInt8 tile;

	oldLoc = mapCurrentLocation;
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
		if (newLoc.x < currentMapSize.width && newLoc.y < currentMapSize.height) {
			if (canMoveTo(newLoc.x, newLoc.y)) {
				noteOn(NoteF+Oct3, 1, 4, 15, 0x00, 3);
				mapCurrentLocation = newLoc;
				drawCurrentMap(newLoc.x, newLoc.y);
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
	zeroOut16(textWindow, 7*SCREEN_ROW_BYTES);

	// Set up missiles for use by sound sequencer for debuggging
	clearSprite(0);
	fillSprite(0, 0xFF, 0, 10);
	// setSpriteOriginX(5, 56);
	// setSpriteOriginX(6, 72);

	transitionToMap(currentMapType, 0, 1);
	drawMapTextBox();
	setCursorEventHandler(mapCursorHandler);
}
