// tiles.h

#ifndef TILES_H
#define TILES_H

#include "types.h"


// Color constants
#define DesertColor (0x00)
#define PlainsColor (0x40)
#define ForestColor (0x80)
#define WaterColor  (0xC0)
#define DungeonWallColor  (0x00)
#define DungeonFloorColor (0x40)
#define DungeonExitColor  (0x80)
#define DungeonChestColor (0xC0)

// Tile constants
// Add 0x40 to these values to get the character value.
// This puts them in the graphics/control character range in ATASCII.

// Overworld map
#define tCastle (1)
#define tTown (2)
#define tVillage (3)
#define tMonument (4)
#define tCave (5)

#define tPlains (6)
#define tBridgeH (7)
#define tBridgeV (8)
#define tForest (9)
#define tShallows (10)
#define tWater (11)
#define tDesert (12)
#define tMountain (13)

// Dungeon maps
#define tBrick (14)
#define tFloor (15)
#define tExit (16)
#define tChest (17)

// Town maps

// Text window
#define tPotion (0x3B)
#define tFang (0x3D)

// Bitmap Data
extern const UInt8 tileBitmaps[];
extern const UInt8 cursorSprite1[];
extern const UInt8 cursorSprite2[];
extern const UInt8 tileSprites[];

// Overworld Map
extern const UInt8 overworldRleMap[];
extern const SizeU8 overworldMapSize;
extern UInt8 overworldTileMap[];
extern const UInt8 overworldColorTable[];

// Dungeon Map
extern const UInt8 dungeonRleMap[];
extern const SizeU8 dungeonMapSize;
extern UInt8 dungeonTileMap[];
extern const UInt8 dungeonColorTable[];


#endif
