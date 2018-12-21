// graphics.c

#include "graphics.h"
#include "text.h"
#include "tiles.h"
#include "atari_memmap.h"
#include <atari.h>


extern void __fastcall__ initVBI(void *addr);
extern void __fastcall__ immediateUserVBI(void);
extern void __fastcall__ displayListInterrupt(void);


// Player-Missile Constants
#define PM_LEFT_MARGIN (52)
#define P2_XPOS (0x0604)
#define SCREEN_WIDTH (24)

// Globals
UInt8 *textWindow;
UInt8 buffer[256];
const UInt8 *currentRunLenMap;
UInt8 *currentTileMap;
SizeU8 currentMapSize;
UInt8 currentMapType;

// Private globals 
UInt8 spritePage; // shadow of ANTIC hardware register, which cannot be read
PointU8 mapFrameOrigin;
SizeU8 mapFrameSize;


// == initGraphics() ==
void initGraphics(void) {
	UInt8 ramtopValue = PEEK(RAMTOP);
	
	// Globals
	spritePage = ramtopValue - 8;
	
	// == Colors ==
	POKE (PCOLR0, 0x38); // Player 0 color = red
	POKE (PCOLR1, 0x38); // Player 1 color = red
	loadColorTable(NULL);

	// == Other ==
	initDisplayList();
	initSprites();
	initFont(ramtopValue - 12);
	
	// == Set up interrupts ==
	initVBI(immediateUserVBI); // Safe value: 0xE45F
	POKEW (VDSLST, (UInt16)displayListInterrupt);
	ANTIC.nmien = 0xC0; // enable both DLI and VBI

	// == Use scrolling to center the odd number of tiles ==
	ANTIC.hscrol = 4;
}


// == initDisplayList() ==
void initDisplayList(void) {
	// == Display List ==
	// Display list is already set up by the runtime to the equivalent of BASIC's GR.0. 
	// We can just overwrite that DL with our own.
	UInt8 *displayListPtr = (UInt8 *)PEEKW(SDLSTL);
	UInt8 *write = displayListPtr;
	UInt8 *screenMemPtr = (UInt8 *)PEEKW(SAVMSC);
	UInt8 i;
	const UInt8 dl_Interrupt = 0x80;
	const UInt8 dl_LMS = 0x40;
	const UInt8 dl_VScroll = 0x20;
	const UInt8 dl_HScroll = 0x10;
	const UInt8 colorModeLine = (dl_Interrupt | dl_HScroll | 7);


	write += 3; // Skip DL instructions that are already blank rows.
	*write = colorModeLine | dl_LMS;
	*(++write) = (UInt16)screenMemPtr % 256;
	*(++write) = (UInt16)screenMemPtr / 256;
	
	for (i=1; i<9; ++i) { // 9 rows * 16 scanlines = 144 scanlines
		*(++write) = colorModeLine; // DLI on every tile row
	}
	
	*(++write) = DL_BLK8; // 8 blank scanlines

	for (i=0; i<5; ++i) { // 5 rows of text = 40 scanlines
		*(++write) = DL_CHR40x8x1; 
	}
	
	*(++write) = DL_JVB; // Vertical blank + jump to beginning of display list
	*(++write) = (UInt16)displayListPtr % 256;
	*(++write) = (UInt16)displayListPtr / 256;

	// Set the textWindow pointer based on DL memory usage
	textWindow = screenMemPtr + (9 * 24);
}


// == initFont() ==
void initFont(UInt8 fontPage) {
	const UInt8 *romFont = (UInt8 *)0xE000;
	UInt8 *customFont = (UInt8 *) ((UInt16)fontPage * 256);
	UInt8 *tileFont = customFont + (0x40 * 8);
	UInt16 index;
	UInt8 tileIndex, bitmapIndex;
	
	// Copy character set from ROM to RAM, 128 characters.
	for (index=0; index<1024; ++index) {
		customFont[index] = romFont[index];
	}

	// Blank out the tile with value 0 in the graphics character area
	for (index=0; index<8; ++index) {
		tileFont[index] = 0;
	}
	
	// Add our custom tiles into the graphics character area
	bitmapIndex = 0;
	while (1) {
		// Each tile bitmap has 9 bytes. First byte indicated which character it replaces, 
		// or nil for the end of the data.
		tileIndex = tileBitmaps[bitmapIndex * 9];
		if (tileIndex == 0) {
			break;
		}
		for (index=0; index<8; ++index) { 
			tileFont[tileIndex * 8 + index] = tileBitmaps[bitmapIndex * 9 + index + 1];
		}
		++bitmapIndex;
	}

	// Set CHBAS to point to the graphics character set area.
	// This lets the  map tiles show in the color map part of the screen.
	// It seems that the text window area truncates the value to a multple of 4, 
	// neatly allowing for regular characters there.
	POKE(CHBAS, fontPage + 2);
}

