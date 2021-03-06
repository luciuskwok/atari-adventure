// info.c

#include "info.h"
#include "atari_memmap.h" // debugging
#include "cursor.h"
#include "game_chara.h"
#include "graphics.h"
#include "sound.h"
#include "sprites.h"
#include "text.h"


#define TEXT_WINDOW_ROW (25)
#define ITEM_ROW (14)

// Data

const UInt8 infoColorTable[] = {
	0x88, 0x88, 0xB6, 0xB6, // sprite
	0x3A,
	0x0E,
	0x00, 
	0x68, // missiles
	0x04, // background
	0x0E, // text luminance
	0x00, // text background
	0x82  // bar background
};

/*
const DataBlock avatar1ImageDeflated = {
	132, // length
	{
		0x4D,0xCC,0xB1,0x0D,0xC2,0x40,0x0C,0x05,0x50,0x4B,0x37,0x40,0x2A,0x46,0x89,0xA8,
		0x90,0xA7,0xF8,0xB2,0x44,0xE3,0x3D,0x10,0xE2,0x46,0xCA,0x04,0x28,0xD9,0x20,0xD5,
		0x89,0x06,0x94,0x15,0xAE,0x39,0xD1,0x70,0x32,0x36,0x4A,0x41,0xF5,0x2C,0x7F,0xFB,
		0x73,0xD7,0x9C,0xCD,0x8C,0x9B,0xEC,0x42,0xD5,0x1D,0x0A,0x20,0xE1,0x06,0xC4,0x3E,
		0x35,0xA0,0x79,0x90,0x7C,0x1F,0x01,0xBD,0x5C,0xB8,0x5B,0xFB,0xDD,0xD1,0xA3,0x43,
		0x7B,0x38,0xE1,0x76,0xCF,0x46,0xC7,0x19,0x97,0xFE,0xE7,0x38,0xE3,0x1C,0x1E,0xE2,
		0x4F,0x8D,0x8A,0x84,0x9F,0xDD,0x4A,0x05,0xDE,0x9C,0xC3,0xE8,0xAD,0x34,0xAA,0x4F,
		0xAB,0x2B,0x53,0x91,0xB4,0xD0,0x49,0xDE,0x90,0xA1,0x12,0xA3,0xE3,0xCA,0x95,0xAC,
		0x40,0x9E,0x5C,0xBF
	}
};
*/

const DataBlock avatar1ImageRaw = {
	192, 
	{
		0x3F,0xFD,0x5F,0x7F,0x7F,0xFF,0xFF,0xFF,0x3F,0xF5,0x57,0x7F,0x7F,0xFF,0xFF,0xFF,
		0x3F,0xF5,0x55,0x5F,0x5F,0xFF,0xFF,0xFF,0x0F,0xD5,0x55,0x55,0x57,0xFF,0xFF,0xFF,
		0x0F,0xDF,0x55,0x55,0x7F,0xFF,0xFF,0xFF,0x03,0xF5,0x55,0x55,0xF5,0x5F,0xFF,0xFF,
		0x03,0xD5,0x55,0x55,0x55,0x57,0xFF,0xFF,0x00,0xDD,0x55,0x55,0x55,0x55,0xFF,0xFF,
		0x00,0xDF,0xF5,0x55,0x57,0xFF,0xFF,0xFF,0x00,0xD7,0xFD,0x55,0x5F,0xFD,0xFF,0xFF,
		0x00,0xD7,0xAF,0x55,0x7E,0xBD,0x7F,0xFF,0x00,0x37,0xBF,0x55,0x7A,0xFD,0x7F,0xFF,
		0x00,0x37,0xBF,0x55,0x7A,0xFD,0x7F,0xFF,0x00,0x35,0xBF,0x55,0x5A,0xFD,0x7F,0xFF,
		0x00,0x15,0x55,0x55,0x55,0x55,0x5F,0xFF,0x00,0xD5,0x57,0x55,0x55,0x55,0x5F,0xFC,
		0x00,0xD5,0x57,0x55,0x55,0x55,0x5F,0xF0,0x00,0xD5,0x55,0xD5,0x55,0x55,0x7F,0xF0,
		0x00,0xD5,0x55,0x55,0x55,0x55,0xFF,0xF0,0x00,0x35,0x5F,0xD5,0x55,0x55,0xCF,0xF0,
		0x00,0x35,0x57,0xAF,0xD5,0x57,0x03,0xC0,0x00,0x3D,0x57,0xF7,0x55,0x57,0x0F,0xF0,
		0x00,0x3F,0x55,0xFD,0x55,0x7C,0x3F,0xF0,0x00,0xFF,0xD5,0x55,0x57,0xDC,0x3F,0xF0
	}
};

