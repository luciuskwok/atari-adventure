// map.h

#ifndef MAP_H
#define MAP_H

#include "types.h"


extern const UInt8 *currentRunLenMap;
extern const UInt8 *currentTileMap;
extern SizeU8 currentMapSize;
extern UInt8 currentMapType;


UInt8 mapTileAt(PointU8 *pt);
PointU8 mapEntryPoint(UInt8 mapType);

void loadMap(UInt8 mapType, UInt8 sightDistance, PointU8 *location);
const UInt8 *colorTableForMap(UInt8 mapType);
void clearMapScreen(void);

void layoutCurrentMap(UInt8 sightDistance);
void drawCurrentMap(PointU8 *center);

// Assembly routine
extern void __fastcall__ decodeRunLenRange(UInt8 *outData, UInt8 start, UInt8 end, const UInt8 *runLenData);

// Constants
enum MapTypes {
	OverworldMapType = 0,
	DungeonMapType = 1,
	TownMapType = 2,
};

#endif