// == initSprites() ==
void initSprites(void) {
	UInt8 *pmbasePtr = (UInt8 *) ((UInt16)spritePage * 256 + 384);
	UInt16 i;
	UInt8 c;
	
	// Zero out memory
	for (i=0; i<640; ++i) {
		pmbasePtr[i] = 0;
	}
	
	// Clear GTIA registers
	for (c=0; c<12; ++c) {
		POKE (HPOSP0 + c, 0);
	}
	
	// Draw cursor sprite, which takes up 2 players because it is 10 pixels wide
	drawSprite(cursorSprite1, 10, 0, 31);
	drawSprite(cursorSprite2, 10, 1, 31);
	
	// Cursor sprites are in a fixed position
	GTIA_WRITE.hposp0 = PM_LEFT_MARGIN + 9 * 8 - 4;
	GTIA_WRITE.hposp1 = PM_LEFT_MARGIN + 10 * 8 - 4;

	// Set up ANTIC
	ANTIC.pmbase = spritePage;
	GTIA_WRITE.gractl = 3; // enable both missile and player graphics
	POKE (SDMCTL, 0x2E); // standard playfield + missile DMA + player DMA + display list DMA
	POKE (GPRIOR, 0x01); // layer players above playfield.
}


// ==========================================================================================


// == clearMapScreen() ==
void clearMapScreen(void) {
	UInt8 *screen = (UInt8 *)PEEKW(SAVMSC);
	UInt16 i;
	// Screen is made up of 9 lines * 24 tiles = 216 tiles
	for (i=0; i<(9*24); ++i) {
		screen[i] = 0;
	}
}

// == loadMap() ==
void loadMap(UInt8 mapType, UInt8 variation) {
	switch (mapType) {
		case OverworldMapType: 
			currentRunLenMap = overworldRleMap;
			currentMapSize = overworldMapSize;
			currentTileMap = overworldTileMap;
			break;
		case DungeonMapType: 
			currentRunLenMap = dungeonRleMap;
			currentMapSize = dungeonMapSize;
			currentTileMap = dungeonTileMap;
			break;
		case TownMapType: 
			currentRunLenMap = townRleMap;
			currentMapSize = townMapSize;
			currentTileMap = townTileMap;
			break;
	}
	currentMapType = mapType;
}

// == loadColorTable() ==
void loadColorTable(const UInt8 *colors) {
	UInt8 i;
	for (i=0; i<5; ++i) {
		if (colors == NULL) {
			POKE(COLOR0 + i, 0);
		} else {
			POKE(COLOR0 + i, colors[i]);
		}
	}

	// Also change the sprite overlay colors
	if (colors == NULL) {
		POKE (PCOLR2, 0x00);
	} else {
		POKE (PCOLR2, 0x0F);
	}
}

// == decodeRunLenMap() ==
void decodeRunLenMap(UInt8 *outMap, UInt16 mapLength, const UInt8 *inRunLenMap) {
	UInt16 rowEnd;
	UInt16 rleIndex = 0;
	UInt16 outIndex = 0;
	UInt8 op, tile, count;

	while (outIndex < mapLength) {
		rowEnd = rleIndex + inRunLenMap[rleIndex];
		++rleIndex;

		while (rleIndex < rowEnd) {
			op = inRunLenMap[rleIndex];
			++rleIndex;

			tile = (op & 0xF0) >> 4;
			count = (op & 0x0F);

			if (count == 15) {
				count += inRunLenMap[rleIndex];
				++rleIndex;
			}

			//printDebugInfo("Tile:", tile, 0);
			//printDebugInfo("Count:", count, 10);

			++count;
			while (count > 0) {
				outMap[outIndex] = tile;
				++outIndex;
				--count;
			}
		}
	}
}


// ==========================================================================================


// == layoutCurrentMap() ==
void layoutCurrentMap(UInt8 sightDistance) {
	UInt8 *overlay = (UInt8 *)P2_XPOS;
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
		overlay[x] = 0;
	}

	// printDebugInfo("W $", mapFrameSize.width, 40);
	// printDebugInfo("H $", mapFrameSize.height, 50);
}