const DataBlock avatar2ImageRaw = {
	192, 
	{
		0x0A,0x64,0x81,0x55,0x55,0x4A,0x0A,0xA8,0x29,0x27,0x81,0x55,0x55,0x42,0x22,0xAA,
		0x2A,0x65,0xE1,0x53,0xF1,0x42,0x28,0xAA,0xAA,0x3C,0x71,0x3C,0x55,0x52,0x8A,0x2A,
		0xA4,0xDF,0x5D,0x55,0x55,0x50,0x8A,0x8A,0x90,0xC5,0x55,0x54,0x05,0x50,0xA2,0xA2,
		0xA0,0xE1,0x55,0x52,0x51,0x54,0xA2,0xA0,0xA3,0xFC,0x55,0x4F,0xC4,0x54,0xA2,0xBF,
		0x90,0xFF,0x55,0xFF,0xFF,0xD4,0xAB,0xD7,0x90,0x3D,0x55,0x53,0xFF,0x54,0xAB,0x7D,
		0x40,0xCD,0x55,0x53,0x2C,0x54,0xAB,0xC1,0x00,0xC0,0x55,0x50,0x21,0x50,0xAB,0xF1,
		0x00,0xE4,0x55,0x55,0x55,0x50,0xAB,0xF1,0x03,0x91,0x55,0x55,0x55,0x42,0xAA,0xD5,
		0x03,0x91,0x55,0x55,0x55,0x02,0xAA,0xFF,0x03,0x94,0x55,0x55,0x55,0x02,0xAA,0x02,
		0x03,0xA5,0x55,0x55,0x54,0x02,0xAA,0x0A,0x00,0x69,0x55,0x55,0x50,0x0A,0xAA,0x2A,
		0x00,0xE4,0x01,0x55,0x50,0x0A,0xAA,0x2A,0x00,0x39,0xF0,0x15,0x40,0x0A,0xAA,0x2A,
		0x00,0x39,0x50,0x55,0x40,0xEA,0xAA,0xEA,0x00,0x0E,0x01,0x55,0x00,0xEA,0xAA,0x2A,
		0x00,0x0E,0x55,0x54,0x00,0xEA,0xAA,0xEA,0x00,0x03,0x95,0x50,0x13,0xAA,0xAB,0xAA
	}
};

const DataBlock avatar2SpriteL = { 2, {0x80, 0x80} };
const DataBlock avatar2SpriteR = { 2, {0xA0, 0xE0} };

const DataBlock avatar3ImageRaw = {
	192, 
	{
		0xCF,0xFF,0xFF,0xFF,0xEF,0xFF,0xFB,0xFC,0xCF,0xFF,0xFD,0xFF,0xAF,0xBF,0xEB,0xFC,
		0xCF,0xFF,0xFE,0xAA,0xAC,0xAA,0xAB,0xFC,0xCF,0xCF,0xFA,0xAA,0xB2,0x8F,0xAB,0xFC,
		0xCF,0xEF,0xFC,0x2A,0xAA,0xCA,0xAB,0xFC,0xCF,0xEA,0xA8,0xFA,0xA9,0xAB,0xFB,0xFC,
		0xFF,0xEA,0xAA,0xA6,0xAA,0xAF,0xFD,0xFC,0xFF,0xEA,0xFF,0xEA,0xAA,0xBC,0xF9,0xFC,
		0xC7,0xEB,0xFF,0xFA,0xAA,0xBC,0x89,0xF0,0xCD,0xEF,0xFC,0xFA,0xAA,0x80,0x99,0xD0,
		0xC7,0xEB,0x7C,0xBA,0xAA,0xA2,0x69,0xC0,0xC4,0xE8,0x80,0xA6,0xAA,0xAA,0xAB,0x00,
		0xC4,0xEA,0x62,0xAA,0xAA,0xAA,0xAB,0x00,0xF5,0xEA,0xAA,0xAA,0xAA,0xAA,0xAB,0x00,
		0xFD,0xDA,0xAA,0xAA,0xAA,0xAA,0xAB,0x00,0xFF,0xDA,0xAA,0xAA,0xAA,0x2A,0xAB,0x00,
		0xFF,0xF6,0xAA,0xAA,0xA4,0xAA,0xA7,0x00,0xFF,0xF6,0xAA,0xAA,0xAA,0xAA,0xA3,0x00,
		0xFF,0xFD,0xAA,0xAA,0xAA,0xAA,0x9F,0x00,0xFF,0xFF,0x6A,0xAA,0xAA,0xEA,0xBF,0x00,
		0x3F,0xFF,0xDA,0xA8,0xFF,0xAA,0xFF,0x00,0x0F,0xFF,0xF6,0xAA,0x95,0xAB,0xFF,0x00,
		0x07,0xFF,0xFD,0xAA,0xAA,0xAF,0xFF,0x00,0x07,0xFF,0xFC,0xDA,0xAA,0xBF,0xFF,0x00
	}
};

