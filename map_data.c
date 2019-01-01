// map_data.c

#include "map_data.h"


// Color constants
#define WaterColor  (0x00)
#define DesertColor (0x40)
#define PlainsColor (0x80)
#define ForestColor (0xC0)
#define DungeonFloorColor (0x00)
#define DungeonChestColor (0x40)
#define DungeonWallColor  (0x80)
#define DungeonExitColor  (0xC0)
#define TownBlueColor  (0x00)
#define TownGrayColor  (0x40)
#define TownRedColor   (0x80)
#define TownGreenColor (0xC0)

// Tile Data

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

/* Unused tile bitmaps
	0x00, 0x00, 0x02, 0x04, 0x76, 0x54, 0x74, 0x00, // of (unused)
	0x00, 0x04, 0x0E, 0x1F, 0x04, 0x04, 0x04, 0x00, // up arrow (unused)
	tLevel    , 0x00, 0x00, 0x40, 0x40, 0x45, 0x45, 0x72, 0x00, // Lv (Level)
	tHP       , 0x00, 0x00, 0x57, 0x55, 0x77, 0x54, 0x54, 0x00, // HP
*/


UInt8 overworldTileMap[] = { 
	0,  
	tPlains   | PlainsColor,
	tForest   | ForestColor, 
	tSolid    | DesertColor,
	tMountain | DesertColor,
	tShallows | WaterColor,
	tWater    | WaterColor, 
	tBridgeH  | PlainsColor, 
	tBridgeV  | PlainsColor,
	tCastle   | PlainsColor,
	tTown 	  | DesertColor, 
	tVillage  | PlainsColor, 
	tMonument | PlainsColor,
	tCave     | PlainsColor,
};
const UInt8 overworldColorTable[] = {
	0x58, // PM0: purple / cursor color
	0x58, // PM1: purple / cursor color
	0x0F, // PM2: white / unused
	0x0F, // PM3: white / tile overlay
	0x82, // 0: blue / water
	0x1C, // 1: yellow / desert
	0x24, // 2: brown / plains
	0xB6, // 3: dark green / forest
	0x00, // 4: black / background
	0x0E, 0x04, 0x82 // text box
};
const UInt8 overworldRleMap[] = {
0x0F,0x4D,0x34,0x14,0x2F,0x05,0x11,0x53,0x10,0x23,0x41,0x10,0x50,0x10,0x22,
0x11,0x41,0x10,0x54,0x45,0x34,0x15,0x2F,0x01,0x14,0x53,0x10,0x24,0x11,0x50,0x10,0x22,
0x0D,0x41,0x57,0x43,0x33,0x17,0x2D,0x16,0x53,0x17,0x70,0x11,0x21,
0x14,0x40,0x10,0x53,0x11,0x51,0x43,0x33,0x12,0x50,0x13,0x2C,0x17,0x53,0x12,0xA0,0x13,0x50,0x11,0x21,
0x16,0x41,0x10,0x52,0x10,0xB0,0x10,0x51,0x42,0x33,0x12,0x50,0x14,0x29,0x15,0x50,0x15,0x50,0x17,0x50,0x12,0x20,
0x14,0x41,0x10,0x52,0x11,0x52,0x42,0x33,0x12,0x50,0x14,0x29,0x15,0x50,0x15,0x51,0x15,0x51,0x12,0x20,
0x16,0x41,0x10,0x57,0x42,0x33,0x13,0x50,0x12,0x29,0x11,0x40,0x13,0x50,0x15,0x51,0x14,0x51,0x11,0xB0,0x10,0x20,
0x15,0x41,0x11,0x56,0x42,0x33,0x13,0x51,0x11,0x26,0x13,0x41,0x13,0x50,0x12,0xD0,0x12,0x50,0x11,0x53,0x15,
0x16,0x30,0x42,0x55,0x10,0x42,0x32,0x15,0x50,0x11,0x24,0x14,0x42,0x13,0x50,0x11,0x21,0x12,0x51,0x80,0x53,0x15,
0x15,0x30,0x44,0x10,0x50,0x45,0x32,0x15,0x50,0x11,0x23,0x14,0x42,0x14,0x50,0x11,0x21,0x16,0x54,0x80,0x52,
0x14,0x32,0x41,0x21,0x50,0x44,0x32,0x15,0x50,0x12,0x23,0x11,0xD0,0x11,0x42,0x14,0x50,0x11,0x22,0x1E,
0x13,0x33,0x22,0x50,0x20,0x10,0x41,0x33,0x15,0x50,0x13,0x21,0x14,0x42,0x14,0x50,0x12,0x22,0x1E,
0x11,0x33,0x22,0x50,0x20,0x41,0x33,0x17,0x50,0x1F,0x02,0x50,0x12,0x24,0x10,0x20,0x1A,
0x10,0x33,0x22,0x50,0x20,0x41,0x33,0x17,0x50,0x1F,0x02,0x50,0x12,0x2A,0x15,0x50,
0x14,0x33,0x10,0x21,0x50,0x42,0x33,0x17,0x50,0x1F,0x02,0x50,0x13,0x25,0x40,0xC0,0x11,0x20,0x13,0x51,
0x12,0x34,0x21,0x43,0x32,0x18,0x50,0x17,0x22,0x16,0x50,0x12,0x25,0x41,0x11,0x21,0x12,0x52,
0x13,0x33,0x10,0x21,0x42,0x33,0x17,0x51,0x16,0x23,0x16,0x50,0x12,0x25,0x41,0x11,0x21,0x12,0x52,
0x14,0x33,0x22,0x42,0x33,0x17,0x50,0x17,0x23,0x11,0x40,0x13,0x50,0x13,0x24,0x41,0x10,0x23,0x11,0x52,
0x14,0x32,0x21,0x43,0x33,0x18,0x50,0x17,0x23,0x11,0x40,0x13,0x50,0x13,0x24,0x41,0x10,0x23,0x11,0x52,
0x14,0x32,0x20,0x43,0x33,0x19,0x50,0x17,0x23,0x10,0x42,0x12,0x50,0x14,0x23,0x41,0x10,0x23,0x11,0x52,
0x16,0x31,0x21,0x43,0x33,0x19,0x50,0x17,0x22,0x11,0x42,0x12,0x51,0x12,0xC0,0x10,0x22,0x40,0x12,0x22,0x11,0x52,
0x12,0x31,0x20,0x30,0x42,0x34,0x19,0x50,0x16,0x22,0x12,0x42,0x13,0x50,0x16,0x41,0x18,0x51,
0x12,0x33,0x42,0x34,0x19,0x50,0x16,0x22,0x12,0x42,0x13,0x50,0x15,0x42,0x11,0x50,0x15,0x51,
0x10,0x33,0x41,0x36,0x18,0x50,0x1C,0x40,0x15,0x50,0x15,0x41,0x12,0x50,0x15,0x51,
0x0F,0x32,0x42,0x36,0x18,0x50,0x1F,0x04,0x50,0x15,0x41,0x12,0x50,0x15,0x51,
0x10,0x32,0x42,0x36,0x18,0x50,0x1F,0x04,0x70,0xA0,0x13,0x42,0x12,0x50,0x15,0x51,
0x0E,0x32,0x41,0x37,0x18,0x50,0x18,0x5B,0x14,0x42,0x13,0x50,0x14,0x51,
0x10,0x32,0x41,0x37,0x18,0x50,0x18,0x52,0x17,0x50,0x14,0x41,0x14,0x50,0x14,0x51,
0x10,0x32,0x41,0x38,0x17,0x50,0x16,0x54,0x17,0x50,0x14,0x41,0x14,0x50,0x14,0x51,
0x14,0x31,0x42,0x33,0x10,0x33,0x13,0x90,0x12,0x52,0x14,0x53,0x18,0x50,0x14,0x41,0x15,0x50,0x13,0x51,
0x10,0x31,0x41,0x37,0x1B,0x51,0x80,0x50,0x11,0x53,0x18,0x50,0x1C,0x50,0x13,0x51,
0x0E,0x31,0x41,0x37,0x1E,0x56,0x18,0x50,0x18,0xD0,0x12,0x51,0x11,0x52,
0x0C,0x32,0x40,0x36,0x50,0x1F,0x0F,0x50,0x1D,0x50,0x11,0x52,
0x0F,0x32,0x40,0x32,0x13,0x50,0x1F,0x0A,0x41,0x12,0x51,0x1C,0x50,0x10,0x53,
0x0D,0x32,0x40,0x33,0x12,0x50,0x1F,0x0A,0x41,0x12,0x51,0x1C,0x55,
0x11,0x32,0x40,0x33,0x12,0x50,0x17,0x50,0x1F,0x00,0x42,0x12,0x57,0x80,0x53,0x10,0x56,
0x12,0x32,0x41,0x10,0x30,0x13,0x50,0x17,0x50,0x1F,0x00,0x40,0x10,0x40,0x1E,0x53,0x11,0x52,
0x0D,0x32,0x41,0x15,0x50,0x17,0x50,0x1F,0x00,0x42,0x1F,0x05,0x52,
0x0D,0x32,0x41,0x15,0x50,0x17,0x50,0x1F,0x00,0x41,0x1F,0x06,0x52,
0x0F,0x33,0x40,0x15,0x50,0x18,0x50,0x15,0x53,0x1B,0x22,0x12,0x23,0x16,0x52,
0x0F,0x33,0x40,0x15,0x50,0x19,0x50,0x17,0x52,0x19,0x23,0x10,0x26,0x14,0x52,
0x0D,0x33,0x41,0x14,0x50,0x1A,0x50,0x18,0x52,0x17,0x2B,0x15,0x51,
0x0F,0x34,0x40,0x14,0x50,0x1A,0x50,0x1A,0x52,0x16,0x24,0x13,0x21,0x15,0x51,
0x11,0x34,0x40,0x14,0x50,0x1A,0x50,0x1C,0x51,0x17,0x23,0x10,0x90,0x10,0x21,0x15,0x51,
0x0F,0x34,0x41,0x13,0x51,0x19,0x50,0x1D,0x51,0x16,0x24,0x11,0x21,0x15,0x51,
0x0D,0x35,0x41,0x13,0x50,0x19,0x50,0x1E,0x50,0x16,0x28,0x15,0x51,
0x0D,0x35,0x41,0x13,0x5B,0x1E,0x50,0x17,0x27,0x13,0xA0,0x10,0x61,
0x0B,0x36,0x40,0x1E,0x50,0x1E,0x51,0x17,0x27,0x14,0x61,
0x0C,0x37,0x40,0x1D,0x50,0x1F,0x00,0x51,0x16,0x27,0x14,0x61,
0x0C,0x37,0x40,0x1D,0x50,0x1F,0x01,0x51,0x15,0x27,0x14,0x61,
0x0C,0x38,0x1D,0x50,0x18,0xA0,0x16,0x50,0x18,0x26,0x13,0x61,
0x0B,0x38,0x1D,0x50,0x1F,0x01,0x50,0x1D,0x21,0x13,0x61,
0x0C,0x39,0x1C,0x50,0x11,0x69,0x14,0x50,0x1D,0x21,0x13,0x61,
0x0B,0x3A,0x1A,0x6F,0x0A,0x15,0xC0,0x10,0x21,0x13,0x61,
0x07,0x3A,0x1A,0x6F,0x0C,0x1B,0x61,
0x07,0x3B,0x18,0x6F,0x10,0x18,0x61,
0x08,0x3B,0x18,0x68,0x19,0x6D,0x16,0x62,
0x08,0x3B,0x18,0x67,0x1D,0x6B,0x15,0x62,
0x09,0x3C,0x16,0x66,0x1F,0x04,0x67,0x14,0x63,
0x0A,0x3C,0x16,0x64,0x18,0xA0,0x1D,0x66,0x13,0x63,
0x09,0x3D,0x14,0x64,0x1F,0x0B,0x65,0x12,0x63,
0x0C,0x3E,0x13,0x64,0x16,0x24,0x13,0xB0,0x1A,0x65,0x11,0x63,
0x0A,0x3E,0x13,0x64,0x15,0x28,0x1C,0x65,0x10,0x64,
0x09,0x3F,0x00,0x12,0x64,0x15,0x2C,0x19,0x6A,
};
const SizeU8 overworldMapSize = { 64, 64 };
//const PointU8 overworldEntryPoint = { 47, 7 }; // At cave
const PointU8 overworldEntryPoint = { 60, 6 }; // At village


UInt8 dungeonTileMap[] = { 
	tBrick   | DungeonWallColor, 
	tBrick   | DungeonWallColor, 
	tFloor   | DungeonFloorColor, 
	tLadder  | DungeonExitColor,
	tChest   | DungeonChestColor,
};
const UInt8 dungeonColorTable[] = {
	0x58, // PM0: purple / cursor color
	0x58, // PM1: purple / cursor color
	0x0F, // PM2: white / unused
	0x0F, // PM3: white / tile overlay
	0x90, // 0: floor: dark blue
	0x1C, // 1: chest: gold
	0x04, // 2: wall: dark grey
	0xBC, // 3: exit: light green
	0x00, // 4: black / background
	0x0A, 0x02, 0x82 // text box
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
	0x58, // PM0: purple / cursor color
	0x58, // PM1: purple / cursor color
	0x0F, // PM2: white / unused
	0x0F, // PM3: white / tile overlay
	0x74, // 0: blue
	0x0A, // 1: gray
	0x32, // 2: dark red
	0xC6, // 3: green
	0x00, // 4: black / background
	0x0E, 0x04, 0x82 // text box
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

