// cursor.h

#include "types.h"


// Callbacks
typedef SInt8 (*CursorEventHandlerCallbackType)(UInt8 eventType);
enum CursorEventType {
	CursorNone = 0,
	CursorClick = 1,
	CursorUp,
	CursorDown,
	CursorLeft,
	CursorRight
};
enum CursorEventMessages {
	MessageNone = 0,
	MessageEnterDialog = 1,
	MessageEnterBattle = 2,
	MessageReturnToMap = 3,
};

// Functions

void initCursor(void);
void registerCursorEventHandler(CursorEventHandlerCallbackType handler);
SInt8 handleStick(void);
SInt8 handleTrigger(void);
void waitForAnyInput(void);
void resetAttractMode(void);
