// text.c

#include "text.h"
#include "atari_memmap.h"
#include "game_chara.h"
#include "graphics.h"
#include <atari.h>


// Constants
#define TEXTBOX_WIDTH (40)
#define TEXTBOX_HEIGHT (8)


void drawHpBar(UInt8 x, UInt8 y, UInt8 hp, UInt8 maxHp) {
	UInt8 width;
	UInt8 fill;
	UInt8 remainder;
	UInt16 n;

	if (maxHp >= 72) {
		width = 36;
	} else {
		width = maxHp / 2;
	}
	n = (UInt16)hp * width;
	remainder = n % maxHp;
	fill = n / maxHp;
	if (remainder) {
		++fill;
	}
	drawBarChart(textWindow, x * 4, y, width, fill);
}

static void printCharaStats(UInt8 x, UInt8 y, GameCharaPtr chara) {
	UInt8 hp = chara->hp;
	UInt8 maxHp = charaMaxHp(chara);
	UInt8 lvStr[9] = "Lv ";
	UInt8 hpStr[9];

	SET_TXT_ORIGIN(x, y)
	printLine(chara->name);

	uint8toString(lvStr+3, chara->level);
	printLine(lvStr);

	uint8toString(hpStr, hp);
	stringConcat(hpStr, "/");
	uint8toString(hpStr+stringLength(hpStr), maxHp);
	printLine(hpStr);

	drawHpBar(x, y+3, hp, maxHp);
}

void printCharaAtIndex(UInt8 index, UInt8 y, UInt8 clear) {
	UInt8 x = 1 + index * 10;
	GameCharaPtr chara = charaAtIndex(index);
	RectU8 r;

	if (clear) {
		r.origin.x = x;
		r.origin.y = y;
		r.size.width = 10;
		r.size.height = 4;
		clearTextRect(&r);
	}

	printCharaStats(x, y, chara);
}

void printAllCharaText(UInt8 y) {
	UInt8 count = numberInParty();
	UInt8 i;

	for (i=0; i<count; ++i) {
		printCharaAtIndex(i, y, 0);
	}
}

void printPartyStats(void) {
	UInt8 s[40];
	UInt8 len;
	UInt8 x;

	s[0] = '$';
	uint16toString(s+1, partyMoney);
	stringConcat(s, "  ");

	uint8toString(s+stringLength(s), partyPotions);
	stringConcat(s, "{  ");

	uint8toString(s+stringLength(s), partyFangs);
	stringConcat(s, "}");

	len = stringLength(s);
	x = 20 - len / 2;

	SET_TXT_ORIGIN(x, 4)
	printLine(s);
}

void clearTextWindow(UInt8 lines) {
	UInt16 i;

	for (i=0; i<lines*TEXTBOX_WIDTH; ++i) {
		textWindow[i] = 0;
	}
}

void clearTextRect(RectU8 *rect) {
	UInt8 rowSkip = TEXTBOX_WIDTH - rect->size.width;
	UInt16 i = rect->origin.x + TEXTBOX_WIDTH * rect->origin.y;
	UInt8 x, y;

	for (y=0; y<rect->size.height; ++y) {
		for (x=0; x<rect->size.width; ++x) {
			textWindow[i++] = 0;
		}
		i += rowSkip;
	}
}

void printStringAtXY(const UInt8 *s, UInt8 x, UInt8 y) {
	SET_TXT_ORIGIN(x, y)
	printLine(s);
}

void drawTextBox(const UInt8 *s, UInt8 x, UInt8 y, UInt8 width, UInt8 lineSpacing, SInt8 indent) {
	UInt8 i = 0;
	UInt8 previousBreakable = 0;
	UInt8 lineStartIndex = 0;
	UInt8 xMax = x + width;
	UInt8 isNewLine = 1;
	UInt8 left = x;
	UInt8 c;

	if (indent < 0) {
		left -= indent;
	}

	while (y < TEXTBOX_HEIGHT) {
		c = s[i];
		if (c == ' ' || c == '\n') { // whitespace
			previousBreakable = i;			
		}

		if (x >= xMax || c == '\n') {
			isNewLine = 0;

			// Rewind to previous breakable character
			if (previousBreakable > lineStartIndex) {
				while (i > previousBreakable + 1) {
					--i;
					--x;
					textWindow[x + TEXTBOX_WIDTH * y] = 0; // space char
				}			
			}

			if (c == '\n') {
				isNewLine = 1;
			}

			// Skip past whitespace
			while (s[i] == ' ' || s[i] == '\n') {
				++i;
				if (s[i] == '\n') {
					y += lineSpacing;
					isNewLine = 1;
				}
			}
			c = s[i];

			// Start new line
			y += lineSpacing;
			x = left;
			if (isNewLine != 0) {
				x += indent;
			}
			lineStartIndex = i;
			previousBreakable = i;
		}

		if (c == 0) { return; } // End of string

		textWindow[x + TEXTBOX_WIDTH * y] = toAtascii(c);
		++x;
		++i;
		c = s[i];
	}
}

void uint8toString(UInt8 *outString, UInt8 value) {
	if (value >= 100) {
		*outString++ = value / 100 + 0x30;
	}
	if (value >= 10) {
		*outString++ = (value / 10) % 10 + 0x30;
	}
	*outString++ = value % 10 + 0x30;
	*outString = 0;
}

void uint16toString(UInt8 *outString, UInt16 value) {
	if (value >= 10000) {
		*outString++ = value / 10000 + 0x30;
	}
	if (value >= 1000) {
		*outString++ = (value / 1000) % 10 + 0x30;
	}
	if (value >= 100) {
		*outString++ = (value / 100) % 10 + 0x30;
	}
	if (value >= 10) {
		*outString++ = (value / 10) % 10 + 0x30;
	}
	*outString++ = value % 10 + 0x30;
	*outString = 0;	
}

void hexString(UInt8 *outString, UInt8 length, UInt16 value) {
	UInt8 i, c;	
	for (i=0; i<length; ++i) {
		c = value & 0x0F;
		if (c < 10) {
			c += 0x30;
		} else {
			c += 0x41 - 10;
		}
		outString[length-1-i] = c;
		value = value >> 4;
	}
	outString[length] = 0;
}

void debugPrint(const UInt8 *s, UInt16 value, UInt8 x, UInt8 y) {
	UInt8 message[40] = { 0 };
	UInt8 valStr[6];

	uint16toString(valStr, value);
	stringConcat(message, s);
	stringConcat(message, valStr);
	printStringAtXY(message, x, y);
}
