// cursor.c

#include "cursor.h"
#include <atari.h>
#include "atari_memmap.h"
#include "graphics.h"
#include "sprites.h"


// Globals
UInt8 previousStick;
UInt8 previousTrigger;
CursorEventHandlerCallbackType gCursorEventHandler;

// Event Handling

SInt8 handleStick(void) {
	// Only allow moves in 4 cardinal directions and not diagonals.
	UInt8 stick = STICK0;
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
	UInt8 trigger = STRIG0;
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
	while (STRIG0 == 0) {
	}

	// Then wait for trigger to be pressed
	while (STRIG0 != 0) {
		resetAttractMode();
	}
}

void resetAttractMode(void) {
	ATRACT_value = 0;
}

// Init

void setCursorEventHandler(CursorEventHandlerCallbackType handler) {
	gCursorEventHandler = handler;
}

void initCursor(void) {
	previousStick = 0x0F;
	previousTrigger = 1;
}

