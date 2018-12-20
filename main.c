// main.c

/* Build commands:
/Applications/Emulation/cc65-master/bin/cl65 -O -t atari -C config.cfg main.c graphics.c interrupts.s -o adv.xex
open adv.xex
*/

// == About This Program ==
// This program is a rogue-like game. 

// == Notes on Memory Usage ==
// In the config.cfg file, the reserved memory is set to 0x0820, giving us 2KB of space below the display area, which itself takes 1KB. The space between APPMHI and MEMTOP should be ours to use. 
// Calculate 3KB from RAMTOP, which is 12*256 bytes.
// RAMTOP: 0xC0 without BASIC, 0xA0 with BASIC. Below are values with BASIC.
// 0x9400: Custom character set needs 128 chars * 8 bytes = 1024 bytes.
// 0x9800: PMGraphics needs 640 bytes (double-line sprites), but the data area doesn't start until 384 bytes after PMBase, which must be on 1KB boundary.
// 0x9C00: Display list and screen memory has 1024 bytes allocated, so that the graphics driver can do its thing. We overwrite this area with our own display list, which takes up less space than the standard text screen.


// Includes
#include <atari.h>
#include <conio.h>
#include "atari_memmap.h"
#include "graphics.h"


// Globals
unsigned char gQuit;

// Function prototypes
void hexString(char *s, unsigned int x);



// == main() == 
int main (void) {
	// Graphics
	initGraphics();
	
	// Put something on screen
	fillScreen();

	// Start new game
	gQuit = 0;
	while (gQuit == 0) {
		//startNewGame();
		//runLoop();
	}

	return 0; // success
}

