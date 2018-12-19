// graphics.h

#ifndef GRAPHICS_H
#define GRAPHICS_H

void initGraphics(void);
void clearScreen(void);
void fillScreen(void);
void printString(const char* s, unsigned char color, unsigned char x, unsigned char y);
void drawSprite(unsigned char *sprite, char spriteLength, char player, char x, char y);

#endif
