// cursor.c

#include "cursor.h"
#include "atari_memmap.h"
#include "graphics.h"


// Globals
UInt8 previousStick;
UInt8 previousTrigger;
CursorEventHandlerCallbackType gCursorEventHandler;

// Functions

void initCursor(void) {
	previousStick = 0x0F;
	previousTrigger = 1;
}

void registerCursorEventHandler(CursorEventHandlerCallbackType handler) {
	gCursorEventHandler = handler;
}

void handleStick(void) {
	// Only allow moves in 4 cardinal directions and not diagonals.
	UInt8 stick = PEEK (STICK0);
	UInt8 vb_timer = *VB_TIMER;
	UInt8 cursorEvent = CursorNone;

	if (stick == previousStick && vb_timer != 0) { 
		// Handle changes in stick position immediately but delay repeating same moves.
		return;
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
		gCursorEventHandler(cursorEvent);
	}
}

void handleTrigger(void) {
	UInt8 trigger = PEEK (STRIG0);

	// Recognize trigger only when it transitions from up to down.
	if (trigger == previousTrigger) {
		return;
	}
	previousTrigger = trigger;
	if (trigger != 0) {  // trigger == 0 when it is pressed
		return;
	}
	if (gCursorEventHandler) {
		gCursorEventHandler(CursorClick);
	}
}
