// text.c

#include "text.h"
#include "game_chara.h"
#include "graphics.h"
#include <atari.h>


// Constants
#define TEXTBOX_WIDTH (40)
#define TEXTBOX_HEIGHT (8)


UInt8 toAtascii(UInt8 c) {
	if (c < 0x20) {
		c += 0x40;
	} else if (c < 0x60) {
		c -= 0x20;
	}
	return c;
}

void printCharaStats(UInt8 x, UInt8 y, GameCharaPtr chara) {
	UInt8 hp = chara->hp;
	UInt8 maxHp = charaMaxHp(chara);
	UInt8 lvStr[9] = "Lv ";
	UInt8 hpStr[9];

	printString(chara->name, x, y);

	numberString(lvStr+3, 0, chara->level);
	printString(lvStr, x, y+1);

	numberString(hpStr, 0, hp);
	stringConcat(hpStr, "/");
	numberString(hpStr+stringLength(hpStr), 0, maxHp);
	printString(hpStr, x, y+2);

	// Draw bar chart
	{
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
		drawBarChart(textWindow, x * 4, y + 3, width, fill);
	}
}

void printAllCharaText(UInt8 y) {
	UInt8 count = numberInParty();
	UInt8 x = 1;
	GameCharaPtr chara;
	UInt8 i;

	for (i=0; i<count; ++i) {
		chara = charaAtIndex(i);
		printCharaStats(x, y, chara);
		x += 10;
	}
}

void printPartyStats(SInt32 money, UInt16 potions, UInt16 fangs) {
	UInt8 s[40];
	UInt8 len;
	UInt8 x;

	s[0] = '$';
	numberString(s+1, ',', money);
	stringConcat(s, "  ");

	numberString(s+stringLength(s), 0, potions);
	stringConcat(s, "{  ");

	numberString(s+stringLength(s), 0, fangs);
	stringConcat(s, "}");

	len = stringLength(s);
	x = 20 - len / 2;

	printString(s, x, 4);
}

void clearTextWindow(UInt8 lines) {
	UInt16 i;

	for (i=0; i<lines*TEXTBOX_WIDTH; ++i) {
		textWindow[i] = 0;
	}
}

void printString(const UInt8 *s, UInt8 x, UInt8 y) {
	UInt8 index = 0;
	UInt8 c;

	while (c = s[index]) {
		c = toAtascii(c);
		textWindow[x + TEXTBOX_WIDTH * y] = c;
		++x;
		++index;
	}
}

void drawTextBox(const UInt8 *s, const PointU8 *position, UInt8 width, UInt8 lineSpacing, SInt8 indent) {
	// text will be broken at space characters

	UInt8 i = 0;
	UInt8 y = position->y;
	UInt8 x = position->x + indent;
	UInt8 previousBreakable = 0;
	UInt8 lineStartIndex = 0;
	UInt8 xMax = x + width;
	UInt8 isNewLine = 1;
	UInt8 c;

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
			x = position->x;
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

void numberString(UInt8 *outString, UInt8 thousandsSeparator, SInt32 value) {
	UInt8 isNegative = 0;
	UInt8 len = 0;
	UInt32 x;
	UInt8 c, i, j;

	if (value == 0) { // Special case for value of zero.
		outString[0] = '0';
		++len;
	} else {
		if (value < 0) {
			x = -value;
			isNegative = 1;
		} else {
			x = value;
		}

		// Add digits in reverse order, then reverse the string.
		while (x != 0) {
			if ((len >= 3) && (len % 4 == 3) && (thousandsSeparator != 0)) {
				outString[len] = thousandsSeparator;
				++len;
			}
			c = x % 10;
			outString[len] = c + 0x30;
			++len;
			x = x / 10;
		}

		// Add negative sign
		if (isNegative) {
			outString[len] = '-';
			++len;
		}

		// Reverse the string
		for (i=0, j=len-1; i<j; ++i, --j) {
			c = outString[j];
			outString[j] = outString[i];
			outString[i] = c;
		}
	}
	outString[len] = 0; // null terminate string
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

	numberString(valStr, 0, value);
	stringConcat(message, s);
	stringConcat(message, valStr);
	printString(message, x, y);
}
