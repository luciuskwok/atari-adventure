// graphics.h

#include "types.h"

// Globals
extern UInt8 *textWindow; // Pointer to screen memory for text window


#define P3_XPOS ((UInt8 *)0x0610)
#define BG_COLOR ((UInt8 *)0x0620)



// Init
void initGraphics(void);
void initDisplayList(UInt8 startPage);
void selectDisplayList(UInt8 index);
void initFont(UInt8 fontPage);
void initSprites(void);
void hidePlayfieldAndSprites(void);

// Color Table
void loadColorTable(const UInt8 *colors);
void setBackgroundGradient(const UInt8 *data);

// Sprites
void setPlayerCursorVisible(UInt8 x);
void setTileOverlaySprite(const UInt8 *sprite, UInt8 column, UInt8 row);
void drawSprite(const UInt8 *sprite, UInt8 length, UInt8 player, UInt8 y);
void setMegaSprite(const UInt8 *sprite, const UInt8 length, const PointU8 *position, UInt8 magnification);
void clearSprite(UInt8 player);


