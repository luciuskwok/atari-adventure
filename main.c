// main.c

/* Build commands:
/Applications/Emulation/cc65-master/bin/cl65 -O -t atari -C config.cfg main.c interrupts.s -o adv.xex
open adv.xex
*/

// == About This Program ==
// This program is a rogue-like game. 

// == Notes on Memory Usage ==
// In the config.cfg file, the reserved memory is set to 0x0820, giving us 2KB of space below the display area, which itself takes 1KB. The space between APPMHI and MEMTOP should be ours to use. 
// Calculate 3KB from RAMTOP, which is 12*256 bytes.
// RAMTOP: 0xC0 without BASIC, 0xA0 with BASIC. Below are values with BASIC.
// 0x9400: Custom character set needs 128 chars * 8 bytes = 1024 bytes.
// 0x9800: PMGraphics needs 640 bytes (double-line sprites), but the data area doesn't start until 384 bytes after PMBase, which must be on 1KB boundary.
// 0x9C00: Display list and screen memory has 1024 bytes allocated, so that the graphics driver can do its thing. We overwrite this area with our own display list, which takes up less space than the standard text screen.


// Includes
#include <atari.h>
#include <conio.h>
#include "atari_memmap.h"


// Globals
unsigned char gQuit;

// Function prototypes
void initGraphics(void);
void initAnticMode7(void);
void initFont(unsigned char fontPage);
void initSprites(unsigned char spritePage);
void fillScreen(void);
void printString(const char* s, unsigned char color, unsigned char x, unsigned char y);


// Binary Data
unsigned char tileFont[] = { 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xF7, 0xFF, 0xBF, 0xFD, 0xFF, 0xEF, 0xFF,
0xFF, 0xF7, 0xE3, 0xF7, 0xC1, 0xF7, 0x80, 0xF7, 
0xF7, 0xE3, 0xC1, 0x80, 0xFF, 0xBF, 0x1F, 0x0E,  
0xFE, 0xFF, 0xFB, 0xFF, 0xEF, 0xFF, 0xBF, 0xFF, 
0xCE, 0xB5, 0x5D, 0x73, 0xCE, 0xBA, 0xAD, 0x73, 
0x00, 0x00, 0x40, 0x40, 0x45, 0x45, 0x72, 0x00, 
0x00, 0x00, 0x57, 0x55, 0x77, 0x54, 0x54, 0x00, 
0x00, 0x00, 0x02, 0x04, 0x76, 0x54, 0x74, 0x00, 
0x00, 0x04, 0x0E, 0x1F, 0x04, 0x04, 0x04, 0x00, 
0x3C, 0x18, 0x18, 0x2C, 0x5E, 0x7E, 0x3C, 0x00,
};
#define tileFontLength (11*8)

unsigned char sampleMap[] = {
0x01, 0x01, 0x83, 0x83, 0xC5, 0xC5, 0xC5, 0x83, 0x01, 0x01, 0x01,
0x01, 0x01, 0x83, 0x83, 0xC5, 0xC5, 0xC5, 0x83, 0x01, 0x01, 0x42,
0x01, 0x01, 0x01, 0x83, 0x83, 0xC5, 0xC5, 0x83, 0x83, 0x42, 0x42,
0x01, 0x01, 0x04, 0x04, 0x83, 0x83, 0x83, 0x83, 0x83, 0x42, 0x42,
0x01, 0x04, 0x04, 0x04, 0x04, 0x83, 0x83, 0x42, 0x42, 0x42, 0x42,
0x04, 0x04, 0x04, 0x04, 0x83, 0x83, 0x42, 0x42, 0x42, 0x42, 0x42,
0x04, 0x04, 0x04, 0x04, 0x83, 0x83, 0x42, 0x42, 0x42, 0x42, 0x42,
0x04, 0x04, 0x04, 0x83, 0x83, 0x83, 0x83, 0x83, 0x42, 0x42, 0x42,
0x04, 0x83, 0x83, 0xC5, 0x83, 0x83, 0x83, 0x83, 0x42, 0x42, 0x42,
0x83, 0x83, 0x83, 0xC5, 0x83, 0x83, 0x83, 0x83, 0x42, 0x42, 0x42,
0x83, 0x83, 0x83, 0xC5, 0xC5, 0x83, 0x83, 0x83, 0x42, 0x42, 0x42
};

unsigned char cursorSprite[] = {
0x1F, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F, 
0xF8, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0xF8 
};


// == main() == 
int main (void) {
	// Graphics
	initGraphics();
	
	// Put something on the screen
	fillScreen();
	
	// Start new game
	gQuit = 0;
	while (gQuit == 0) {
		//startNewGame();
		//runLoop();
	}

	return 0; // success
}

// == initGraphics() ==
void initGraphics(void) {
	unsigned char ramtopValue = PEEK(RAMTOP);
	
	// == Colors ==
	POKE (PCOLR0, 0x38); // Player 0 color = red
	POKE (PCOLR1, 0x38); // Player 1 color = red
	POKE (PCOLR2, 0x0E); // Player 2 color = white
	POKE (PCOLR3, 0x0E); // Player 3 color = white
	POKE (COLOR0, 0x1E); // Playfield 0 color = yellow / desert
	POKE (COLOR1, 0x22); // Playfield 1 color = dark brown / plains
	POKE (COLOR2, 0xC6); // Playfield 2 color = green / forest
	POKE (COLOR3, 0x84); // Playfield 3 color = blue / water
	POKE (COLOR4, 0x00); // Playfield 4 color / background = black
	
	// == Other ==
	initAnticMode7();
	initSprites(ramtopValue - 8);
	initFont(ramtopValue - 12);
}


