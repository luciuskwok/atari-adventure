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
#define PM_LEFT_MARGIN (48)
#define P2_XPOS (0x0604)

// Globals
UInt8 spritePage;
UInt8 testMap[256]; // holds decoded map 16x16 for testing


// == initGraphics() ==
void initGraphics(void) {
	UInt8 ramtopValue = PEEK(RAMTOP);
	
	
	// Globals
	spritePage = ramtopValue - 8;
	
	// == Colors ==
	POKE (PCOLR0, 0x38); // Player 0 color = red
	POKE (PCOLR1, 0x38); // Player 1 color = red
	POKE (PCOLR2, 0x0F); // Player 2 color = white
	POKE (PCOLR3, 0x0F); // Player 3 color = white
	POKE (COLOR0, 0x1C); // Playfield 0 color = yellow / desert
	POKE (COLOR1, 0x22); // Playfield 1 color = dark brown / plains
	POKE (COLOR2, 0xC6); // Playfield 2 color = green / forest
	POKE (COLOR3, 0x84); // Playfield 3 color = blue / water
	POKE (COLOR4, 0x00); // Playfield 4 color / background = black
	
	// == Other ==
	initDisplayList();
	initSprites();
	initFont(ramtopValue - 12);
	
	// == Set up interrupts ==
	initVBI(immediateUserVBI); // Safe value: 0xE45F
	POKEW (VDSLST, (UInt16)displayListInterrupt);
	ANTIC.nmien = 0xC0; // enable both DLI and VBI
}


// == initDisplayList() ==
void initDisplayList(void) {
	// == Display List ==
	// Display list is already set up by the runtime to the equivalent of BASIC's GR.0. 
	// We can just overwrite that DL with our own.
	UInt8 *displayListPtr = (UInt8 *)PEEKW(SDLSTL);
	UInt8 *screenMemPtr = (UInt8 *)PEEKW(SAVMSC);
	UInt8 *writePtr;
	UInt8 i;
	
	writePtr = displayListPtr + 3;
	*writePtr = DL_DLI(DL_LMS(DL_CHR20x16x2)); // Antic mode 3 + LMS + DLI
	*(++writePtr) = (UInt16)screenMemPtr % 256;
	*(++writePtr) = (UInt16)screenMemPtr / 256;
	
	for (i=1; i<11; ++i) { // 11 lines of double height tiles = 176 scanlines
		*(++writePtr) = DL_DLI(DL_CHR20x16x2); // + DLI on every tile row
	}
	
	*(++writePtr) = DL_BLK4; // 4 blank lines
	*(++writePtr) = DL_CHR40x8x1; // 2 lines of text
	*(++writePtr) = DL_CHR40x8x1; 
	
	*(++writePtr) = DL_JVB; // Vertical blank + jump to beginning of display list
	*(++writePtr) = (UInt16)displayListPtr % 256;
	*(++writePtr) = (UInt16)displayListPtr / 256;
}


// == initFont() ==
void initFont(UInt8 fontPage) {
	UInt8 *customFontPtr = (UInt8 *) ((UInt16)fontPage * 256);
	const UInt8 *romFontPtr = (UInt8 *)0xE000;
	UInt8 *tilePtr;
	UInt16 index;
	UInt8 tileIndex;
	
	// Copy char set from ROM to RAM, 128 characters.
	for (index=0; index<1024; ++index) {
		customFontPtr[index] = romFontPtr[index];
	}
	
	// Add our custom tiles.
	for (tileIndex=0; tileIndex<tileCount; ++tileIndex) {
		// Each tile bitmap has 9 bytes. First byte indicated which character it replaces. 
		// Remove the highest 2 bits because they're used for color data.
		tilePtr = customFontPtr + 8 * tileBitmaps[tileIndex * 9];
		for (index=0; index<8; ++index) {
			tilePtr[index] = tileBitmaps[tileIndex * 9 + index + 1];
		}
	}

	// Switch to custom character set.
	POKE(CHBAS, fontPage);
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
	drawSprite(cursorSprite1, 10, 0, 39);
	drawSprite(cursorSprite2, 10, 1, 39);
	
	// Cursor sprites are in a fixed position
	GTIA_WRITE.hposp0 = 36 + PM_LEFT_MARGIN;
	GTIA_WRITE.hposp1 = 44 + PM_LEFT_MARGIN;

	// Set up ANTIC
	ANTIC.pmbase = spritePage;
	GTIA_WRITE.gractl = 3; // enable both missile and player graphics
	POKE (SDMCTL, 0x2E); // standard playfield + missile DMA + player DMA + display list DMA
	POKE (GPRIOR, 0x01); // layer players above playfield.
}

