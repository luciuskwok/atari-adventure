// graphics.c

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

// Private globals 
UInt8 spritePage; // shadow of ANTIC hardware register, which cannot be read



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
	POKEW (VDSLST, (UInt16)mapViewDLI);
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
		P3_XPOS[i] = 0;
	}
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
	
	// Draw sprite at vertical position
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
	for (i=0; i<9; ++i) {
		P3_XPOS[i] = x;
	}
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


void hideAllSprites(void) {
	UInt8 *spritePositions = (UInt8 *)HPOSP0;
	UInt8 i;
	for (i=0; i<8; ++i) {
		spritePositions[i] = 0;
	}
}


