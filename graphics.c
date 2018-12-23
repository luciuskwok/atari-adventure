// graphics.c

/*
== Notes on Memory Usage ==
In the config.cfg file, the reserved memory is set to 0x1020, giving us 4KB 
of space below the 1 KB display area for a total of 5KB. The space between 
APPMHI and MEMTOP should be ours to use. 

RAMTOP: 0xC0 without BASIC, 0xA0 with BASIC. Below are values with BASIC.

0x9C00: 1 KB: PMGraphics needs 640 bytes (double-line sprites), but the data area 
	doesn't start until 384 (0x180) bytes after PMBase, which must be on 1KB boundary.
0x9000: 3 KB: Display list and screen memory, which must be on 4 KB boundary.
0x8C00: 1 KB: Custom character set needs 128 chars * 8 bytes = 1024 bytes.

Screen memory is allocated:
- Map View:
	- Display List: 32 bytes
	- Screen memory: 24x9 = 216 bytes
- Story View:
	- Display List: 96 bytes
	- Screen memory: 40x72 = 2,880 bytes
- Total: about 3,008 bytes if screen memory is shared between the 2 views.
- Shared text window: 40x6 = 240 bytes. Goes into memory hole at start of PMGraphics.

- Display list should not cross a 1KB boundary (0x0400)
- Screen memory should not cross a 4KB boundary (0x1000)

*/


#include "graphics.h"
#include "text.h"
#include "tiles.h"
#include "map.h"
#include "atari_memmap.h"
#include <atari.h>


extern void __fastcall__ initVBI(void *addr);
extern void __fastcall__ immediateUserVBI(void);
extern void __fastcall__ mapViewDLI(void);
extern void __fastcall__ storyViewDLI(void);



// Constants
#define PM_LEFT_MARGIN (48)
#define CUR_TIMER (0x0600)

// Globals
UInt8 *textWindow;
UInt8 *mapViewDisplayList;
UInt8 *storyViewDisplayList;

// Private globals 
UInt8 spritePage; // shadow of ANTIC hardware register, which cannot be read



// Init


void initGraphics(void) {
	UInt8 ramtopValue = PEEK(RAMTOP);
	
	spritePage = ramtopValue - 16;

	initVBI(immediateUserVBI); // Safe value: 0xE45F
	
	// Set color table to all black and change the display list
	//hidePlayfieldAndSprites();
	initDisplayList(ramtopValue - 12);
	selectDisplayList(1);
	initSprites();
	initFont(ramtopValue - 20);
	
	// == Use scrolling to center the odd number of tiles ==
	ANTIC.hscrol = 4;

	// Debugging
	// print16bitValue("Map DL: ", (UInt16)mapViewDisplayList, 1, 1);
	// print16bitValue("Story DL: ", (UInt16)storyViewDisplayList, 1, 2);
	// print16bitValue("textWindow: ", (UInt16)textWindow, 1, 3);	

}


