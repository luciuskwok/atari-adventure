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
	MessageReturnToMap,
	MessageEnterDialog,
	MessageEnterShop,
	MessageEnterBattle,
	MessageEnterInfo,
};

// Functions

SInt8 handleStick(void);
SInt8 handleTrigger(void);
void waitForAnyInput(void);
void resetAttractMode(void);

void setCursorEventHandler(CursorEventHandlerCallbackType handler);
void initCursor(void);