const DataBlock avatar4ImageRaw = {
	192, 
	{
		0x00,0x35,0xDF,0x57,0xB5,0x57,0xFF,0x35,0x00,0x35,0xF6,0xD5,0xD5,0x55,0x55,0xD5,
		0x00,0x35,0xC5,0xB5,0x55,0xAA,0x55,0xD5,0x00,0x35,0xD5,0x85,0x55,0x3F,0x14,0x51,
		0x00,0x35,0xF3,0x11,0x55,0x55,0xC7,0x45,0x00,0xC5,0x3F,0x45,0x55,0x55,0xB7,0x55,
		0x0F,0x55,0xDB,0xD1,0x55,0x30,0x57,0x55,0x35,0x55,0xD8,0x31,0x57,0x3E,0x17,0x15,
		0xC5,0x55,0xD8,0xF1,0x55,0xEF,0x57,0x15,0xD5,0x55,0x18,0xF5,0x55,0xA0,0xD5,0xD5,
		0xFD,0x57,0x54,0x75,0x55,0xA3,0xE5,0xD5,0xC3,0x54,0x55,0xD5,0x55,0xA3,0xE5,0xD5,
		0x00,0xDE,0x57,0x55,0x55,0x60,0x55,0xD5,0x00,0x3E,0x57,0x55,0x55,0x55,0x55,0x35,
		0x00,0x0E,0x55,0x11,0x55,0x55,0x54,0xD5,0x00,0x0E,0x55,0x55,0x55,0x55,0x57,0x15,
		0x00,0x34,0x95,0x55,0x55,0x55,0x57,0x55,0x00,0xC7,0x97,0x55,0x55,0x55,0x54,0x55,
		0x00,0xD5,0x24,0xFC,0x54,0x55,0x5D,0x74,0x03,0x55,0xE5,0xEA,0xFF,0x55,0x4D,0xFD,
		0x03,0x55,0x49,0x00,0x45,0x55,0x4D,0x55,0x03,0x55,0x79,0x40,0x15,0x55,0xDD,0x55,
		0x03,0x55,0x52,0x55,0x55,0x57,0x77,0x55,0x0C,0x55,0x5E,0x55,0x55,0x4D,0x77,0x55
	}
};

const DataBlock *avatarImages[4] = {
	&avatar1ImageRaw,
	&avatar2ImageRaw,
	&avatar3ImageRaw,
	&avatar4ImageRaw
};

// avatar2SpriteL is reused for avatar4
const DataBlock avatar4SpriteR = { 4, {0xC0, 0x80, 0x80, 0xC0} };

// Functions

static SInt8 infoCursorHandler(UInt8 event) {
	return (event == CursorClick) ? MessageReturnToMap : MessageNone;
}

static void drawCharaInfoAtIndex(UInt8 index) {
	PlayerChara *chara = &partyChara[index];
	UInt8 maxHp = chara->maxHp;
	UInt8 hp = chara->hp;
	UInt8 x = 1 + index * 10;
	UInt8 s[9];

	POKE(BITMSK, 0); // Set text to non-inverse.
	SET_TXT_ORIGIN(x, 0)
	printLine(chara->name);

	printString("Lv.");
	uint8toString(s, chara->level);
	printLine(s);

	if (chara->level >= maxCharaLevel) {
		printLine("max");
	} else {
		printString("next ");
		uint16toString(s, chara->maxXp - chara->xp);
		printLine(s);
	}

	moveToNextLine();

	printLine("HP");

	uint8toString(s, hp);
	printString(s);
	printString("/");
	uint8toString(s, maxHp);
	printLine(s);
	
	moveToNextLine();

	printString("ATK ");
	uint8toString(s, chara->attack);
	printLine(s);

	printString("DEF ");
	uint8toString(s, chara->defense);
	printLine(s);

	moveToNextLine();

	printLine("Sword+1");
	printLine("Armor+2");
	printLine("Shield+1");
}

static void drawAvatarAtIndex(UInt8 index) {
	const UInt8 imageRowBytes = 8; // 8 bytes * 4 ppb = 32 pixels
	const UInt8 imageHeight = 24;	
	const UInt8 x = index * 10 + 1;
	const UInt8 y = 0;

	POKE(DELTAC, imageRowBytes);
	POKE(DELTAR, imageHeight);
	POKE(COLCRS, x);
	POKE(ROWCRS, y);

	drawImage(avatarImages[index]->bytes);
}

