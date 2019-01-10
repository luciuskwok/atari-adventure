// graphics.h

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "types.h"

// Globals
extern UInt8 *graphicsWindow; // Pointer to screen memory for graphics
extern UInt8 *textWindow; // Pointer to screen memory for text window


// VBI and DLI registers
#define CUR_TIMER ((UInt8 *)0x0600)
#define VB_TIMER ((UInt8 *)0x0601)
#define P3_XPOS ((UInt8 *)0x0603)
#define BG_COLOR ((UInt8 *)0x0620)


enum ScreenMode {
	ScreenModeOff = 0,
	ScreenModeMap,
	ScreenModeDialog,
	ScreenModeBattle,
	ScreenModeInfo,
};
extern void __fastcall__ setScreenMode(UInt8 mode);

// Color Table
enum FadeOptions {
	FadeGradient = 1, FadeTextBox = 2
};
void fadeOutColorTable(UInt8 fadeOptions);
void fadeInColorTable(UInt8 fadeOptions, const UInt8 *colorTable);
extern void __fastcall__ loadColorTable(const UInt8 *colors);

// Drawing
void drawBarChart(UInt8 *screen, UInt8 x, UInt8 y, UInt8 width, UInt8 filled);
SInt8 drawImage(const DataBlock *image, UInt8 rowOffset, UInt8 rowCount);
void clearGraphicsWindow(UInt8 rows);

// Timing
void waitVsync(UInt8 ticks);

// Init
extern void __fastcall__ initFont(UInt8 fontPage);
void initGraphics(void);

#endif
