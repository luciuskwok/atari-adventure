// text.c

#include "text.h"
#include "tiles.h"
#include "atari_memmap.h"
#include <atari.h>


// == printString() ==
void printString(const char *s, UInt8 color, UInt8 x, UInt8 y) {
	UInt8 *screen = (UInt8 *)PEEKW(SAVMSC);
	UInt8 index = 0;
	char c;
	
	while (c = s[index]) {
		if (c < 0x20) {
			c += 0x40;
		} else if (c < 0x60) {
			c -= 0x20;
		}
		c += (color * 64);
		screen[(UInt16)x + 20 * (UInt16)y] = c;
		++x;
		++index;
	}
}

// == printDebugInfo() ==
void printDebugInfo(const char *label, UInt16 value, UInt8 position) {
	// Prints a label and a hex value in the text box area.
	char hexStr[5];
	char labelLength = strlen(label);
	
	hexString(hexStr, value);
	printString(label, 0, position, 11);
	printString(hexStr, 0, position + labelLength, 11);
}

// == hexString() ==
void hexString(char *s, UInt16 x) {
	char i;
	char c;
	
	for (i=0; i<4; ++i) {
		c = x & 0x0F;
		if (c < 10) {
			c += 0x30;
		} else {
			c += 0x41 - 10;
		}
		s[3-i] = c;
		x = x >> 4;
	}
	s[4] = 0;
}

// == strlen() ==
UInt8 strlen(const char *s) {
	char len = 0;
	while (s[len] != 0 && len < 0xFF) {
		++len;
	}
	return len;
}
