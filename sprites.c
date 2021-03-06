// sprites.c

#include "sprites.h"
#include "atari_memmap.h"
#include "graphics.h"
#include <atari.h>
#include <string.h>


UInt8 *spriteArea;

// Sprite Data

const DataBlock cursorSprite1 = { 10, { 0x1F, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F } };
const DataBlock cursorSprite2 = { 10, { 0xF8, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0xF8 } };

// Cursor

void setPlayerCursorVisible(UInt8 visible) {
	// Hide sprites while making changes.
	GTIA_WRITE.hposp0 = 0;
	GTIA_WRITE.hposp1 = 0;

	if (visible) { 
		// Clear sprite data.
		clearSprite(1);
		clearSprite(2);

		// Draw cursor sprite, which takes up 2 players because it is 10 pixels wide
		drawSprite(&cursorSprite1, 1, 31 + PM_TOP_MARGIN);
		drawSprite(&cursorSprite2, 2, 31 + PM_TOP_MARGIN);

		// Set sprite sizes
		GTIA_WRITE.sizep0 = 0;
		GTIA_WRITE.sizep1 = 0;
		
		// Position sprites		
		GTIA_WRITE.hposp0 = PM_LEFT_MARGIN + (9 * 8);
		GTIA_WRITE.hposp1 = PM_LEFT_MARGIN + (10 * 8);
	}
}

// Drawing

void drawSprite(const DataBlock *sprite, UInt8 player, UInt8 y) {
	// player: 0=missile, 1-4=p0-p3.
	// this simplifies the math and allows both players and missiles to be addressed
	UInt8 *p = (UInt8 *) (384 + spriteArea + 128 * player + y);
	UInt8 length = sprite->length;
	UInt8 *bytes = sprite->bytes;
	UInt8 i;
	for (i=0; i<length; ++i) {
		p[i] = bytes[i];
	}
}

void fillSprite(UInt8 player, UInt8 value, UInt8 offset, UInt8 length) {
	UInt8 *p = (UInt8 *) (384 + spriteArea + 128 * player + offset);
	memset(p, value, length);
}

void setSpriteOriginX(UInt8 player, UInt8 x) {
	UInt8 *spritePositions = (UInt8 *)HPOSP0;

	if (1 <= player && player <= 8) {
		player -= 1;
		spritePositions[player] = x;
	}
}

void setSpriteWidth(UInt8 player, UInt8 width) {
	UInt8 *spriteSize = (UInt8 *)SIZEP0;
	UInt8 widthCode = width - 1;

	if (1 <= player && player <= 4) {
		spriteSize[player-1] = widthCode;
	} else if (5 <= player && player <= 8) {
		// Changing missile sprite widths not yet supported.
	}
}

// Clearing

void clearSprite(UInt8 player) {
	UInt8 *p = (UInt8 *) (spriteArea + 384 + 128 * player);
	zeroOut8(p, 128);
}

void hideSprites(void) {
	UInt8 *spritePositions = (UInt8 *)HPOSP0;
	UInt8 i;

	for (i=0; i<8; ++i) {
		spritePositions[i] = 0;
	}
	for (i=0; i<dliSpriteDataLength; ++i) {
		dliSpriteData[i] = 0;
	}
}

void initSprites(UInt8 page) {
	UInt16 i;
	UInt8 c;

	spriteArea = (UInt8 *) (page * 256);
	
	// Zero out sprite memory area
	for (i=384; i<1024; ++i) {
		spriteArea[i] = 0;
	}
	
	// Clear GTIA registers
	for (c=0; c<12; ++c) {
		POKE (HPOSP0 + c, 0);
	}
	
	// Set up ANTIC
	ANTIC.pmbase = page;
	GPRIOR_ = 0x11; // layer players above playfield + missiles use COLOR3
	GTIA_WRITE.gractl = 3; // enable both missile and player graphics
}

