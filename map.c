// map.c

#include "map.h"
#include "graphics.h"
#include "tiles.h"
#include "map_overworld.h"
#include "map_dungeons.h"
#include "map_towns.h"
#include "atari_memmap.h"
//#include <atari.h>


// Globals
const UInt8 *currentRunLenMap;
const UInt8 *currentTileMap;
SizeU8 currentMapSize;
UInt8 currentMapType;
PointU8 mapFrameOrigin;
SizeU8 mapFrameSize;

#define SCREEN_WIDTH (24)


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
	decodeRunLenRange(&tile, x, x+1, runLenPtr);

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


void loadMap(UInt8 mapType, UInt8 sightDistance, PointU8 *location) {
	const UInt8 *colorTable;

	hidePlayfieldAndSprites();
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

	layoutCurrentMap(sightDistance);
	drawCurrentMap(location);
	loadColorTable(colorTable);
	setPlayerCursorVisible(1);
}


void clearMapScreen(void) {
	UInt8 *screen = (UInt8 *)PEEKW(SAVMSC);
	UInt8 i;
	// Screen is made up of 9 lines * 24 tiles = 216 tiles
	for (i=0; i<(9*24); ++i) {
		screen[i] = 0;
	}
	// Clear out the sprite overlays
	for (i=0; i<9; ++i) {
		P3_XPOS[i] = 0;
	}
}


void layoutCurrentMap(UInt8 sightDistance) {
	UInt8 x, halfWidth, halfHeight;

	if (sightDistance > 3) {
		mapFrameSize.width = 19;
		mapFrameSize.height = 9;
	} else {
		x = sightDistance * 2 + 1;
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

	// printDebugInfo("W $", mapFrameSize.width, 40);
	// printDebugInfo("H $", mapFrameSize.height, 50);
}


void drawCurrentMap(PointU8 *center) {
	UInt8 *screen = (UInt8 *)PEEKW(SAVMSC);
	const UInt8 *runLenPtr = currentRunLenMap;
	UInt8 screenRowSkip = SCREEN_WIDTH - mapFrameSize.width;
	UInt8 screenIndex = mapFrameOrigin.x + SCREEN_WIDTH * mapFrameOrigin.y;
	UInt8 mapFrameHalfWidth = mapFrameSize.width / 2;
	UInt8 mapFrameHalfHeight = mapFrameSize.height / 2;
	UInt8 row, col;
	UInt8 leftBlank, leftSkip, decodeEnd, topBlank, topSkip;
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
	decodeEnd = leftSkip + mapFrameSize.width - leftBlank;
	if (decodeEnd > currentMapSize.width) {
		decodeEnd = currentMapSize.width;
	}

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
			decodeRunLenRange(buffer, leftSkip, decodeEnd, runLenPtr);
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


void decodeRunLenRange(UInt8 *outData, UInt8 start, UInt8 end, const UInt8 *runLenData) {
	UInt8 rowLength = runLenData[0];
	UInt8 runLenIndex = 1;
	UInt8 outIndex = 0;
	UInt8 op, tile, count;

	while (runLenIndex < rowLength && outIndex < end) {
		op = runLenData[runLenIndex];
		++runLenIndex;

		tile = (op & 0xF0) >> 4;
		count = (op & 0x0F);

		if (count == 15) {
			count += runLenData[runLenIndex];
			++runLenIndex;
		}

		++count;
		while (count > 0 && outIndex < end) {
			if (outIndex >= start) {
				outData[outIndex - start] = tile;
			}
			++outIndex;
			--count;
		}		
	}
}