// == drawCurrentMap() ==
void drawCurrentMap(PointU8 *center) {
	UInt8 *screen = (UInt8 *)PEEKW(SAVMSC);
	UInt8 *overlay = (UInt8 *)P2_XPOS;
	UInt8 screenRowSkip = SCREEN_WIDTH - mapFrameSize.width;
	UInt8 screenIndex = mapFrameOrigin.x + SCREEN_WIDTH * mapFrameOrigin.y;
	UInt8 mapFrameHalfWidth = mapFrameSize.width / 2;
	UInt8 mapY = center->y - mapFrameSize.height / 2;
	UInt8 runLenIndex = 0;
	UInt8 row, col, leftBlank, leftSkip, c, low, hasSpriteOverlay;

	// Integrity check
	if (currentRunLenMap == NULL) {
		return;
	}

	// Skip within RLE data to first row that is displayed.
	if (mapY < currentMapSize.height) { // verify mapY is within bounds, negative values wrap 
		for (c=0; c<mapY; ++c) {
			if (currentRunLenMap[runLenIndex] == 0) { // Integrity check
				return;
			}
			runLenIndex += currentRunLenMap[runLenIndex]; // skip length byte + row data
		}
	}

	// Calculate number of columns to leave blank and how many to skip.
	if (center->x < mapFrameHalfWidth) {
		leftBlank = mapFrameHalfWidth - center->x;
		leftSkip = 0;
	} else {
		leftBlank = 0;
		leftSkip = center->x - mapFrameHalfWidth;
	}

	// Main Loop
	for (row=0; row<mapFrameSize.height; ++row) {
		hasSpriteOverlay = 0;

		if (mapY >= currentMapSize.height) {
			// Beyond borders: fill with the default empty tile.
			for (col=0; col<mapFrameSize.width; ++col) {
				screen[screenIndex] = currentTileMap[0];
				++screenIndex;
			}
		} else {	
			runLenIndex += decodeRunLenRow(buffer, currentMapSize.width, currentRunLenMap + runLenIndex);
			for (col=0; col<mapFrameSize.width; ++col) {
				if (col < leftBlank || col + leftSkip >= currentMapSize.width) {
					c = 0; // Tiles outside map bounds are set to default blank tile.			
				} else {
					c = buffer[col + leftSkip - leftBlank];
				}

				// Convert decoded value to character value
				c = currentTileMap[c];				


				// Add sprite overlay for special characters
				low = (c & 0x3F);
				if (low >= tCastle) {
					drawSpriteTile(tileSprites + 8 * (low - tCastle), col, row);
					hasSpriteOverlay = 1;
				}

				screen[screenIndex] = c;
				++screenIndex;
			} // end for(col)
		} // end if
			
		if (hasSpriteOverlay == 0) {
			// Clear the sprite overlay for this row
			overlay[row] = 0;
		}
		screenIndex += screenRowSkip;
		++mapY;
	}
}

// == decodeRunLenRow() ==
UInt8 decodeRunLenRow(UInt8 *outData, UInt8 length, const UInt8 *runLenData) {
	UInt8 rowLength = runLenData[0];
	UInt8 runLenIndex = 1;
	UInt8 outIndex = 0;
	UInt8 op, tile, count;

	while (runLenIndex < rowLength) {
		op = runLenData[runLenIndex];
		++runLenIndex;

		tile = (op & 0xF0) >> 4;
		count = (op & 0x0F);

		if (count == 15) {
			count += runLenData[runLenIndex];
			++runLenIndex;
		}

		++count;
		while (count > 0 && outIndex < length) {
			outData[outIndex] = tile;
			++outIndex;
			--count;
		}		
	}
	return rowLength;
}


// == drawCurrentMapOLD() ==
// void drawCurrentMapOLD(PointU8 *center) {
// 	UInt8 *screen = (UInt8 *)PEEKW(SAVMSC);
// 	UInt8 *overlay = (UInt8 *)P2_XPOS;
// 	UInt8 row, col, mapX, mapY, startMapX, c, low, hasSpriteOverlay;
// 	UInt8 rowSkip = SCREEN_WIDTH - mapFrameSize.width;
// 	UInt8 screenIndex = mapFrameOrigin.x + SCREEN_WIDTH * mapFrameOrigin.y;

// 	startMapX = center->x - mapFrameSize.width / 2;
// 	mapY = center->y - mapFrameSize.height / 2;

// 	for (row=0; row<mapFrameSize.height; ++row) {
// 		hasSpriteOverlay = 0;
// 		mapX = startMapX;
// 		for (col=0; col<mapFrameSize.width; ++col) {
// 			if (mapY < currentMapSize.height && mapX < currentMapSize.width) {
// 				c = currentRawMap[mapX + currentMapSize.width * mapY];
// 			} else {
// 				c = 0;
// 			}

// 			// Convert map value to character value
// 			c = currentTileMap[c];				

// 			// Add sprite tile for characters
// 			low = (c & 0x3F);
// 			if (low >= tCastle) {
// 				drawSpriteTile(tileSprites + 8 * (low - tCastle), col, row);
// 				hasSpriteOverlay = 1;
// 			}

// 			screen[screenIndex] = c;
// 			++screenIndex;
// 			++mapX;
// 		}

// 		if (hasSpriteOverlay == 0) {
// 			// Clear the sprite overlay for this row
// 			overlay[row] = 0;
// 		}
// 		screenIndex += rowSkip;
// 		++mapY;
// 	}
// }


// == drawSprite() ==
void drawSprite(const UInt8 *sprite, UInt8 spriteLength, UInt8 player, UInt8 y) {
	UInt8 *pmbasePtr = (UInt8 *) (spritePage * 256 + 384);
	const UInt16 pmLength = 128;
	UInt16 offset = y + 16; // overscan area
	UInt16 i;
	
	// Copy sprite data at Y position
	pmbasePtr += pmLength * (player + 1);
	for (i=0; i<spriteLength; ++i) {
		pmbasePtr[i + offset] = sprite[i];
	}
}

// == drawSpriteTile() ==
void drawSpriteTile(const UInt8 *sprite, UInt8 column, UInt8 row) {
	// Set horizontal position for tile
	POKE(P2_XPOS + row, PM_LEFT_MARGIN + 8 * column);
	
	// Draw sprite at vertical position
	drawSprite(sprite, 8, 2, row * 8);
}
