// map.c

#include "graphics.h"
//#include "text.h"
#include "tiles.h"
#include "map_overworld.h"
#include "map_dungeons.h"
#include "map_towns.h"
//#include "atari_memmap.h"
//#include <atari.h>


// Globals
const UInt8 *currentRunLenMap;
const UInt8 *currentTileMap;
SizeU8 currentMapSize;
UInt8 currentMapType;


UInt8 mapTileAt(PointU8 *pt) {
	const UInt8 *runLenPtr = currentRunLenMap;
	UInt8 x = pt->x;
	UInt8 y = pt->y;
	UInt8 tile, i;

	// Skip to row
	for (i=0; i<y; ++i) {
		runLenPtr += runLenPtr[0];
	}

	// Get tile
	decodeRunLenRange(&tile, x, x+1, runLenPtr);

	// Convert to character value
	return currentTileMap[tile];
}


PointU8 mapEntryPoint(UInt8 mapType) {
	PointU8 pt = {0,0};

	switch (mapType) {
	case OverworldMapType: 
		pt = overworldEntryPoint;
		break;
	case DungeonMapType: 
		pt = dungeonEntryPoint;
		break;
	case TownMapType: 
		pt = townEntryPoint;
		break;
	}
	return pt;
}


void loadMap(UInt8 mapType, UInt8 sightDistance, PointU8 *location) {
	const UInt8 *colorTable;

	blackOutColorTable();
	clearMapScreen();
	
	switch (mapType) {
		case OverworldMapType: 
			currentRunLenMap = overworldRleMap;
			currentMapSize = overworldMapSize;
			currentTileMap = overworldTileMap;
			colorTable = overworldColorTable;
			break;
		case DungeonMapType: 
			currentRunLenMap = dungeonRleMap;
			currentMapSize = dungeonMapSize;
			currentTileMap = dungeonTileMap;
			colorTable = dungeonColorTable;
			break;
		case TownMapType: 
			currentRunLenMap = townRleMap;
			currentMapSize = townMapSize;
			currentTileMap = townTileMap;
			colorTable = townColorTable;
			break;
	}
	currentMapType = mapType;

	layoutCurrentMap(sightDistance);
	drawCurrentMap(location);
	loadColorTable(colorTable);
}



