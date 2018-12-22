// graphics.c

#include "graphics.h"
#include "text.h"
#include "tiles.h"
#include "atari_memmap.h"
#include <atari.h>


extern void __fastcall__ initVBI(void *addr);
extern void __fastcall__ immediateUserVBI(void);
extern void __fastcall__ displayListInterrupt(void);


// Constants
#define PM_LEFT_MARGIN (52)
#define SCREEN_WIDTH (24)
#define CUR_TIMER (0x0600)
#define P2_XPOS ((UInt8 *)0x0610)
#define BG_COLOR ((UInt8 *)0x0620)

// Globals
UInt8 *textWindow;
const UInt8 *currentRunLenMap;
UInt8 *currentTileMap;
SizeU8 currentMapSize;
UInt8 currentMapType;

// Private globals 
UInt8 spritePage; // shadow of ANTIC hardware register, which cannot be read
PointU8 mapFrameOrigin;
SizeU8 mapFrameSize;



// Init


void initGraphics(void) {
	UInt8 ramtopValue = PEEK(RAMTOP);
	
	// Globals
	spritePage = ramtopValue - 8;
	
	// == Colors ==
	loadColorTable(NULL);
	POKE (PCOLR0, 0x58); // Player cursor color: purple
	POKE (PCOLR1, 0x58); 

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

	// Debugging
	// printDebugInfo("VBI ", (UInt16)immediateUserVBI, 0);

}


void initDisplayList(void) {
	// == Display List ==
	// Display list is already set up by the runtime to the equivalent of BASIC's GR.0. 
	// We can just overwrite that DL with our own.
	UInt8 *displayList = (UInt8 *)PEEKW(SDLSTL);
	UInt8 *screen = (UInt8 *)PEEKW(SAVMSC);
	const UInt8 dl_Interrupt = 0x80;
	const UInt8 dl_LMS = 0x40;
	const UInt8 dl_VScroll = 0x20;
	const UInt8 dl_HScroll = 0x10;
	const UInt8 colorModeLine = (dl_Interrupt | dl_HScroll | 7);
	UInt8 x = 3; // Skip DL instructions that are already blank rows.
	UInt8 i;


	displayList[x] = colorModeLine | dl_LMS;
	displayList[++x] = (UInt16)screen % 256;
	displayList[++x] = (UInt16)screen / 256;
	
	for (i=1; i<9; ++i) { // 9 rows * 16 scanlines = 144 scanlines
		displayList[++x] = colorModeLine; // DLI on every tile row
	}
	
	displayList[++x] = DL_BLK8; // 8 blank scanlines

	for (i=0; i<5; ++i) { // 5 rows of text = 40 scanlines
		displayList[++x] = DL_CHR40x8x1; 
	}
	
	displayList[++x] = DL_JVB; // Vertical blank + jump to beginning of display list
	displayList[++x] = (UInt16)displayList % 256;
	displayList[++x] = (UInt16)displayList / 256;

	// Set the textWindow pointer based on DL memory usage
	textWindow = screen + (9 * 24);
}


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


void initSprites(void) {
	UInt8 *pmbasePtr = (UInt8 *) ((UInt16)spritePage * 256 + 384);
	UInt16 i;
	UInt8 c;
	
	// Zero out sprite memory area
	for (i=0; i<640; ++i) {
		pmbasePtr[i] = 0;
	}
	
	// Clear GTIA registers
	for (c=0; c<12; ++c) {
		POKE (HPOSP0 + c, 0);
	}
	
	// Set up ANTIC
	ANTIC.pmbase = spritePage;
	POKE (SDMCTL, 0x2E); // standard playfield + missile DMA + player DMA + display list DMA
	POKE (GPRIOR, 0x01); // layer players above playfield.
	GTIA_WRITE.gractl = 3; // enable both missile and player graphics
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
		P2_XPOS[i] = 0;
	}
}


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

// ==========================================================================
// ==== Color Table ====

void blackOutColorTable(void) {
	UInt8 *p = (UInt8*)(PCOLR0);
	UInt8 i;
	for (i=0; i<9; ++i) {
		p[i] = 0;
	}
}

void loadColorTableForCurrentMap(void) {
	switch (currentMapType) {
		case DungeonMapType: 
			loadColorTable(dungeonColorTable);
			break;
		case TownMapType: 
			loadColorTable(townColorTable);
			break;
		case OverworldMapType: 
		default:
			loadColorTable(overworldColorTable);
			break;
	}
}

void loadColorTable(const UInt8 *colors) {
	UInt8 *p = (UInt8*)(PCOLR0);
	UInt8 i;
	for (i=0; i<9; ++i) {
		p[i] = colors[i];
	}
}

void setBackgroundGradient(const UInt8 *colors) {
	UInt8 x;

	// First color goes into shadow register, which the OS VBI uses.
	POKE(COLOR4, colors[0]);

	for (x=0; x<10; ++x) {
		if (colors) {
			BG_COLOR[x] = colors[x];
		} else {
			BG_COLOR[x] = 0;
		}
	}
}

// ==========================================================================
// ==== Map Drawing ====


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
		P2_XPOS[x] = 0;
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
			P2_XPOS[row] = 0;
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



// ==========================================================================
// Getting Map Info


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


// ==========================================================================
// Player Cursor


void setPlayerCursorVisible(UInt8 visible) {
	if (visible) {
		// Draw cursor sprite, which takes up 2 players because it is 10 pixels wide
		drawSprite(cursorSprite1, 10, 1, 31 + 16);
		drawSprite(cursorSprite2, 10, 2, 31 + 16);
		
		// Position sprites		
		GTIA_WRITE.hposp0 = PM_LEFT_MARGIN + (9 * 8) - 4;
		GTIA_WRITE.hposp1 = PM_LEFT_MARGIN + (9 * 8) + 4;

		// Enable color cycling
		POKE (CUR_TIMER, 15);
	} else {
		GTIA_WRITE.hposp0 = 0;
		GTIA_WRITE.hposp1 = 0;	

		// Disable color cycling
		POKE (CUR_TIMER, 0xFF);
	}
}


void drawSprite(const UInt8 *sprite, UInt8 length, UInt8 player, UInt8 y) {
	// player: 0=missile, 1-4=p0-p3.
	// this simplifies the math and allows both players and missiles to be addressed
	UInt8 *p = (UInt8 *) (384 + 256 * spritePage + 128 * player + y);
	UInt8 i;
	
	for (i=0; i<length; ++i) {
		p[i] = sprite[i];
	}
}


void setTileOverlaySprite(const UInt8 *sprite, UInt8 column, UInt8 row) {
	// Set horizontal position for tile
	P2_XPOS[row] = PM_LEFT_MARGIN + 8 * column;
	
	// Draw sprite at vertical position
	drawSprite(sprite, 8, 3, row * 8 + 16);
}

void clearSprite(UInt8 player) {
	UInt8 *pmbasePtr = (UInt8 *) (256 * spritePage + 384 + 128 * player);
	UInt8 i;

	for (i=0; i<128; ++i) {
		pmbasePtr[i] = 0;
	}
}



// ==========================================================================
// ==== Testing ====


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


