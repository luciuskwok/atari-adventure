// tiles.c

#include "tiles.h"


const UInt8 tileBitmaps[] = { 
tCastle   , 0x00, 0x00, 0x22, 0x1C, 0x1C, 0x18, 0x22, 0x00, // Castle bkgnd
tTown     , 0x00, 0x00, 0x28, 0x00, 0x28, 0x00, 0x00, 0x00, // Town bkgnd
tVillage  , 0xFF, 0xD3, 0x89, 0xC9, 0xAC, 0x93, 0xCD, 0xF7, // Village bkgnd
tMonument , 0xFF, 0xFF, 0xFF, 0xE7, 0xE3, 0xF1, 0xF8, 0xFC, // Monument bkgnd
tCave     , 0xFF, 0xFF, 0xFF, 0xE7, 0xE7, 0xE7, 0xFF, 0xFF, // Cave bkgnd
tPlains   , 0xFF, 0xF7, 0xFF, 0xBF, 0xFD, 0xFF, 0xEF, 0xFF, // Plains
tBridgeH  , 0x7E, 0x7E, 0x00, 0x7E, 0x7E, 0x00, 0x7E, 0x7E, // Bridge (east-west)
tBridgeV  , 0x00, 0xDB, 0xDB, 0xDB, 0xDB, 0xDB, 0xDB, 0x00, // Bridge (north-south)
tForest   , 0xFF, 0xF7, 0xE3, 0xF7, 0xC1, 0xF7, 0x80, 0xF7, // Forest
tShallows , 0xFE, 0xFF, 0xFB, 0xFF, 0xEF, 0xFF, 0xBF, 0xFF, // Shallow water
tWater    , 0xCE, 0xB5, 0x5D, 0x73, 0xCE, 0xBA, 0xAD, 0x73, // Deep water
tSolid    , 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // Desert
tMountain , 0xF7, 0xE3, 0xC1, 0x80, 0xFF, 0xBF, 0x1F, 0x0E, // Mountains
tPotion   , 0x3C, 0x18, 0x18, 0x2C, 0x5E, 0x7E, 0x3C, 0x00, // Health Potion
tFang     , 0x04, 0x04, 0x0C, 0x0C, 0x1C, 0x7C, 0x78, 0x30, 
tBrick    , 0xDF, 0xDF, 0xDF, 0x00, 0xFD, 0xFD, 0xFD, 0x00, // Dungeon tiles
tFloor    , 0xFE, 0xFF, 0xBB, 0xFF, 0xEF, 0xFF, 0xBB, 0xFF, 
tLadder   , 0x24, 0x3C, 0x24, 0x3C, 0x24, 0x3C, 0x24, 0x00,
tChest    , 0x3C, 0xFF, 0x00, 0xE7, 0xFF, 0xFF, 0xFF, 0x00,
tHouse1   , 0xC0, 0xDF, 0x9F, 0xBF, 0x3F, 0x7F, 0x7F, 0x00, // Town tiles
tHouse2   , 0x07, 0xE3, 0xEB, 0xC9, 0xDD, 0x9C, 0xBE, 0x00, 
tHouse3   , 0x7F, 0x7F, 0x7F, 0x4C, 0x4C, 0x7F, 0x7F, 0x00, 
tHouse4   , 0x6D, 0x6D, 0x6D, 0x12, 0x6D, 0x6D, 0x6D, 0x00, 
tHouseDoor, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 
0x00 // null terminator
};

// Unused tile bitmaps
// 0x00, 0x00, 0x02, 0x04, 0x76, 0x54, 0x74, 0x00, // of (unused)
// 0x00, 0x04, 0x0E, 0x1F, 0x04, 0x04, 0x04, 0x00, // up arrow (unused)
// tLevel    , 0x00, 0x00, 0x40, 0x40, 0x45, 0x45, 0x72, 0x00, // Lv (Level)
// tHP       , 0x00, 0x00, 0x57, 0x55, 0x77, 0x54, 0x54, 0x00, // HP

