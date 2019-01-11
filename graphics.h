// graphics.h

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "types.h"


// VBI and DLI registers
#define CUR_TIMER ((UInt8 *)0x0600)
#define VB_TIMER ((UInt8 *)0x0601)
#define P3_XPOS ((UInt8 *)0x0603)
#define BG_COLOR ((UInt8 *)0x0620)

#define SCREEN_ROW_BYTES (40)
#define SCREEN_WINDOW ((UInt8 *)PEEKW(SAVMSC))
#define TEXT_WINDOW ((UInt8 *)PEEKW(TXTMSC))


enum ScreenMode {
	ScreenModeOff = 0,
	ScreenModeMap,
	ScreenModeDialog,
	ScreenModeBattle,
	ScreenModeInfo,
};
extern void setScreenMode(UInt8 mode);

// Color Table
extern void fadeOutColorTable(void);
extern void fadeInColorTable(const UInt8 *colorTable);
extern void loadColorTable(const UInt8 *colors);

// Drawing
UInt16 drawImage(const DataBlock *image, UInt8 rowOffset);
extern void drawBarChart(void);
extern void zeroOut16(UInt8 *ptr, UInt16 length);
extern void zeroOut8(UInt8 *ptr, UInt8 length);

// Timing
extern void delayTicks(UInt8 ticks);

// Init
extern void initGraphics(void);

#endif
