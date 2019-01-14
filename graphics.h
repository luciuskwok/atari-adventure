// graphics.h

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "types.h"


// VBI and DLI registers
#define VB_TIMER ((UInt8 *)0x0601)

#define SCREEN_ROW_BYTES (40)
#define GRAPHICS_WINDOW ((UInt8 *)PEEKW(SAVMSC))
#define TEXT_WINDOW ((UInt8 *)PEEKW(TXTMSC))

extern UInt8 dliSpriteData[];
#define dliSpriteDataLength (10)


enum ScreenMode {
	ScreenModeOff = 0,
	ScreenModeMap,
	ScreenModeDialog,
	ScreenModeBattle,
	ScreenModeInfo,
};
void setScreenMode(UInt8 mode);

// PackBits
void unpackbits(UInt8 *dest, const UInt8 *source);

// Color Table
void fadeOutColorTable(void);
void fadeInColorTable(const UInt8 *colorTable);
void loadColorTable(const UInt8 *colors);

// Drawing
int drawCompressedImage(const DataBlock *image, UInt8 rowOffset, UInt8 rowCount);
void drawImage(const UInt8 *data);
void compositeImage(const UInt8 *data);
void drawBarChart(void);
void zeroOut16(UInt8 *ptr, UInt16 length);
void zeroOut8(UInt8 *ptr, UInt8 length);

// Timing
void delayTicks(UInt8 ticks);

// Init
void initGraphics(void);

// From misc.asm
void setDliColorTable(UInt8 *ptr);
UInt8* getDliColorTable(void);

#endif
