// map.c

#include "map.h"
#include "cursor.h"
#include "graphics.h"
#include "map_data.h"
#include "sprites.h"
#include "images.h"
#include "atari_memmap.h"
#include <string.h>


// Globals
PointU8 mapOverworldLocation;
PointU8 mapCurrentLocation;
UInt8 mapShipType;
UInt8 mapLampStrength;
UInt8 mapSightDistance;


// Private Globals
const UInt8 *currentRunLenMap;
const UInt8 *currentTileMap;
SizeU8 currentMapSize;
UInt8 currentMapType;
PointU8 mapFrameOrigin;
SizeU8 mapFrameSize;

#define SCREEN_WIDTH (24)

// Assembly routine
extern void __fastcall__ decodeRunLenRange(UInt8 *outData, UInt8 start, UInt8 end, const UInt8 *runLenData);


// Map Movement

void transitionToMap(UInt8 mapType, UInt8 shouldFade) {
	const UInt8 *colorTable = colorTableForMap(mapType);

	if (shouldFade) {
		fadeOutColorTable(0);
	}

	loadMap(mapType, mapSightDistance, &mapCurrentLocation);
	
	if (shouldFade) {	
		fadeInColorTable(0, colorTable);
	} else {
		loadColorTable(colorTable);
	}

	// Show player cursor
	setPlayerCursorVisible(1);
	setPlayerCursorColorCycling(1);
}

void exitToOverworld(void) {
	mapCurrentLocation = mapOverworldLocation;
	mapSightDistance = 0xFF;
	transitionToMap(OverworldMapType, 1);
}

void enterDungeon(void) {
	mapSightDistance = mapLampStrength;
	mapOverworldLocation = mapCurrentLocation;
	mapCurrentLocation = mapEntryPoint(DungeonMapType);
	transitionToMap(DungeonMapType, 1);
}

void enterTown(void) {
	mapSightDistance = 0xFF;
	mapOverworldLocation = mapCurrentLocation;
	mapCurrentLocation = mapEntryPoint(TownMapType);
	transitionToMap(TownMapType, 1);
}

