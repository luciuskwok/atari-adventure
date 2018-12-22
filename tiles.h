// tiles.h

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
#define TownGrayColor  (0x00)
#define TownRedColor   (0x40)
#define TownGreenColor (0x80)
#define TownBlueColor  (0xC0)

// Tile constants
// Add 0x40 to these values to get the character value.
// This puts them in the graphics/control character range in ATASCII.

// Map Tiles
enum TileValues {
	tSolid = 1,

	// Overworld
	tPlains,
	tBridgeH,
	tBridgeV,
	tForest, // also used in town
	tShallows,
	tWater,
	tMountain,

	// Dungeon/Town
	tBrick,
	tFloor,
	tLadder,
	tChest,
	tHouse1,
	tHouse2,
	tHouse3,
	tHouse4,

	// Tiles with sprite overlays
	tCastle = 24,
	tTown,
	tVillage,
	tMonument,
	tCave, 
	tHouseDoor,
};

// Text window
#define tPotion (0x3B)
#define tFang (0x3D)

// Bitmap Data
extern const UInt8 tileBitmaps[];
extern const UInt8 cursorSprite1[];
extern const UInt8 cursorSprite2[];
extern const UInt8 tileSprites[];
extern const UInt8 sansMegaSprite[];
extern const UInt8 sansMegaSpriteLength;

