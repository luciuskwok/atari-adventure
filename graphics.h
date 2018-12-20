// graphics.h

#ifndef GRAPHICS_H
#define GRAPHICS_H

void initGraphics(void);
void initDisplayList(void);
void initFont(unsigned char fontPage);
void initSprites(void);

void clearScreen(void);
void fillScreen(void);
void drawSprite(unsigned char *sprite, char spriteLength, char player, char y);
void drawSpriteTile(unsigned char *sprite, char column, char row);

void printString(const char* s, unsigned char color, unsigned char x, unsigned char y);
void hexString(char *s, unsigned int x);


#endif
