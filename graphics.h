// graphics.h

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "types.h"

// Globals
extern UInt8 *textWindow; // Pointer to screen memory for text window


// VBI and DLI registers
#define CUR_TIMER ((UInt8 *)0x0600)
#define VB_TIMER ((UInt8 *)0x0601)
#define TEXT_LUM ((UInt8 *)0x0611)
#define TEXT_BG ((UInt8 *)0x0612)
#define P3_XPOS ((UInt8 *)0x0613)
#define BG_COLOR ((UInt8 *)0x0620)


// Init
void initGraphics(void);
void initDisplayList(UInt8 startPage, UInt8 textPage);
void selectDisplayList(UInt8 index);
void initFont(UInt8 fontPage);
void hidePlayfieldAndSprites(void);

// Color Table
void loadColorTable(const UInt8 *colors);
void setBackgroundGradient(const UInt8 *data);

#endif
