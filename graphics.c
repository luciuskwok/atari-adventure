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
UInt8 spritePage;
UInt8 testMap[256]; // holds decoded map 16x16 for testing
UInt8 *textWindow;


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

	// == Test Scrolling ==
	ANTIC.hscrol = 4;
	ANTIC.vscrol = 8;
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
	UInt8 *customFontPtr = (UInt8 *) ((UInt16)fontPage * 256);
	const UInt8 *romFontPtr = (UInt8 *)0xE000;
	UInt8 *tilePtr;
	UInt16 index;
	UInt8 tileIndex;
	
	// Copy character set from ROM to RAM, 128 characters.
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

// == decodeRleMap() ==
void decodeRleMap(UInt8 *outMap, UInt16 mapLength, const UInt8 *inRleMap) {
	UInt16 rowLength, rowEnd;
	UInt16 rleIndex = 0;
	UInt16 outIndex = 0;
	UInt8 op, tile, count;

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

// == clearScreen() ==
void clearScreen(void) {
	UInt8 *screen = (UInt8 *)PEEKW(SAVMSC);
	UInt16 i;
	// Screen is made up of 10 lines * 24 tiles + 5 lines * 48 tiles = 480
	for (i=0; i<512; ++i) {
		screen[i] = 0;
	}
}

// == drawMap() ==
void drawMap(const UInt8 *map, UInt8 mapWidth, UInt8 mapHeight, UInt8 centerX, UInt8 centerY) {
	UInt8 *screen = (UInt8 *)PEEKW(SAVMSC);
	const int windowOriginX = (int)centerX - 9;
	const int windowOriginY = (int)centerY - 4;
	const UInt8 windowWidth = 19;
	const UInt8 windowHeight = 9;
	const UInt8 leftMargin = 2;
	UInt8 row, col, c;
	int x, y, screenIndex;
	
	
	for (row=0; row<windowHeight; ++row) {
		y = windowOriginY + row;
		
		// Clear the sprite tile for this row
		POKE(P2_XPOS + row, 0);
		
		for (col=0; col<windowWidth; ++col) {
			x = windowOriginX + col;
			screenIndex = col + SCREEN_WIDTH * row + leftMargin;
			if (0 <= y && y < mapHeight && 0 <= x && x < mapWidth) {
				c = map[x + mapWidth * y];
				
				// Add sprite tile for special tiles
				if (1 <= c && c <= 5) {
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
