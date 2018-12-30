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
	MessageEnterDialog,
	MessageEnterBattle,
	MessageEnterInfo,
	MessageReturnToMap,
};

// Functions

void initCursor(void);
void registerCursorEventHandler(CursorEventHandlerCallbackType handler);
void setCursorSprite(const UInt8 *sprite, UInt8 height);
void setCursorPosition(UInt8 x, UInt8 y);
void hideCursor(void);

SInt8 handleStick(void);
SInt8 handleTrigger(void);
void waitForAnyInput(void);
void resetAttractMode(void);
