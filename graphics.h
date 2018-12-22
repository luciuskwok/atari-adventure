// graphics.h

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "types.h"

// Globals
extern UInt8 *textWindow; // Pointer to screen memory for text window
extern SizeU8 currentMapSize;
extern UInt8 currentMapType;

// Init
void initGraphics(void);
void initDisplayList(void);
void initFont(UInt8 fontPage);
void initSprites(void);
void clearMapScreen(void);
void loadMap(UInt8 mapType, UInt8 variation);

// Color Table
void blackOutColorTable(void);
void loadColorTableForCurrentMap(void);
void loadColorTable(const UInt8 *colors);
void setBackgroundGradient(const UInt8 *colors);

// Map Drawing
void layoutCurrentMap(UInt8 sightDistance);
void drawCurrentMap(PointU8 *center);
void decodeRunLenRange(UInt8 *outData, UInt8 start, UInt8 end, const UInt8 *runLenData);

// Getting Map Info
UInt8 mapTileAt(PointU8 *pt);
PointU8 mapEntryPoint(UInt8 mapType);

// Sprites
void setPlayerCursorVisible(UInt8 x);
void setTileOverlaySprite(const UInt8 *sprite, UInt8 column, UInt8 row);
void drawSprite(const UInt8 *sprite, UInt8 length, UInt8 player, UInt8 y);
void setMultiSprite(const UInt8 *sprite, const SizeU8 *spriteSize, const PointU8 *position);
void clearSprite(UInt8 player);


// Constants
enum MapTypes {
	OverworldMapType = 0,
	DungeonMapType = 1,
	TownMapType = 2,
};



#endif
