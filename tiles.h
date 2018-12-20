// tiles.h

#ifndef TILES_H
#define TILES_H

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

#endif
