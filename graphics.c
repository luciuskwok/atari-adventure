// graphics.c

#include <atari.h>
#include "atari_memmap.h"
#include "graphics.h"
#include "tiles.h"


extern void __fastcall__ initVBI(void *addr);
extern void __fastcall__ immediateUserVBI(void);
extern void __fastcall__ displayListInterrupt(void);


// Player-Missile Constants
#define PM_LEFT_MARGIN (48)
#define P2_XPOS (0x0604)

// Globals
unsigned char spritePage;


// == initGraphics() ==
void initGraphics(void) {
	unsigned char ramtopValue = PEEK(RAMTOP);
	
	
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
	POKEW (VDSLST, (unsigned int)displayListInterrupt);
	ANTIC.nmien = 0xC0; // enable both DLI and VBI
}


// == initDisplayList() ==
void initDisplayList(void) {
	// == Display List ==
	// Display list is already set up by the runtime to the equivalent of BASIC's GR.0. 
	// We can just overwrite that DL with our own.
	unsigned char *displayListPtr = (unsigned char *)PEEKW(SDLSTL);
	unsigned char *screenMemPtr = (unsigned char *)PEEKW(SAVMSC);
	unsigned char *writePtr;
	unsigned char i;
	
	writePtr = displayListPtr + 3;
	*writePtr = DL_DLI(DL_LMS(DL_CHR20x16x2)); // Antic mode 3 + LMS + DLI
	*(++writePtr) = (unsigned int)screenMemPtr % 256;
	*(++writePtr) = (unsigned int)screenMemPtr / 256;
	
	for (i=1; i<11; ++i) { // 11 lines of double height tiles = 176 scanlines
		*(++writePtr) = DL_DLI(DL_CHR20x16x2); // + DLI on every tile row
	}
	
	*(++writePtr) = DL_BLK4; // 4 blank lines
	*(++writePtr) = DL_CHR40x8x1; // 2 lines of text
	*(++writePtr) = DL_CHR40x8x1; 
	
	*(++writePtr) = DL_JVB; // Vertical blank + jump to beginning of display list
	*(++writePtr) = (unsigned int)displayListPtr % 256;
	*(++writePtr) = (unsigned int)displayListPtr / 256;
}


// == initFont() ==
void initFont(unsigned char fontPage) {
	unsigned char *customFontPtr = (unsigned char *) ((unsigned int)fontPage * 256);
	const unsigned char *romFontPtr = (unsigned char *)0xE000;
	unsigned char *tilePtr;
	unsigned int index;
	unsigned char tileIndex;
	
	// Copy char set from ROM to RAM, 128 characters.
	for (index=0; index<1024; ++index) {
		customFontPtr[index] = romFontPtr[index];
	}
	
	// Add our custom tiles.
	for (tileIndex=0; tileIndex<tileCount; ++tileIndex) {
		// Each tile bitmap has 9 bytes. First byte indicated which character it replaces. Remove the highest 2 bits because they're used for color data.
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
	unsigned char *pmbasePtr = (unsigned char *) ((unsigned int)spritePage * 256 + 384);
	unsigned int i;
	unsigned char c;
	
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
	
	// Draw sprites for special tiles
	drawSpriteTile(monumentSprite, 9, 1);
	drawSpriteTile(caveSprite, 1, 3);
	drawSpriteTile(villageSprite, 9, 4);
	drawSpriteTile(townSprite, 8, 6);
	drawSpriteTile(castleSprite, 9, 9);

	// Set up ANTIC
	ANTIC.pmbase = spritePage;
	GTIA_WRITE.gractl = 3; // enable both missile and player graphics
	POKE (SDMCTL, 0x2E); // standard playfield + missile DMA + player DMA + display list DMA
	POKE (GPRIOR, 0x01); // layer players above playfield.
}

// == clearScreen() ==
void clearScreen(void) {
	unsigned char *screen = (unsigned char *)PEEKW(SAVMSC);
	unsigned int i;
	// Screen is made up of 11 lines * 20 tiles + 2 lines * 40 tiles
	const unsigned int max = 11 * 20 + 2 * 40;
	
	for (i=0; i<max; ++i) {
		screen[i] = 0;
	}
}

// == fillScreen() ==
// Set the screen to show all 256 characters
void fillScreen(void) {
	unsigned char *screen = (unsigned char *)PEEKW(SAVMSC);
	unsigned char x, y;
	
	for (y = 0; y < 11; ++y) {
		for (x = 0; x < 11; ++x) {
			screen[x + 20 * y] = sampleMap[x + 11 * y];
		}
	}
	
// 	for (x=0; x<80; ++x) {
// 		screen[220+x] = x;
// 	}

	// Draw the HP and LV tiles
	for (y=1; y<12; y+=3) {
		screen[20 * y + 12] = tLV | 0x40;
		screen[20 * y + 15] = tHP | 0x40;
	}
	
	// Draw health potion and fang tiles
// 	screen[220 + 1 * 20 + 13] = tPotion;
// 	screen[220 + 3 * 20 + 13] = tFang;
	
	
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

// 	printDebugInfo("DLI:", (unsigned int)displayListInterrupt, 0);

}

// == drawSprite() ==
void drawSprite(unsigned char *sprite, char spriteLength, char player, char y) {
	unsigned char *pmbasePtr = (unsigned char *) (spritePage * 256 + 384);
	const unsigned int pmLength = 128;
	unsigned int offset = y + 16; // overscan area
	unsigned int i;
	
	// Copy sprite data at Y position
	pmbasePtr += pmLength * (player + 1);
	for (i=0; i<spriteLength; ++i) {
		pmbasePtr[i + offset] = sprite[i];
	}
}

// == drawSpriteTile() ==
void drawSpriteTile(unsigned char *sprite, char column, char row) {
	// Set horizontal position for tile
	POKE(P2_XPOS + row, PM_LEFT_MARGIN + 8 * column);
	
	// Draw sprite at vertical position
	drawSprite(sprite, 8, 2, row * 8);
}

// == printString() ==
void printString(const char* s, unsigned char color, unsigned char x, unsigned char y) {
	unsigned char *screen = (unsigned char *)PEEKW(SAVMSC);
	unsigned char index = 0;
	char c;
	
	while (c = s[index]) {
		if (c < 0x20) {
			c += 0x40;
		} else if (c < 0x60) {
			c -= 0x20;
		}
		c += (color * 64);
		screen[(unsigned int)x + 20 * (unsigned int)y] = c;
		++x;
		++index;
	}
}

// == printDebugInfo() ==
void printDebugInfo(const char* label, unsigned int value, unsigned char position) {
	// Prints a label and a hex value in the text box area.
	char hexStr[5];
	char labelLength = strlen(label);
	
	hexString(hexStr, value);
	printString(label, 0, position, 11);
	printString(hexStr, 0, position + labelLength, 11);
}

// == hexString() ==
void hexString(char *s, unsigned int x) {
	char i;
	char c;
	
	for (i=0; i<4; ++i) {
		c = x & 0x0F;
		if (c < 10) {
			c += 0x30;
		} else {
			c += 0x41 - 10;
		}
		s[3-i] = c;
		x = x >> 4;
	}
	s[4] = 0;
}

// == strlen() ==
unsigned char strlen(const char *s) {
	char len = 0;
	while (s[len] != 0 && len < 0xFF) {
		++len;
	}
	return len;
}



