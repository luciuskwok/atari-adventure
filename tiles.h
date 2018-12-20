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
unsigned char tileBitmaps[];
#define tileCount (17)

unsigned char sampleMap[];
unsigned char cursorSprite1[];
unsigned char cursorSprite2[];

unsigned char castleSprite[];
unsigned char townSprite[];
unsigned char villageSprite[];
unsigned char monumentSprite[];
unsigned char caveSprite[];

#endif
