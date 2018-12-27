// text.c

#include "text.h"
#include "graphics.h"
//#include "atari_memmap.h"
#include <atari.h>
#include <stdio.h>
#include <string.h>


// Constants
#define TEXTBOX_WIDTH (40)
#define TEXTBOX_HEIGHT (7)



void printCharaStats(UInt8 player, const UInt8 *name, UInt8 level, UInt8 hp, UInt8 maxHp) {
	UInt8 x = (player % 2) * 10 + 2;
	UInt8 y = (player / 2) * 4;
	UInt8 lvStr[9];
	UInt8 hpStr[9];

	printString(name, x, y);

	sprintf(lvStr, "Lv %u", level);
	printString(lvStr, x, y+1);

	sprintf(hpStr, "%u/%u", hp, maxHp);
	printString(hpStr, x, y+2);
}

void printPartyStats(SInt32 money, UInt16 potions, UInt16 fangs, SInt16 reputation) {
	UInt8 s[16];
	UInt8 len;
	const UInt8 x = 38;
	UInt8 y = 1;

	s[0] = '$';
	numberString(s+1, ',', money);
	len = strlen(s);
	printString(s, x-len-1, y);
	y += 2;

	numberString(s, 0, potions);
	strcat(s, "{");
	len = strlen(s);
	printString(s, x-len, y);
	y += 2;

	numberString(s, ',', fangs);
	strcat(s, "}");
	len = strlen(s);
	printString(s, x-len, y);
	y += 2;

}

void clearTextWindow(void) {
	UInt16 i;

	for (i=0; i<TEXTBOX_HEIGHT*TEXTBOX_WIDTH; ++i) {
		textWindow[i] = 0;
	}
}

void setTextWindowColorTheme(UInt8 theme) {
	switch (theme) {
		case 1:
			*TEXT_LUM = 0x0E;    // white
			*TEXT_BG  = 0x00;    // black
			break;
		case 2:
			*TEXT_LUM = 0x0E;    // white
			*TEXT_BG  = 0x92;    // dark blue
			break;
		default:
			*TEXT_LUM = 0x0E;    // white
			*TEXT_BG  = 0x04;    // gray
			break;
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

void printStringWithLayout(const UInt8 *s, UInt8 top, UInt8 firstIndent, UInt8 leftMargin, UInt8 rightMargin) {
	// firstIndent is independent of leftMargin
	// leftMargin controls lines after the first one
	// rightMargin controls all lines
	// text will be broken at space characters

	UInt8 i = 0;
	UInt8 y = top;
	UInt8 x = firstIndent;
	UInt8 previousBreakable = 0;
	UInt8 lineStartIndex = 0;
	UInt8 xMax = TEXTBOX_WIDTH - rightMargin;
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
			}
			c = s[i];

			// Start new line
			++y;
			x = leftMargin;
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
