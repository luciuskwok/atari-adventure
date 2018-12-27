// cursor.h

#include "types.h"


// Callbacks
typedef void (*CursorEventHandlerCallbackType)(UInt8 eventType);
enum CursorEventType {
	CursorNone = 0,
	CursorClick = 1,
	CursorUp,
	CursorDown,
	CursorLeft,
	CursorRight
};

// Functions

void initCursor(void);
void registerCursorEventHandler(CursorEventHandlerCallbackType handler);
void handleStick(void);
void handleTrigger(void);
void waitForAnyInput(void);
void resetAttractMode(void);