const UInt8 cursorSprite1[] = { 0x1F, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F };
const UInt8 cursorSprite2[] = { 0xF8, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0xF8 };
const UInt8 tileSprites[] = { 
0x66, 0x99, 0x81, 0x42, 0x42, 0x81, 0x99, 0x66, // Castle
0xEE, 0x82, 0x82, 0x00, 0x82, 0x82, 0xEE, 0x00, // Town
0x00, 0x2C, 0x60, 0x06, 0x50, 0x44, 0x10, 0x00, // Village
0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, // Monument
0x00, 0x00, 0x18, 0x24, 0x24, 0x24, 0x00, 0x00, // Cave
0x3E, 0x3E, 0x22, 0x22, 0x22, 0x22, 0x22, 0x00, // House Door
};

// == Overworld Map ==
UInt8 overworldTileMap[] = { 
	0,  
	tPlains   | PlainsColor,
	tForest   | ForestColor, 
	tSolid,
	tMountain,
	tShallows | WaterColor,
	tWater    | WaterColor, 
	tBridgeH  | PlainsColor, 
	tBridgeV  | PlainsColor,
	tCastle   | PlainsColor,
	tTown, 
	tVillage  | PlainsColor, 
	tMonument | PlainsColor,
	tCave     | PlainsColor,
};
const UInt8 overworldColorTable[] = {
	0x1C, // 0: yellow / desert
	0x22, // 1: dark brown / plains
	0xC6, // 2: green / forest
	0x84, // 3: blue / water
	0x00, // 4: black / background
};
const UInt8 overworldRleMap[] = {
0x04,0x38,0x41,0x14,
0x06,0x34,0xB0,0x31,0x41,0x15,
0x08,0x32,0x52,0x80,0x55,0x10,0xA0,0x10,
0x08,0x32,0x50,0x32,0x40,0x13,0x50,0x12,
0x0A,0x32,0x50,0x31,0x41,0x10,0xB0,0x11,0x51,0x11,
0x09,0x32,0x50,0x30,0x41,0x15,0x70,0x10,0x60,
0x09,0x32,0x50,0x30,0x40,0xD0,0x15,0x50,0x61,
0x0B,0x30,0xA0,0x30,0x50,0x30,0x40,0x11,0x21,0x13,0x61,
0x09,0x32,0x50,0x30,0x40,0x10,0x23,0x12,0x61,
0x09,0x53,0x41,0x10,0x22,0x11,0xB0,0x10,0x61,
0x06,0x32,0x42,0x23,0x12,0x62,
0x08,0x31,0x43,0x22,0x10,0xC0,0x11,0x62,
0x08,0x30,0x43,0x23,0x11,0x90,0x11,0x61,
0x07,0x30,0x43,0x22,0xA0,0x13,0x62,
0x06,0x30,0x42,0x23,0x11,0x65,
0x06,0x30,0x40,0x25,0x11,0x65,
};
const SizeU8 overworldMapSize = { 16, 16 };
const PointU8 overworldEntryPoint = { 5, 5 };

// == Dungeon Map ==
UInt8 dungeonTileMap[] = { 
	tBrick   | DungeonWallColor, 
	tBrick   | DungeonWallColor, 
	tFloor   | DungeonFloorColor, 
	tLadder  | DungeonExitColor,
	tChest   | DungeonChestColor,
};
const UInt8 dungeonColorTable[] = {
	0x06, // 0: wall: dark grey
	0x90, // 1: floor: dark blue
	0xBA, // 2: exit: light green
	0x1C, // 3: chest: gold
	0x00, // 4: black / background
};
const UInt8 dungeonRleMap[] = {
0x03,0x24,0x0A,
0x07,0x20,0x02,0x20,0x00,0x28,0x00,
0x0B,0x20,0x00,0x22,0x00,0x20,0x04,0x20,0x00,0x20,0x00,
0x10,0x20,0x00,0x21,0x30,0x00,0x20,0x00,0x40,0x20,0x40,0x00,0x20,0x00,0x20,0x00,
0x0B,0x20,0x04,0x20,0x00,0x22,0x00,0x20,0x00,0x20,0x00,
0x0D,0x20,0x00,0x22,0x00,0x20,0x00,0x22,0x00,0x20,0x00,0x20,0x00,
0x0D,0x20,0x00,0x20,0x02,0x20,0x01,0x20,0x01,0x20,0x00,0x20,0x00,
0x0D,0x20,0x00,0x22,0x00,0x20,0x01,0x20,0x01,0x20,0x00,0x20,0x00,
0x0D,0x20,0x02,0x20,0x00,0x20,0x01,0x20,0x01,0x20,0x00,0x20,0x00,
0x0B,0x20,0x00,0x22,0x00,0x20,0x01,0x23,0x00,0x20,0x00,
0x07,0x20,0x00,0x20,0x02,0x20,0x08,
0x04,0x20,0x00,0x2D,
0x06,0x20,0x00,0x20,0x0B,0x20,
0x06,0x20,0x00,0x22,0x00,0x29,
0x04,0x20,0x0D,0x20,
0x03,0x2F,0x00,
};
const SizeU8 dungeonMapSize = { 16, 16 };
const PointU8 dungeonEntryPoint = { 4, 3 };