void initDisplayList(UInt8 startPage) {
	// Create two display lists, one for map view, and the other for story view.
	const UInt8 dl_Interrupt = 0x80;
	const UInt8 dl_LMS = 0x40;
	const UInt8 dl_VScroll = 0x20;
	const UInt8 dl_HScroll = 0x10;
	const UInt8 mapTileLine = (dl_Interrupt | dl_HScroll | 7);
	const UInt8 rasterLine = (dl_Interrupt | 13);
	const UInt8 textWindowLine = 2;
	UInt8 *screen = (UInt8 *)(startPage * 256 + 128);
	UInt8 x = 0;
	UInt8 i;


	// Allocate display memory usage
	mapViewDisplayList = (UInt8 *)(startPage * 256);
	storyViewDisplayList = mapViewDisplayList + 32;
	textWindow = (UInt8 *)(spritePage * 256); // Using the unused memory area below PMGraphics

	// Update screen memory pointer
	POKEW (SAVMSC, (UInt16)screen);

	// == Map View DL ==
	for (i=0; i<3; ++i) {
		mapViewDisplayList[x++] = DL_BLK8;
	}

	mapViewDisplayList[x++] = mapTileLine | dl_LMS;
	mapViewDisplayList[x++] = (UInt16)screen % 256;
	mapViewDisplayList[x++] = (UInt16)screen / 256;
	
	for (i=1; i<9; ++i) { // 9 rows * 16 scanlines = 144 scanlines
		mapViewDisplayList[x++] = mapTileLine; // DLI on every tile row
	}
	
	mapViewDisplayList[x++] = DL_BLK8; // 8 blank scanlines

	// Text window
	mapViewDisplayList[x++] = textWindowLine | dl_LMS;
	mapViewDisplayList[x++] = (UInt16)textWindow % 256;
	mapViewDisplayList[x++] = (UInt16)textWindow / 256;

	for (i=1; i<5; ++i) { // 5 rows of text = 40 scanlines
		mapViewDisplayList[x++] = textWindowLine; 
	}
	
	mapViewDisplayList[x++] = DL_JVB; // Vertical blank + jump to beginning of display list
	mapViewDisplayList[x++] = (UInt16)mapViewDisplayList % 256;
	mapViewDisplayList[x++] = (UInt16)mapViewDisplayList / 256;

	// == Story View DL ==
	x = 0;
	for (i=0; i<3; ++i) {
		storyViewDisplayList[x++] = DL_BLK8;
	}

	storyViewDisplayList[x++] = rasterLine | dl_LMS;
	storyViewDisplayList[x++] = (UInt16)screen % 256;
	storyViewDisplayList[x++] = (UInt16)screen / 256;
	
	for (i=1; i<72; ++i) { // 72 rows * 2 scanlines = 144 scanlines
		storyViewDisplayList[x++] = rasterLine; // DLI on every tile row
	}
	
	storyViewDisplayList[x++] = DL_BLK8; // 8 blank scanlines

	// Text window
	storyViewDisplayList[x++] = textWindowLine | dl_LMS;
	storyViewDisplayList[x++] = (UInt16)textWindow % 256;
	storyViewDisplayList[x++] = (UInt16)textWindow / 256;

	for (i=1; i<5; ++i) { // 5 rows of text = 40 scanlines
		storyViewDisplayList[x++] = textWindowLine; 
	}
	
	storyViewDisplayList[x++] = DL_JVB; // Vertical blank + jump to beginning of display list
	storyViewDisplayList[x++] = (UInt16)storyViewDisplayList % 256;
	storyViewDisplayList[x++] = (UInt16)storyViewDisplayList / 256;
}


void selectDisplayList(UInt8 index) {
	POKE (SDMCTL, 0); // turn off DMA and DLI before changing pointers
	ANTIC.nmien = 0x40;

	switch (index) {
	case 1: // Map view
		POKEW (SDLSTL, (UInt16)mapViewDisplayList);
		POKEW (VDSLST, (UInt16)mapViewDLI);
		break;
	case 2: // Story view
		POKEW (SDLSTL, (UInt16)storyViewDisplayList);
		POKEW (VDSLST, (UInt16)storyViewDLI);
		break;
	default: // No DL: keep everything off.
		return;
	}

	ANTIC.nmien = 0xC0; // enable both DLI and VBI
	POKE (SDMCTL, 0x2E); // standard playfield + missile DMA + player DMA + display list DMA
}


void initFont(UInt8 fontPage) {
	const UInt8 *romFont = (UInt8 *)0xE000;
	UInt8 *customFont = (UInt8 *) ((UInt16)fontPage * 256);
	UInt8 *tileFont = customFont + 512;
	UInt16 index;
	UInt8 tileIndex, bitmapIndex;

	//print8bitValue("Start Font: ", fontPage, 1, 5);
	
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
	POKE (GPRIOR, 0x01); // layer players above playfield.
	GTIA_WRITE.gractl = 3; // enable both missile and player graphics
}


// ==== Color Table ====


void hidePlayfieldAndSprites(void) {
	// This function quickly hides most display elements so that slower operations can make changes.
	// Blacks out the color table and hides all sprites, but does not affect colors set in DLI.
	UInt8 *p = (UInt8*)(PCOLR0);
	UInt8 *spritePositions = (UInt8 *)HPOSP0;
	UInt8 i;
	for (i=0; i<9; ++i) {
		p[i] = 0;
	}
	for (i=0; i<8; ++i) {
		spritePositions[i] = 0;
	}
}

void loadColorTable(const UInt8 *colors) {
	UInt8 *p = (UInt8*)(PCOLR0);
	UInt8 i;
	for (i=0; i<9; ++i) {
		p[i] = colors[i];
	}
}

