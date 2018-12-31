// info.c

#include "info.h"
#include "atari_memmap.h"
#include "cursor.h"
#include "game_chara.h"
#include "graphics.h"
#include "image_data.h"
#include "puff.h"
#include "sprites.h"
#include "text.h"
#include <string.h>



#define ITEM_WINDOW_ROW (13)


static SInt8 infoCursorHandler(UInt8 event) {
	return (event == CursorClick) ? MessageReturnToMap : MessageNone;
}

static void drawCharaInfoAtIndex(UInt8 index) {
	GameCharaPtr chara = charaAtIndex(index);
	UInt8 maxHp = charaMaxHp(chara);
	UInt8 hp = chara->hp;
	UInt8 x = 1 + index * 10;
	UInt8 y = 0;
	UInt8 s[11];

	printString(chara->name, x, y++);

	stringCopy(s, "Lv ");
	uint8toString(s+3, chara->level);
	printString(s, x, y++);
	
	stringCopy(s, "next ");
	uint16toString(s+5, charaXpToNextLevel(chara));
	printString(s, x, y++);

	++y;

	printString("HP", x, y++);

	uint8toString(s, hp);
	stringConcat(s, "/");
	uint8toString(s+stringLength(s), maxHp);
	printString(s, x, y++);
	
	++y;

	stringCopy(s, "ATK ");
	uint8toString(s+4, charaAttackRating(chara));
	printString(s, x, y++);

	stringCopy(s, "DEF ");
	uint8toString(s+4, charaDefenseRating(chara));
	printString(s, x, y++);

	++y;

	printString("Sword+1", x, y++);
	printString("Armor+2", x, y++);
	printString("Shield+1", x, y++);
}

static void drawAvatarAtIndex(UInt8 index) {
	UInt8 *screen = (UInt8 *)PEEKW(SAVMSC);
	UInt8 *buffer = textWindow + 40 * 18; // use empty space after screen memory
	const UInt8 bufferRowBytes = 8; // 8 bytes * 4 ppb = 32 pixels
	const UInt8 imageHeight = 24;
	UInt16 bufferLength = imageHeight * bufferRowBytes;
	UInt16 compressedLength = avatarImages[index]->length;

	SInt8 err = puff(buffer, &bufferLength, avatarImages[index]->bytes, &compressedLength);

	if (err) {
		debugPrint("puff() error:", err, 1, ITEM_WINDOW_ROW);
	} else {
		// Copy image from buffer to screen
		UInt8 i = 0;
		UInt8 x, y;

		screen += index * 10 + 1; // align with chara info text

		for (y=0; y<imageHeight; ++y) {
			for (x=0; x<bufferRowBytes; ++x) {
				screen[x] = buffer[i++];
			}
			screen += 40; // next row
		}
	}

}

void initInfo(void) {
	const UInt8 missileHeight = 91;
	UInt8 count = numberInParty();
	UInt8 i;
	UInt8 s[20];

	UInt16 startTime = SHORT_CLOCK;
	UInt16 duration;

	// Set up graphics window
	setScreenMode(ScreenModeOff);
	setPlayerCursorVisible(0);
	clearRasterScreen(24+18); // also clears text window
	setCursorEventHandler(infoCursorHandler);

	// Position missile sprites as borders
	fillSprite(0, 0xFF, 0, missileHeight);
	fillSprite(0, 0x03, missileHeight, 128-missileHeight);
	setSpriteOriginX(5, 48);
	setSpriteOriginX(6, 88);
	setSpriteOriginX(7, 128);
	setSpriteOriginX(8, 168);

	// Turn on screen
	setScreenMode(ScreenModeInfo);
	fadeInColorTable(FadeTextBox, infoColorTable);

	// Print chara info
	for (i=0; i<count; ++i) {
		drawCharaInfoAtIndex(i);
	}

	// Print party info
	printString("Items", 1, ITEM_WINDOW_ROW);

	stringCopy(s, "Gold: $");
	uint16toString(s+stringLength(s), partyMoney);
	printString(s, 1, ITEM_WINDOW_ROW+1);

	stringCopy(s, "Herbs: ");
	uint8toString(s+stringLength(s), partyPotions);
	stringConcat(s, "{");
	printString(s, 1, ITEM_WINDOW_ROW+2);

	stringCopy(s, "Fangs: ");
	uint8toString(s+stringLength(s), partyFangs);
	stringConcat(s, "}");
	printString(s, 1, ITEM_WINDOW_ROW+3);

	stringCopy(s, "Nuts: ");
	uint8toString(s+stringLength(s), 11);
	printString(s, 16, ITEM_WINDOW_ROW+1);

	stringCopy(s, "Staff: ");
	uint8toString(s+stringLength(s), 3);
	printString(s, 16, ITEM_WINDOW_ROW+3);
	
	printString("Boat", 31, ITEM_WINDOW_ROW+1);
	printString("Lamp", 31, ITEM_WINDOW_ROW+2);
	printString("Crystal", 31, ITEM_WINDOW_ROW+3);
	printString("Mantle", 31, ITEM_WINDOW_ROW+4);

	// Draw avatars last because it can be slow
	for (i=0; i<count; ++i) {
		drawAvatarAtIndex(i);
	}

	duration = SHORT_CLOCK - startTime;
	uint16toString(s, duration);
	printString(s, 0, ITEM_WINDOW_ROW+4);
}
