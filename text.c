// text.c

#include "text.h"
#include "graphics.h"
#include <atari.h>


// Constants
#define TEXTBOX_WIDTH (40)
#define TEXTBOX_HEIGHT (7)


void setTextBoxColors(void) {
	// Set text window colors to defaults
	*TEXT_LUM = 0x0E;    // white
	*TEXT_BG  = 0x04;    // grey
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

void printCharaStats(UInt8 player, const UInt8 *name, UInt8 level, UInt8 hp, UInt8 maxHp) {
	UInt8 x = player * 10;
	UInt8 y = 0;
	UInt8 lvStr[9] = "Lv ";
	UInt8 hpStr[9];

	printString(name, x, 0);

	numberString(lvStr+3, 0, level);
	printString(lvStr, x, 1);

	numberString(hpStr, 0, hp);
	stringConcat(hpStr, "/");
	numberString(hpStr+stringLength(hpStr), 0, maxHp);
	printString(hpStr, x, 2);

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
		drawBarChart(x, 3, width / 2, fill);
	}
}

void printPartyStats(SInt32 money, UInt16 potions, UInt16 fangs, SInt16 /* reputation */) {
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

void clearTextWindow(void) {
	UInt16 i;

	for (i=0; i<TEXTBOX_HEIGHT*TEXTBOX_WIDTH; ++i) {
		textWindow[i] = 0;
	}
}

void printStatText(void) {
	clearTextWindow();

	// Print character statistics
	printCharaStats(0, "Alisa", 99, 123, 255);
	printCharaStats(1, "Marie", 1, 1, 8);
	printCharaStats(2, "Guy", 19, 35, 36);
	printCharaStats(3, "Nyorn", 7, 1, 40);

	// Print party statistics
	printPartyStats(987123, 21, 1325, -891);
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

void drawTextBox(const UInt8 *s, PointU8 *position, UInt8 width, UInt8 lineSpacing) {
	// text will be broken at space characters

	UInt8 i = 0;
	UInt8 y = position->y;
	UInt8 x = position->x;
	UInt8 previousBreakable = 0;
	UInt8 lineStartIndex = 0;
	UInt8 xMax = x + width;
	UInt8 c;

	while (y < TEXTBOX_HEIGHT) {
		c = s[i];
		if (c == ' ' || c == '\n') { // whitespace
			previousBreakable = i;
		}

		if (x >= xMax || c == '\n') {
			// Rewind to previous breakable character
			if (previousBreakable > lineStartIndex) {
				while (i > previousBreakable + 1) {
					--i;
					--x;
					textWindow[x + TEXTBOX_WIDTH * y] = 0; // space char
				}			
			}

			// Skip past whitespace
			while (s[i] == ' ' || s[i] == '\n') {
				++i;
				if (s[i] == '\n') {
					y += lineSpacing;
				}
			}
			c = s[i];

			// Start new line
			y += lineSpacing;
			x = position->x;
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

UInt8 toAtascii(UInt8 c) {
	if (c < 0x20) {
		c += 0x40;
	} else if (c < 0x60) {
		c -= 0x20;
	}
	return c;
}