/*
static void drawDeflatedAvatarAtIndex(UInt8 index) {
	UInt8 *screen = GRAPHICS_WINDOW;
	UInt8 *buffer = TEXT_WINDOW + 40 * 18; // use empty space after screen memory
	const UInt8 bufferRowBytes = 8; // 8 bytes * 4 ppb = 32 pixels
	const UInt8 imageHeight = 24;
	UInt16 bufferLength = imageHeight * bufferRowBytes;
	UInt16 compressedLength = avatarImages[index]->length;

	SInt8 err = puff(buffer, &bufferLength, avatarImages[index]->bytes, &compressedLength);

	if (err) {
		debugPrint("puff() error:", err, 1, ITEM_ROW);
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
*/

static void setHiglightSprite(UInt8 index, const DataBlock *sprite, UInt8 x, UInt8 y) {
	clearSprite(index);
	drawSprite(sprite, index, PM_TOP_MARGIN + y);
	setSpriteOriginX(index, PM_LEFT_MARGIN + x);
	setSpriteWidth(index, 1);
}

void initInfo(void) {
	const UInt8 musicHeight = 12;
	const UInt8 portraitsHeight = 26;
	const UInt8 charaStatsHeight = 13*4+2;
	const UInt8 charaStatsBottom = musicHeight + portraitsHeight + charaStatsHeight;
	UInt16 oldTextWindow = PEEKW(TXTMSC);
	UInt8 count = partySize;
	UInt8 i;
	UInt8 s[20];

	// UInt16 startTime = Clock16;

	// Set up graphics window
	setScreenMode(ScreenModeOff);
	setPlayerCursorVisible(0);
	zeroOut16(GRAPHICS_WINDOW, (25+19)*SCREEN_ROW_BYTES); // Clear graphics and text windows
	setCursorEventHandler(infoCursorHandler);

	// Start music
	startSong(0);

	// Set TXTMSC to SAVMSC temporarily
	POKEW(TXTMSC, PEEKW(SAVMSC) + TEXT_WINDOW_ROW * SCREEN_ROW_BYTES);

	// Position missile sprites as borders
	fillSprite(0, 0xFF, 0, musicHeight);
	fillSprite(0, 0x00, musicHeight, portraitsHeight);
	fillSprite(0, 0xFF, musicHeight+portraitsHeight, charaStatsHeight);
	fillSprite(0, 0x03, charaStatsBottom, 128-charaStatsBottom);
	dliSpriteData[0] = 48; // Missile 0
	dliSpriteData[1] = 88; // Missile 0
	dliSpriteData[2] = 128; // Missile 0
	dliSpriteData[3] = 168; // Missile 0

	// Set up screen but without showing it
	loadColorTable(NULL);
	setScreenMode(ScreenModeInfo);

	// Fill title top margin raster areas
	setMemory8(GRAPHICS_WINDOW + (TEXT_WINDOW_ROW - 1) * SCREEN_ROW_BYTES, 0xAA, SCREEN_ROW_BYTES);
	setMemory8(GRAPHICS_WINDOW + (TEXT_WINDOW_ROW + ITEM_ROW - 1) * SCREEN_ROW_BYTES, 0xFF, SCREEN_ROW_BYTES);


	// Print chara info
	for (i=0; i<count; ++i) {
		drawCharaInfoAtIndex(i);
	}

	// Print party info

	// Column 1
	SET_TXT_ORIGIN(1, ITEM_ROW)
	POKE(BITMSK, 0);

	printLine("Items");

	printString("Gold:$");
	uint16toString(s, partyMoney);
	printLine(s);

	printString("Herbs:");
	uint8toString(s, partyPotions);
	printString(s);
	printLine("\x0E");

	printString("Fangs:");
	uint8toString(s, partyFangs);
	printString(s);
	printLine("\x0F");

	// Column 2
	SET_TXT_ORIGIN(16, ITEM_ROW+1)

	printString("Nuts:");
	uint8toString(s, 11);
	printLine(s);

	moveToNextLine();

	printString("Staves:");
	uint8toString(s, 3);
	printLine(s);

	// Column 3
	SET_TXT_ORIGIN(31, ITEM_ROW+1)

	printLine("Boat");
	printLine("Lamp");
	printLine("Crystal");
	printLine("Mantle");

	// Draw avatars after printing text because it can be slow
	for (i=0; i<count; ++i) {
		drawAvatarAtIndex(i);
	}

	// Add image highlights
	setHiglightSprite(1, &avatar2SpriteL, 44 + 5, 10);
	setHiglightSprite(2, &avatar2SpriteR, 44 + 14, 10);
	setHiglightSprite(3, &avatar2SpriteL, 124 + 11, 8);
	setHiglightSprite(4, &avatar4SpriteR, 124 + 22, 9);

	// debugPrint("Init:", Clock16 - startTime, 0, ITEM_ROW+4);

	// Show screen
	fadeInColorTable(infoColorTable);

	// Restore TXTMSC
	POKEW(TXTMSC, oldTextWindow);
}
