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
void hidePlayfieldAndSprites(void);

// Transition Effects
enum FadeOptions {
	FadeGradient = 1, FadeTextBox = 2
};
UInt8 applyFade(UInt8 color, UInt8 amount);
void fadeOutColorTable(UInt8 fadeOptions);
void fadeInColorTable(UInt8 fadeOptions, const UInt8 *colorTable);

enum ScreenMode {
	ScreenModeOff = 0,
	ScreenModeMap,
	ScreenModeShop,
	ScreenModeBattle,
};
void setScreenMode(UInt8 mode);

// Color Table
void loadColorTable(const UInt8 *colors);
void setBackgroundGradient(const UInt8 *data);

#endif
