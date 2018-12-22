// map.h

#include "types.h"


extern const UInt8 *currentRunLenMap;
extern const UInt8 *currentTileMap;
extern SizeU8 currentMapSize;
extern UInt8 currentMapType;


UInt8 mapTileAt(PointU8 *pt);
PointU8 mapEntryPoint(UInt8 mapType);
void loadMap(UInt8 mapType, UInt8 sightDistance, PointU8 *location);
void layoutCurrentMap(UInt8 sightDistance);
void drawCurrentMap(PointU8 *center);
void decodeRunLenRange(UInt8 *outData, UInt8 start, UInt8 end, const UInt8 *runLenData);


