// graphics.c

#include <atari.h>
#include "atari_memmap.h"
#include "graphics.h"


extern void __fastcall__ initVBI(void *addr);
extern void __fastcall__ immediateUserVBI(void);
extern void __fastcall__ displayListInterrupt(void);

// Tile constants
#define tD (0x3F)
#define tM (0x3E)
#define tP (0x7C)
#define tB (0x47)
#define tH (0x48)
#define tF (0xA0)
#define tS (0xFB)
#define tW (0xFD)
#define tC (0x41)
#define tT (0x02)
#define tV (0x43)
#define tX (0x05)
#define tY (0x06)
#define tLV (0x1B) /*';'*/
#define tHP (0x1D) /*'='*/
#define tPotion (0x40)
#define tFang (0x41)

// Binary Data
unsigned char tileBitmaps[] = { 
tD & 0x3F,  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // Desert
tM & 0x3F,  0xF7, 0xE3, 0xC1, 0x80, 0xFF, 0xBF, 0x1F, 0x0E, // Mountains
tP & 0x3F,  0xFF, 0xF7, 0xFF, 0xBF, 0xFD, 0xFF, 0xEF, 0xFF, // Plains
tB & 0x3F,  0x7E, 0x7E, 0x00, 0x7E, 0x7E, 0x00, 0x7E, 0x7E, // Bridge (east-west)
tH & 0x3F,  0x00, 0xDB, 0xDB, 0xDB, 0xDB, 0xDB, 0xDB, 0x00, // Bridge (north-south)
tF & 0x3F,  0xFF, 0xF7, 0xE3, 0xF7, 0xC1, 0xF7, 0x80, 0xF7, // Forest
tS & 0x3F,  0xFE, 0xFF, 0xFB, 0xFF, 0xEF, 0xFF, 0xBF, 0xFF, // Shallow water
tW & 0x3F,  0xCE, 0xB5, 0x5D, 0x73, 0xCE, 0xBA, 0xAD, 0x73, // Deep water
tC & 0x3F,  0x00, 0x00, 0x22, 0x1C, 0x1C, 0x18, 0x22, 0x00, // Castle bkgnd
tT & 0x3F,  0x00, 0x00, 0x28, 0x00, 0x28, 0x00, 0x00, 0x00, // Town bkgnd
tV & 0x3F,  0xFF, 0xD3, 0x89, 0xC9, 0xAC, 0x93, 0xCD, 0xF7, // Village bkgnd
tX & 0x3F,  0xFF, 0xFF, 0xFF, 0xE7, 0xE3, 0xF1, 0xF8, 0xFC, // Monument bkgnd
tY & 0x3F,  0xFF, 0xFF, 0xFF, 0xE7, 0xE7, 0xE7, 0xFF, 0xFF, // Cave bkgnd
tLV, 0x00, 0x00, 0x40, 0x40, 0x45, 0x45, 0x72, 0x00, // Lv (Level)
tHP, 0x00, 0x00, 0x57, 0x55, 0x77, 0x54, 0x54, 0x00, // HP
tPotion, 0x3C, 0x18, 0x18, 0x2C, 0x5E, 0x7E, 0x3C, 0x00, // Health Potion
tFang, 0x04, 0x04, 0x0C, 0x0C, 0x1C, 0x7C, 0x78, 0x30, 
};
#define tileCount (17)

// Unused tile bitmaps
// 0x00, 0x00, 0x02, 0x04, 0x76, 0x54, 0x74, 0x00, // of (unused)
// 0x00, 0x04, 0x0E, 0x1F, 0x04, 0x04, 0x04, 0x00, // up arrow (unused)


unsigned char sampleMap[] = {
tD, tD, tF, tF, tW, tW, tS, tF, tD, tD, tD,
tD, tD, tF, tF, tW, tW, tS, tF, tD, tX, tP,
tD, tD, tD, tF, tF, tS, tS, tF, tF, tP, tP,
tD, tY, tM, tM, tF, tF, tF, tF, tF, tP, tP,
tD, tM, tM, tM, tM, tF, tF, tP, tP, tV, tP,
tM, tM, tM, tM, tF, tF, tP, tP, tP, tP, tP,
tM, tM, tM, tM, tF, tF, tP, tP, tT, tP, tP,
tM, tM, tM, tF, tF, tF, tF, tF, tP, tP, tP,
tM, tF, tF, tS, tF, tF, tF, tF, tP, tP, tP,
tF, tF, tF, tS, tF, tF, tF, tF, tP, tC, tP,
tF, tF, tF, tS, tS, tF, tF, tF, tP, tP, tP,
};

unsigned char cursorSprite1[] = { 0x1F, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F };
unsigned char cursorSprite2[] = { 0xF8, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0xF8 };

unsigned char castleSprite[] = { 0x66, 0x99, 0x81, 0x42, 0x42, 0x81, 0x99, 0x66 };
unsigned char townSprite[] = { 0xEE, 0x82, 0x82, 0x00, 0x82, 0x82, 0xEE, 0x00 };
unsigned char villageSprite[] = { 0x00, 0x2C, 0x60, 0x06, 0x50, 0x44, 0x10, 0x00 };
unsigned char monumentSprite[] = { 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00 };
unsigned char caveSprite[] = { 0x00, 0x00, 0x18, 0x24, 0x24, 0x24, 0x00, 0x00 };

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
	unsigned char debugStr[5];
	
	for (y = 0; y < 11; ++y) {
		for (x = 0; x < 11; ++x) {
			screen[x + 20 * y] = sampleMap[x + 11 * y];
		}
	}
	
	for (x=0; x<80; ++x) {
		screen[220+x] = x;
	}

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
	
// 	printString("$999,999", 0, 4, 12);
// 	printString("Rep:2010", 0, 4, 14);
// 	printString("21", 0, 14, 12);
// 	printString("1999", 0, 14, 14);

// 	printString("DLI:", 0, 0, 11);
// 	hexString(debugStr, (unsigned int)displayListInterrupt);
// 	printString(debugStr, 0, 4, 11);
// 
// 	printString("Pg6:", 0, 10, 11);
// 	hexString(debugStr, (unsigned int)P2_XPOS);
// 	printString(debugStr, 0, 14, 11);
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