// == initAnticMode7() ==
void initAnticMode7(void) {
	// == Display List ==
	// Display list is already set up by the runtime to the equivalent of BASIC's GR.0. 
	// We can just overwrite that DL with our own.
	unsigned char *displayListPtr = (unsigned char *)PEEKW(SDLSTL);
	unsigned char *screenMemPtr = (unsigned char *)PEEKW(SAVMSC);
	unsigned char *writePtr;
	unsigned char i;
	
	writePtr = displayListPtr + 3;
	*writePtr = DL_LMS(DL_CHR20x16x2); // Antic mode 3 + LMS
	*(++writePtr) = (unsigned int)screenMemPtr % 256;
	*(++writePtr) = (unsigned int)screenMemPtr / 256;
	
	for (i=1; i<11; ++i) { // 11 lines of double height tiles = 176 scanlines
		*(++writePtr) = DL_CHR20x16x2;
	}
	
	*(++writePtr) = DL_BLK2; // 2 blank lines
	for (i=0; i<4; ++i) { // 4 lines of text
		*(++writePtr) = DL_CHR40x8x1; 
	}
	
	*(++writePtr) = DL_JVB; // Vertical blank + jump to beginning of display list
	*(++writePtr) = (unsigned int)displayListPtr % 256;
	*(++writePtr) = (unsigned int)displayListPtr / 256;
}


// == initFont() ==
void initFont(unsigned char fontPage) {
	unsigned char *customCharSetPtr = (unsigned char *) ((unsigned int)fontPage * 256);
	const unsigned char *romCharSetPtr = (unsigned char *)0xE000;
	unsigned char *ptr;
	unsigned int index;
	
	// Copy char set from ROM to RAM, 128 characters.
	for (index=0; index<1024; ++index) {
		customCharSetPtr[index] = romCharSetPtr[index];
	}
	
	// Add our custom tiles.
	ptr = customCharSetPtr + (1*8);
	for (index=0; index<tileFontLength; ++index) {
		ptr[index] = tileFont[index];
	}

	// Switch to custom character set.
	POKE(CHBAS, fontPage);
}

// == initSprites() ==
void initSprites(unsigned char spritePage) {
	unsigned char *pmbasePtr = (unsigned char *) ((unsigned int)spritePage * 256 + 384);
	unsigned int i;
	const unsigned int spriteLength = 128;
	const unsigned char leftMargin = 48;
	const unsigned int cursorSpriteTop = 16 + 5 * 8 - 1; // 16 for overscan area
	
	// Zero out memory
	for (i=0; i<640; ++i) {
		pmbasePtr[i] = 0;
	}
	
	// Put test pattern into sprite data area
	for (i=0; i<10; ++i) {
		pmbasePtr[i + cursorSpriteTop + spriteLength * 1] = cursorSprite[i]; // Player 0
		pmbasePtr[i + cursorSpriteTop + spriteLength * 2] = cursorSprite[i + 10];
	}
	
	// Position and size sprites
	GTIA_WRITE.hposm3 = 0;
	GTIA_WRITE.hposm2 = 0;
	GTIA_WRITE.hposm1 = 0;
	GTIA_WRITE.hposm0 = 0;
	GTIA_WRITE.hposp0 = leftMargin + 36;
	GTIA_WRITE.hposp1 = leftMargin + 44;
	GTIA_WRITE.hposp2 = 0;
	GTIA_WRITE.hposp3 = 0;
	GTIA_WRITE.sizep0 = 0; // normal width
	GTIA_WRITE.sizep1 = 0;
	GTIA_WRITE.sizep2 = 0;
	GTIA_WRITE.sizep3 = 0;
	GTIA_WRITE.sizem = 0;

	// Set up ANTIC
	ANTIC.pmbase = spritePage;
	GTIA_WRITE.gractl = 3; // enable both missile and player graphics
	POKE (SDMCTL, 0x2E); // standard playfield + missile DMA + player DMA + display list DMA
	POKE (GPRIOR, 0x01); // layer players above playfield.
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
	
	for (x=0; x<127; ++x) {
		//screen[220+x] = x;
	}
	
	printString("ALISA", 0, 12, 0);
	printString("g99h12", 0, 12, 1);

	printString("MARIE", 2, 12, 3);
	printString("g1 h2", 2, 12, 4);
	
	printString("GUY", 0, 12, 6);
	printString("g99h12", 0, 12, 7);

	printString("NYORN", 2, 12, 9);
	printString("g1 h2", 2, 12, 10);
	
	printString("$999,999", 0, 0, 15);
	
}

// == printString() ==
void printString(const char* s, unsigned char color, unsigned char x, unsigned char y) {
	unsigned char *screen = (unsigned char *)PEEKW(SAVMSC);
	unsigned char index = 0;
	char c;
	
	while (c = s[index]) {
		c = (c - 0x20) + (color * 64);
		screen[(unsigned int)x + 20 * (unsigned int)y] = c;
		++x;
		++index;
	}
}

