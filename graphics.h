// graphics.h

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "types.h"

// Globals
extern UInt8 *textWindow; // Pointer to screen memory for text window
extern SizeU8 currentMapSize;
extern UInt8 currentMapType;

// Functions
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
void drawSprite(const UInt8 *sprite, UInt8 spriteLength, UInt8 player, UInt8 y);
void drawSpriteTile(const UInt8 *sprite, UInt8 column, UInt8 row);

// Getting Map Info
UInt8 mapTileAt(PointU8 *pt);

// Player Cursor
void setPlayerCursorVisible(UInt8 hide);


// Constants
enum MapTypes {
	OverworldMapType = 0,
	DungeonMapType = 1,
	TownMapType = 2,
};



#endif