// == Town Map ==
UInt8 townTileMap[] = { 
	0,
	tSolid    | TownRedColor, // pavement, passable
	tForest   | TownGreenColor, // tree
	tSolid    | TownGreenColor, // grass, passable
	tBrick    | TownGrayColor, // wall
	tHouse1   | TownRedColor,
	tHouse2   | TownRedColor,
	tHouse1   | TownGreenColor, 
	tHouse2   | TownGreenColor,
	tHouse1   | TownBlueColor,
	tHouse2   | TownBlueColor,
	tHouse3   | TownGrayColor, 
	tHouse4   | TownGrayColor, 
	tHouseDoor| TownGrayColor, // 13, passable
};
const UInt8 townColorTable[] = {
	0x0A, // 0: gray
	0x32, // 1: dark red
	0xC6, // 2: green
	0x74, // 3: blue
	0x00, // 4: black / background
};
const UInt8 townRleMap[] = {
0x04,0x46,0x11,0x46,
0x0A,0x40,0x21,0x10,0x22,0x11,0x22,0x10,0x21,0x40,
0x0C,0x40,0x20,0x30,0x10,0x32,0x11,0x32,0x10,0x30,0x20,0x40,
0x04,0x40,0x1D,0x40,
0x10,0x40,0x20,0x30,0x10,0x20,0x50,0x60,0x11,0x20,0x30,0x20,0x10,0x30,0x20,0x40,
0x10,0x40,0x20,0x30,0x10,0x30,0xC0,0xD0,0x11,0x30,0x70,0x80,0x10,0x30,0x20,0x40,
0x0F,0x40,0x20,0x30,0x10,0x20,0x31,0x11,0x30,0xB0,0xD0,0x10,0x30,0x20,0x40,
0x03,0x1F,0x00,
0x03,0x1F,0x00,
0x0F,0x40,0x20,0x30,0x10,0x50,0x60,0x30,0x11,0x31,0x20,0x10,0x30,0x20,0x40,
0x10,0x40,0x20,0x30,0x10,0xB0,0xD0,0x30,0x11,0x90,0xA0,0x30,0x10,0x30,0x20,0x40,
0x0F,0x40,0x20,0x30,0x10,0x20,0x31,0x11,0xC0,0xD0,0x30,0x10,0x30,0x20,0x40,
0x04,0x40,0x1D,0x40,
0x0C,0x40,0x20,0x30,0x10,0x32,0x11,0x32,0x10,0x30,0x20,0x40,
0x0C,0x40,0x21,0x10,0x20,0x31,0x11,0x31,0x20,0x10,0x21,0x40,
0x04,0x46,0x11,0x46,
};
const SizeU8 townMapSize = { 16, 16 };
const PointU8 townEntryPoint = { 7, 7 };


/*
Run-length encoded map format:
For each row, the first byte indicates length of the row, not including length byte.
High nybble indicates the tileChar to use.
Low nybble stores the number of repeated tiles in addition to the first one.
If low nybble is 0xF, the next byte contains an 8-bit value to add to the repeat.
*/