void setBackgroundGradient(const UInt8 *data) {
	// colors: array of { length, value } tuples, with NULL terminator
	// There should be enough for 73 color values. First one goes into VBI shadow register, and
	// the rest changes the color at the end of each of 72 raster rows.

	UInt8 bgIndex = 0xFF;
	UInt8 dataIndex = 0;
	UInt8 length, value;

	while (length = data[dataIndex++]) {
		value = data[dataIndex++];

		while (length > 0) {
			if (bgIndex == 0xFF) {
				// First color goes into shadow register, which the OS VBI uses.
				POKE(COLOR4, value);
				bgIndex = 0;
			} else {
				BG_COLOR[bgIndex++] = value;
			}
			--length;
		}
	}
}


// Sprites


void setPlayerCursorVisible(UInt8 visible) {
	// Hide sprites while making changes.
	GTIA_WRITE.hposp0 = 0;
	GTIA_WRITE.hposp1 = 0;

	if (visible) { 
		// Clear sprite data.
		clearSprite(1);
		clearSprite(2);

		// Draw cursor sprite, which takes up 2 players because it is 10 pixels wide
		drawSprite(cursorSprite1, 10, 1, 31 + 16);
		drawSprite(cursorSprite2, 10, 2, 31 + 16);

		// Set sprite sizes
		GTIA_WRITE.sizep0 = 0;
		GTIA_WRITE.sizep1 = 0;
		
		// Position sprites		
		GTIA_WRITE.hposp0 = PM_LEFT_MARGIN + (9 * 8);
		GTIA_WRITE.hposp1 = PM_LEFT_MARGIN + (10 * 8);

		// Enable color cycling
		POKE (CUR_TIMER, 15);
	} else {
		// Disable color cycling
		POKE (CUR_TIMER, 0xFF);
	}
}


void setTileOverlaySprite(const UInt8 *sprite, UInt8 column, UInt8 row) {
	// Set horizontal position for tile
	P3_XPOS[row] = PM_LEFT_MARGIN + 8 * column + 4;
	GTIA_WRITE.sizep3 = 0;
	drawSprite(sprite, 8, 4, row * 8 + 16);
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

void setMegaSprite(const UInt8 *sprite, const UInt8 length, const PointU8 *position, UInt8 magnification) {
	// Sprite will be centered on position.
	UInt8 *spritePositions = (UInt8 *)HPOSP0;
	UInt8 *spriteSizes = (UInt8 *)SIZEP0;
	UInt8 *pmbasePtr = (UInt8 *) (256 * spritePage + 384);
	UInt8 spriteIndex = 0;
	UInt8 x = position->x - 10 * magnification + PM_LEFT_MARGIN;
	UInt8 yStart = position->y - length * magnification / 2;
	UInt8 yEnd = yStart + length * magnification;
	UInt8 spriteSizeCode = magnification - 1;
	UInt8 i, y, lineRepeat;

	//printDebugInfo("Sans", (UInt16) spritePositions, 40);

	for (i=0; i<8; ++i) { // Hide sprites.
		spritePositions[i] = 0; 
	}
	
	for (i=0; i<4; ++i) { // Set sprite sizes
		spriteSizes[i] = spriteSizeCode;
	}
	// Special case for SIZEM: all missile sizes are in one byte register
	spriteSizes[4] = (spriteSizeCode << 6) | (spriteSizeCode << 4) | (spriteSizeCode << 2) | spriteSizeCode;

	// Copy sprite data into each stripe
	for (i=0; i<5; ++i) {
		lineRepeat = magnification - 1;
		for (y=0; y<112; ++y) {
			if (yStart <= y && y < yEnd) {
				pmbasePtr[y] = sprite[spriteIndex];
				if (lineRepeat > 0) {
					--lineRepeat;
				} else {
					lineRepeat = magnification - 1;
					++spriteIndex;
				}
				
			} else {
				pmbasePtr[y] = 0;
			}
		}
		pmbasePtr += 128;
	}

	// Position sprites
	spritePositions[0] = x;
	spritePositions[2] = x;
	x += 8 * magnification;
	spritePositions[1] = x;
	spritePositions[3] = x;
	P3_XPOS[0] = x; // VBI still copies P3_XPOS[0] to HPOSP0
	x += 8 * magnification;
	spritePositions[5] = x;
	spritePositions[7] = x;
	x += 2 * magnification;
	spritePositions[4] = x;
	spritePositions[6] = x;
}


void clearSprite(UInt8 player) {
	UInt8 *pmbasePtr = (UInt8 *) (256 * spritePage + 384 + 128 * player);
	UInt8 i;

	for (i=0; i<128; ++i) {
		pmbasePtr[i] = 0;
	}
}