UInt8 canMoveTo(PointU8 *pt) {
	UInt8 tile = mapTileAt(pt) & 0x3F; // remove color data

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

void mapCursorHandler(UInt8 event) {
	UInt8 tile;
	PointU8 oldLoc, newLoc;

	oldLoc = mapCurrentLocation;
	newLoc = oldLoc;
	
	switch (event) {
	case CursorClick:
		// Get the tile without color info.
		tile = mapTileAt(&mapCurrentLocation) & 0x3F; 
		switch (tile) {
			case tTown:
				enterTown();
				break;
			case tVillage:
			case tCastle:
				//presentDialog();
				break;
			case tMonument:
			case tCave:
				enterDungeon();
				break;
			case tHouseDoor:
				//presentDialog();
				break;
			case tLadder:
				exitToOverworld();
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
			if (canMoveTo(&newLoc)) {
				mapCurrentLocation = newLoc;
				drawCurrentMap(&mapCurrentLocation);
			}
		} else {
			// Handle moving off the map for towns
			if (currentMapType == TownMapType) {
				exitToOverworld();
			}
		}
	}
}

// Map Info

UInt8 mapTileAt(PointU8 *pt) {
	const UInt8 *runLenPtr = currentRunLenMap;
	UInt8 x = pt->x;
	UInt8 y = pt->y;
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

PointU8 mapEntryPoint(UInt8 mapType) {
	PointU8 pt = {0,0};

	switch (mapType) {
	case OverworldMapType: 
		pt = overworldEntryPoint;
		break;
	case DungeonMapType: 
		pt = dungeonEntryPoint;
		break;
	case TownMapType: 
		pt = townEntryPoint;
		break;
	}
	return pt;
}

// Map Drawing

void loadMap(UInt8 mapType, UInt8 mapSightDistance, PointU8 *location) {
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
	drawCurrentMap(location);
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
	UInt8 *screen = (UInt8 *)PEEKW(SAVMSC);

	memset(screen, 0, 9*SCREEN_WIDTH);
	memset(P3_XPOS, 0, 9);
}

void layoutCurrentMap(UInt8 mapSightDistance) {
	UInt8 x, halfWidth, halfHeight;

	if (mapSightDistance > 3) {
		mapFrameSize.width = 19;
		mapFrameSize.height = 9;
	} else {
		x = mapSightDistance * 2 + 1;
		mapFrameSize.width = x;
		mapFrameSize.height = x;
	}
	halfWidth = mapFrameSize.width / 2;
	halfHeight = mapFrameSize.height / 2;
	// Map screen size is 24 wide by 9 high.
	mapFrameOrigin.x = 11 - halfWidth;
	mapFrameOrigin.y = 4 - halfHeight;

	// Clear out the sprite overlays
	for (x=0; x<9; ++x) {
		P3_XPOS[x] = 0;
	}
}

void drawCurrentMap(PointU8 *center) {
	UInt8 *screen = (UInt8 *)PEEKW(SAVMSC);
	const UInt8 *runLenPtr = currentRunLenMap;
	UInt8 screenRowSkip = SCREEN_WIDTH - mapFrameSize.width;
	UInt8 screenIndex = mapFrameOrigin.x + SCREEN_WIDTH * mapFrameOrigin.y;
	UInt8 mapFrameHalfWidth = mapFrameSize.width / 2;
	UInt8 mapFrameHalfHeight = mapFrameSize.height / 2;
	UInt8 row, col;
	UInt8 leftBlank, leftSkip, decodeLength, topBlank, topSkip;
	UInt8 c, low, hasSpriteOverlay;
	UInt8 buffer[20];

	// Integrity check
	if (runLenPtr == NULL) {
		return;
	}

	// Calculate number of rows to leave blank and how many to skip in the map.
	if (center->y <= mapFrameHalfHeight) {
		topBlank = mapFrameHalfHeight - center->y;
		topSkip = 0;
	} else {
		topBlank = 0;
		topSkip = center->y - mapFrameHalfHeight;

		// Adjust runLenPtr to skip lines within run-len data.
		for (c=0; c<topSkip; ++c) {
			runLenPtr += runLenPtr[0];
		}
	}

	// Calculate number of columns to leave blank and how many to skip in the map.
	if (center->x < mapFrameHalfWidth) {
		leftBlank = mapFrameHalfWidth - center->x;
		leftSkip = 0;
	} else {
		leftBlank = 0;
		leftSkip = center->x - mapFrameHalfWidth;
	}
	decodeLength = mapFrameSize.width - leftBlank;
	if (decodeLength > currentMapSize.width - leftSkip) {
		decodeLength = currentMapSize.width - leftSkip;
	}

	// Debugging:
	// printDebugInfo("B$", leftBlank, 0);
	// printDebugInfo("S$", leftSkip, 10);
	// printDebugInfo("E$", decodeEnd, 20);

	// Main Loop
	for (row=0; row<mapFrameSize.height; ++row) {
		hasSpriteOverlay = 0;

		if (row < topBlank || row + topSkip >= currentMapSize.height + topBlank) {
			// Beyond borders: fill with the default empty tile.
			for (col=0; col<mapFrameSize.width; ++col) {
				screen[screenIndex] = currentTileMap[0];
				++screenIndex;
			}
		} else {
			decodeRunLenRange(buffer, leftSkip, decodeLength, runLenPtr);
			runLenPtr += runLenPtr[0]; // Next row.
			for (col=0; col<mapFrameSize.width; ++col) {
				if (col < leftBlank || col + leftSkip >= currentMapSize.width + leftBlank) {
					c = 0; // Tiles outside map bounds are set to default blank tile.
				} else {
					c = buffer[col - leftBlank];
				}

				// Convert decoded value to character value
				c = currentTileMap[c];

				// Add sprite overlay for special characters
				low = (c & 0x3F);
				if (low >= tCastle) {
					setTileOverlaySprite(tileSprites + 8 * (low - tCastle), col, row);
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
}
