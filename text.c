// text.c

#include "text.h"
#include <atari.h>
#include "atari_memmap.h"
#include "tiles.h"


// == printString() ==
void printString(const char* s, unsigned char color, unsigned char x, unsigned char y) {
	unsigned char *screen = (unsigned char *)PEEKW(SAVMSC);
	unsigned char index = 0;
	char c;
	
	while (c = s[index]) {
		if (c < 0x20) {
			c += 0x40;
		} else if (c < 0x60) {
			c -= 0x20;
		}
		c += (color * 64);
		screen[(unsigned int)x + 20 * (unsigned int)y] = c;
		++x;
		++index;
	}
}

// == printDebugInfo() ==
void printDebugInfo(const char* label, unsigned int value, unsigned char position) {
	// Prints a label and a hex value in the text box area.
	char hexStr[5];
	char labelLength = strlen(label);
	
	hexString(hexStr, value);
	printString(label, 0, position, 11);
	printString(hexStr, 0, position + labelLength, 11);
}

// == hexString() ==
void hexString(char *s, unsigned int x) {
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
unsigned char strlen(const char *s) {
	char len = 0;
	while (s[len] != 0 && len < 0xFF) {
		++len;
	}
	return len;
}
