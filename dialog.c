// dialog.c 

#include "dialog.h"
#include "cursor.h"
#include "graphics.h"
#include "menu.h"
#include "sprites.h"
#include "text.h"
#include "atari_memmap.h"


#define GRAPHICS_HEIGHT (48)
#define TEXT_HEIGHT (20)

// Constants 

const UInt8 dialogColorTable[] = {
	0x58, // cursor sprite
	0x00, 0x00, 0x00, // unused sprites
	0x26, 0x0E, 0x22, // playfield
	0xFF, // missile sprites
	0x00, // background
	0x0E, 0x00, 0x82 // unused, no DLI
};

const UInt8 jabberwocky1[] = "'Twas brillig, and the slithy toves\nDid gyre and gimble in the wabe:\nAll mimsy were the borogoves,\nAnd the mome raths outgrabe.";
const UInt8 jabberwocky2[] = "\"Beware the Jabberwock, my son! The jaws that bite, the claws that catch! Beware the Jubjub bird, and shun The frumious Bandersnatch!\"";
const UInt8 mobyDick[] = "In good time, Flask's saying proved true. As before, the Pequod steeply leaned over towards the sperm whale's head, now, by the counterpoise of both heads, she regained her even keel; though sorely strained, you may well believe. So, when on one side you hoist in Locke's head, you go over that way; but now, on the other side, hoist in Kant's and you come back again; but in very poor plight. Thus, some minds for ever keep trimming boat. Oh, ye foolish! throw all these thunder-heads overboard, and then you will float light and right.";

// Globals 
const UInt8* nextText;


static void printNextText(void) {
	UInt16 oldTextWindow = PEEKW(TXTMSC);
	UInt8 *rowPtr = GRAPHICS_WINDOW + (GRAPHICS_HEIGHT + 1) * SCREEN_ROW_BYTES;
	UInt8 y;

	// Change TXTMSC temporarily
	POKEW(TXTMSC, (UInt16)rowPtr);

	// Clear text lines only
	for (y=1; y<TEXT_HEIGHT-1; y+=2) {
		zeroOut8(rowPtr, SCREEN_ROW_BYTES);
		rowPtr += SCREEN_ROW_BYTES * 2;
	}

	// Print text 
	POKE(RMARGN, 39); 		// right margin
	POKE(ROWINC, 2);

	SET_TXT_ORIGIN(1, 0)
	nextText = drawTextBox(nextText);

	// Restore TXTMSC
	POKEW(TXTMSC, oldTextWindow);
}

static SInt8 infoCursorHandler(UInt8 event) {
	if (event == CursorClick) {
		if (*nextText == 0) {
			return MessageReturnToMap;
		} else {
			printNextText();
		}
	}

	return MessageNone;
}

void initDialog(void) {
	UInt8 *rowPtr = GRAPHICS_WINDOW + GRAPHICS_HEIGHT * SCREEN_ROW_BYTES;
	UInt8 y;

	// Set up graphics window
	setScreenMode(ScreenModeOff);
	setPlayerCursorVisible(0);
	setCursorEventHandler(infoCursorHandler);
	setScreenMode(ScreenModeDialog);
	zeroOut16(GRAPHICS_WINDOW, (GRAPHICS_HEIGHT)*SCREEN_ROW_BYTES);

	for (y=0; y<TEXT_HEIGHT; y+=2) {
		setMemory8(rowPtr, 0xFF, SCREEN_ROW_BYTES);
		rowPtr += SCREEN_ROW_BYTES * 2;
	}

	// Init to start of text 
	nextText = mobyDick;
	printNextText();

	// Show screen
	fadeInColorTable(dialogColorTable);

}
