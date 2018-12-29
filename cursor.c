// cursor.c

#include "cursor.h"
#include "atari_memmap.h"
#include "graphics.h"
#include "sprites.h"


// Globals
UInt8 previousStick;
UInt8 previousTrigger;
UInt8 cursorHeight;
const UInt8 *cursorSprite;
CursorEventHandlerCallbackType gCursorEventHandler;

// Functions

void initCursor(void) {
	previousStick = 0x0F;
	previousTrigger = 1;
}

void registerCursorEventHandler(CursorEventHandlerCallbackType handler) {
	gCursorEventHandler = handler;
}

void setCursorSprite(const UInt8 *sprite, UInt8 height) {
	cursorSprite = sprite;
	cursorHeight = height;
}

void setCursorPosition(PointU8 *newPos) {
	static UInt8 previousY = 0;
	const UInt8 topMargin = 14;

	// Remove old sprite data
	drawSprite(NULL, cursorHeight, 1, topMargin + previousY);
	
	// Draw sprite in new position
	drawSprite(cursorSprite, cursorHeight, 1, topMargin + newPos->y);
	setSpriteHorizontalPosition(1, PM_LEFT_MARGIN - 8 + newPos->x);

	previousY = newPos->y;
}

void hideCursor(void) {
	setSpriteHorizontalPosition(1, 0);
}

// Event Handling

SInt8 handleStick(void) {
	// Only allow moves in 4 cardinal directions and not diagonals.
	UInt8 stick = PEEK (STICK0);
	UInt8 vb_timer = *VB_TIMER;
	UInt8 cursorEvent = CursorNone;
	SInt8 msg = 0;

	if (stick == previousStick && vb_timer != 0) { 
		// Handle changes in stick position immediately but delay repeating same moves.
		return 0;
	}
	*VB_TIMER = 10; // Reset stick timer
	previousStick = stick;

	switch (stick) {
		case 0x0E: cursorEvent = CursorUp; break; 
		case 0x0D: cursorEvent = CursorDown; break; 
		case 0x0B: cursorEvent = CursorLeft; break;
		case 0x07: cursorEvent = CursorRight; break;
		default: break;
	}
	
	if (cursorEvent != CursorNone && gCursorEventHandler) {
		msg = gCursorEventHandler(cursorEvent);
	}
	return msg;
}

SInt8 handleTrigger(void) {
	UInt8 trigger = PEEK (STRIG0);
	SInt8 msg = 0;

	// Recognize trigger only when it transitions from up to down.
	if (trigger == previousTrigger) {
		return 0;
	}
	previousTrigger = trigger;
	if (trigger != 0) {  // trigger == 0 when it is pressed
		return 0;
	}
	if (gCursorEventHandler) {
		msg = gCursorEventHandler(CursorClick);
	}
	return msg;
}

void waitForAnyInput(void) {
	// Wait for trigger to be released first.
	while (PEEK(STRIG0) == 0) {
	}

	// Then wait for trigger to be pressed
	while (PEEK(STRIG0) != 0) {
		resetAttractMode();
	}
}

void resetAttractMode(void) {
	POKE(ATRACT, 0);
}