/*

void setMegaSprite(DataBlock *sprite, const PointU8 *position, UInt8 magnification) {
	// Sprite will be centered on position.
	UInt8 *spritePositions = (UInt8 *)HPOSP0;
	UInt8 *spriteSizes = (UInt8 *)SIZEP0;
	UInt8 *playerArea = spriteArea + 384;
	UInt8 spriteIndex = 0;
	UInt8 x = position->x - 10 * magnification + PM_LEFT_MARGIN;
	UInt8 yStart = position->y - sprite->length * magnification / 2;
	UInt8 yEnd = yStart + sprite->length * magnification;
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
				playerArea[y] = sprite->bytes[spriteIndex];
				if (lineRepeat > 0) {
					--lineRepeat;
				} else {
					lineRepeat = magnification - 1;
					++spriteIndex;
				}
				
			} else {
				playerArea[y] = 0;
			}
		}
		playerArea += 128;
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

const UInt8 sansSpriteOLD[] = {
	0x00, 0x01, 0x07, 0x07, 0x0F, 0x0F, 0x0F, 0x0C, 0x07, 0x05, 0x0C, 0x0E, 0x03, 0x00, 0x10, 0x36, 0x3B, 0x64, 0x7B, 0x79, 0x0B, 0x03, 0x0B, 0x08, 0x10, 0x10, 0x0F, 0x20, 0x7F, 0x7F, 0x00, 0x00, 
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFC, 0x6C, 0xC7, 0xFF, 0x00, 0xAA, 0x01, 0xFE, 0x00, 0xEE, 0x11, 0x7C, 0x7D, 0x39, 0x7D, 0x01, 0x01, 0x00, 0x10, 0x28, 0xEF, 0x00, 0xC7, 0x87, 0x00, 0x00, 
	0x00, 0x00, 0xC0, 0xC0, 0xE0, 0x60, 0x60, 0x60, 0xC0, 0x40, 0x60, 0xE0, 0x80, 0x00, 0x10, 0xD8, 0xB8, 0x4C, 0xBC, 0x3C, 0xA0, 0x80, 0xA0, 0x20, 0x10, 0x10, 0xE0, 0x18, 0x7C, 0xFC, 0x00, 0x00,
};
const SizeU8 sansSpriteSize = { 24, 32 };

const UInt8 sansMegaSpriteLength = 32;
const UInt8 sansMegaSprite[] = {
	0x00, 0x00, 0x80, 0x80, 0x48, 0x48, 0x48, 0x48, 0x80, 0xC0, 0x48, 0x48, 0x80, 0xC0, 0xA4, 0x96, 0x1E, 0xD2, 0x1E, 0x1E, 0x2C, 0x48, 0x80, 0x00, 0x00, 0x80, 0x80, 0x80, 0x48, 0x2C, 0xC0, 0x00, 
	0x00, 0x07, 0x1F, 0x1F, 0x3F, 0x3F, 0x3F, 0x31, 0x1F, 0x17, 0x30, 0x3A, 0x1C, 0x07, 0x40, 0xDB, 0xEC, 0x91, 0xED, 0xF4, 0x6D, 0x2C, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1E, 0x7E, 0x00, 0x00, 
	0x00, 0xFC, 0xFF, 0xFF, 0xFF, 0xF1, 0xF1, 0xB1, 0x1F, 0xFD, 0x01, 0xAB, 0x06, 0xF8, 0x00, 0xBB, 0x46, 0xF1, 0xF6, 0xE5, 0xF6, 0x06, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00, 
	0x07, 0x18, 0x20, 0x20, 0x40, 0x40, 0x40, 0x4E, 0x20, 0x68, 0x4F, 0x45, 0x23, 0x78, 0xBF, 0x24, 0x13, 0x6E, 0x12, 0x0B, 0x92, 0x53, 0x33, 0x3F, 0x3F, 0x7F, 0x7F, 0x3F, 0x61, 0x81, 0x7E, 0x00, 
	0xFC, 0x03, 0x00, 0x00, 0x00, 0x0E, 0x0E, 0x4E, 0xE0, 0x02, 0xFE, 0x54, 0xF9, 0x07, 0xFF, 0x44, 0xB9, 0x0E, 0x09, 0x1A, 0x09, 0xF9, 0xF9, 0xFF, 0xFF, 0xFF, 0xBF, 0x1F, 0x10, 0x10, 0x1F, 0x00,  
};
*/

