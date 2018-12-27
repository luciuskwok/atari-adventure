// sprites.c

#include "sprites.h"
#include "atari_memmap.h"
#include "graphics.h"
#include "image_data.h"
#include <atari.h>


// Globals
UInt8 spritePage; // shadow of ANTIC hardware register, which cannot be read



void initSprites(UInt8 page) {
	UInt8 *pmbasePtr = (UInt8 *) (page * 256 + 384);
	UInt16 i;
	UInt8 c;

	spritePage = page;
	
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
	//POKE (SDMCTL, 0x2E);
}

// Cursor

void setPlayerCursorVisible(UInt8 visible) {
	// Hide sprites while making changes.
	GTIA_WRITE.hposp0 = 0;
	GTIA_WRITE.hposp1 = 0;

	if (visible) { 
		// Clear sprite data.
		clearSpriteData(1);
		clearSpriteData(2);

		// Draw cursor sprite, which takes up 2 players because it is 10 pixels wide
		drawSprite(cursorSprite1, 10, 1, 31 + PM_TOP_MARGIN);
		drawSprite(cursorSprite2, 10, 2, 31 + PM_TOP_MARGIN);

		// Set sprite sizes
		GTIA_WRITE.sizep0 = 0;
		GTIA_WRITE.sizep1 = 0;
		
		// Position sprites		
		GTIA_WRITE.hposp0 = PM_LEFT_MARGIN + (9 * 8);
		GTIA_WRITE.hposp1 = PM_LEFT_MARGIN + (10 * 8);
	}
}

void setPlayerCursorColorCycling(UInt8 cycle) {
	if (cycle) {
		POKE (CUR_TIMER, 15); // on
	} else {
		POKE (CUR_TIMER, 0xFF); // off
	}
}

// Drawing

void setTileOverlaySprite(const UInt8 *sprite, UInt8 column, UInt8 row) {
	// Set horizontal position for tile
	P3_XPOS[row] = PM_LEFT_MARGIN + 8 * column + 4;
	GTIA_WRITE.sizep3 = 0;
	drawSprite(sprite, 8, 4, row * 8 + PM_TOP_MARGIN);
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

void setSpriteHorizontalPosition(UInt8 player, UInt8 x) {
	UInt8 *spritePositions = (UInt8 *)HPOSP0;

	if (player > 0 && player < 9) {
		player -= 1;
		spritePositions[player] = x;
		if (player == 3) {
			P3_XPOS[0] = x; // VBI still copies P3_XPOS[0] to HPOSP0
		}
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

// Clearing

void clearSpriteData(UInt8 player) {
	UInt8 *pmbasePtr = (UInt8 *) (256 * spritePage + 384 + 128 * player);
	UInt8 i;

	for (i=0; i<128; ++i) {
		pmbasePtr[i] = 0;
	}
}

void hideSprites(void) {
	UInt8 *spritePositions = (UInt8 *)HPOSP0;
	UInt8 i;

	for (i=0; i<8; ++i) {
		spritePositions[i] = 0;
	}
	for (i=0; i<9; ++i) {
		P3_XPOS[i] = 0;
	}
}
