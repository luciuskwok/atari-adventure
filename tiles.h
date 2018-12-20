// tiles.h

#ifndef TILES_H
#define TILES_H

#include "types.h"


// Color constants
#define DesertColor (0x00)
#define PlainsColor (0x40)
#define ForestColor (0x80)
#define WaterColor  (0xC0)

// Tile constants
#define tCastle (0x3B)
#define tTown (0x3C)
#define tVillage (0x3D)
#define tMonument (0x3E)
#define tCave (0x3F)

#define tPlains (0x03)
#define tDesert (0x05)
#define tMountain (0x06)
#define tForest (0x07)
#define tShallows (0x08)
#define tWater (0x09)
#define tBridgeH (0x0A)
#define tBridgeV (0x0B)

#define tLV (0x1B) /*';'*/
#define tHP (0x1D) /*'='*/

#define tPotion (0x40)
#define tFang (0x41)

// Binary Data
const UInt8 tileBitmaps[];
#define tileCount (17)

const UInt8 sampleMap[];
#define sampleMapWidth (11)
#define sampleMapHeight (11)

const UInt8 cursorSprite1[];
const UInt8 cursorSprite2[];
const UInt8 tileSprites[];

const UInt8 tileChars[];
const UInt8 rleMap[];
#define rleMapWidth (16)
#define rleMapHeight (16)


#endif
