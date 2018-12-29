// text.c

#include "text.h"
#include "graphics.h"
#include <atari.h>


// Constants
#define TEXTBOX_WIDTH (40)
#define TEXTBOX_HEIGHT (10)


UInt8 toAtascii(UInt8 c) {
	if (c < 0x20) {
		c += 0x40;
	} else if (c < 0x60) {
		c -= 0x20;
	}
	return c;
}

void drawBarChart(UInt8 x, UInt8 y, UInt8 width, UInt8 filled) {
	UInt8 *pixel = textWindow + x + y * TEXTBOX_WIDTH;
	UInt8 i, c;

	for (i=0; i<width; ++i) {
		c = (i >= filled) ? 1 : 2;
		c <<= ((3-i) % 4) * 2;
		pixel[i/4] |= c;
	}
}

void printCharaStats(UInt8 player, UInt8 y, const UInt8 *name, UInt8 level, UInt8 hp, UInt8 maxHp) {
	UInt8 x = player * 10+1;
	UInt8 lvStr[9] = "Lv ";
	UInt8 hpStr[9];

	printString(name, x, y);

	numberString(lvStr+3, 0, level);
	printString(lvStr, x, y+1);

	numberString(hpStr, 0, hp);
	stringConcat(hpStr, "/");
	numberString(hpStr+stringLength(hpStr), 0, maxHp);
	printString(hpStr, x, y+2);

	// Draw bar chart
	{
		UInt8 width;
		UInt16 fill;

		if (maxHp >= 72) {
			width = 72;
		} else {
			width = maxHp;
		}
		fill = (UInt16)hp * width / maxHp;
		fill = (fill + 1) / 2;
		drawBarChart(x, y+3, width / 2, fill);
	}
}

void printAllCharaText(UInt8 y) {
	// Print character statistics
	printCharaStats(0, y, "Alisa", 99, 123, 255);
	printCharaStats(1, y, "Marie", 1, 1, 8);
	printCharaStats(2, y, "Guy", 19, 35, 36);
	printCharaStats(3, y, "Nyorn", 7, 70, 80);
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

void drawTextBox(const UInt8 *s, PointU8 *position, UInt8 width, UInt8 lineSpacing, SInt8 indent) {
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