// == decodeRleMap() ==
void decodeRleMap(UInt8 *outMap, UInt16 mapLength, const UInt8 *inRleMap) {
	UInt16 rowLength, rowEnd;
	UInt16 rleIndex = 0;
	UInt16 outIndex = 0;
	UInt8 op, tile, count;

	printDebugInfo("Len:", mapLength, 0);

	while (outIndex < mapLength) {
		rowLength = inRleMap[rleIndex];
		++rleIndex;
		rowEnd = rleIndex + rowLength;

		while (rleIndex < rowEnd) {
			op = inRleMap[rleIndex];
			++rleIndex;

			tile = (op & 0xF0) >> 4;
			count = (op & 0x0F);

			if (count == 15) {
				count += inRleMap[rleIndex];
				++rleIndex;
			}

			printDebugInfo("Tile:", tile, 0);
			printDebugInfo("Count:", count, 10);

			++count;
			while (count > 0) {
				outMap[outIndex] = tile;
				++outIndex;
				--count;
			}
		}
	}
}

// == clearScreen() ==
void clearScreen(void) {
	UInt8 *screen = (UInt8 *)PEEKW(SAVMSC);
	UInt16 i;
	// Screen is made up of 11 lines * 20 tiles + 2 lines * 40 tiles
	const UInt16 max = 11 * 20 + 2 * 40;
	
	for (i=0; i<max; ++i) {
		screen[i] = 0;
	}
}

// == fillScreen() ==
// Set the screen to show all 256 characters
void fillScreen(void) {
	UInt8 *screen = (UInt8 *)PEEKW(SAVMSC);
	UInt8 y;
	
// 	for (x=0; x<80; ++x) {
// 		screen[220+x] = x;
// 	}

	// Draw the HP and LV tiles
	for (y=1; y<12; y+=3) {
		screen[20 * y + 12] = tLV | 0x40;
		screen[20 * y + 15] = tHP | 0x40;
	}
	
	// Draw health potion and fang tiles
	screen[220 + 1 * 20 + 13] = tPotion;
	screen[220 + 3 * 20 + 13] = tFang;
	
	
	printString("ALISA", 0, 12, 0);
	printString("99", 0, 13, 1); // Lv
	printString("123", 0, 16, 1); // HP

	printString("MARIE", 2, 12, 3);
	printString("1", 2, 13, 4); // Lv
	printString("4", 2, 16, 4); // HP
	
	printString("GUY", 0, 12, 6);
	printString("19", 0, 13, 7); // Lv
	printString("67", 0, 16, 7); // HP

	printString("NYORN", 2, 12, 9);
	printString("7", 2, 13, 10); // Lv
	printString("78", 2, 16, 10); // HP
	
	printString("$999,999", 0, 4, 12);
	printString("Rep:2010", 0, 4, 14);
	printString("21", 0, 14, 12);
	printString("1999", 0, 14, 14);

// 	printDebugInfo("DLI:", (UInt16)displayListInterrupt, 0);
}

// == drawMap() ==
void drawMap(const UInt8 *map, UInt8 mapWidth, UInt8 mapHeight, UInt8 centerX, UInt8 centerY) {
	UInt8 *screen = (UInt8 *)PEEKW(SAVMSC);
	const UInt8 windowWidth = 11;
	const UInt8 windowHeight = 11;
	const int windowOriginX = (int)centerX - 5;
	const int windowOriginY = (int)centerY - 5;
	UInt8 row, col, c;
	int x, y, screenIndex;
	
	
	for (row=0; row<windowHeight; ++row) {
		y = windowOriginY + row;
		
		// Clear the sprite tile for this row
		POKE(P2_XPOS + row, 0);
		
		for (col=0; col<windowWidth; ++col) {
			x = windowOriginX + col;
			screenIndex = col + 20 * row;
			if (0 <= y && y < mapHeight && 0 <= x && x < mapWidth) {
				c = map[x + mapWidth * y];
				
				// Add sprite tile for special tiles
				if (1 <= c && c <5) {
					drawSpriteTile(tileSprites + 8 * (c-1), col, row);
				}

				// Convert map value to character value
				c = tileChars[c];				
			} else {
				c = 0;
			}
			screen[screenIndex] = c;
		}
	}
}


// == drawSprite() ==
void drawSprite(const UInt8 *sprite, char spriteLength, char player, char y) {
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
void drawSpriteTile(const UInt8 *sprite, char column, char row) {
	// Set horizontal position for tile
	POKE(P2_XPOS + row, PM_LEFT_MARGIN + 8 * column);
	
	// Draw sprite at vertical position
	drawSprite(sprite, 8, 2, row * 8);
}
