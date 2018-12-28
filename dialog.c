// dialog.c

#include "dialog.h"
#include "cursor.h"
#include "graphics.h"
#include "images.h"
#include "image_data.h"
#include "sprites.h"
#include "text.h"
#include <stdio.h>
#include <string.h>


// Globals
PointU8 cursorPosition;
PointU8 menuPosition;
UInt8 menuItemCount;
UInt8 selectedRow;



// Data
UInt8 exitString[] = "Exit";

UInt8 temMessage1[] = "* hOi!\n\n* welcom to...\n\n* da TEM SHOP!!!";
UInt8 temMenu1A[] = "Buy";
UInt8 temMenu1B[] = "Sell";
UInt8 temMenu1C[] = "Talk";

UInt8 temMessage2[] = "HOI!!!\nim temmie";
UInt8 temMenu2A[] = "Say Hello";
UInt8 temMenu2B[] = "About Temmie Armor";
UInt8 temMenu2C[] = "Temmie History";
UInt8 temMenu2D[] = "About Shop";

// Testing strings:
UInt8 sansMessage[] = "Sans: Why are graveyards so noisy?\n Because of all the *coffin*!";
UInt8 ellieMessage[] = "Ellie: How are you doing today? That teacher was totally unfair. C'mon, let's go to the beach!";
UInt8 papyrusMessage[] = "Papyrus: Nyeh Heh Heh!";



enum DialogLayout {
	LayoutBlank = 0,
	LayoutSplitA,
	LayoutSplitB,
};

void layoutDialog(UInt8 mode) {
	UInt8 i;

	clearTextWindow();

	switch (mode) {
		case LayoutSplitA:
		case LayoutSplitB:
			for (i=0; i<7; ++i) {
				printString("|", 28, i);
			}
			break;
	}
}

void drawMenu(UInt8 **items, UInt8 itemCount, PointU8 *position) {
	UInt8 x = position->x;
	UInt8 y = position->y;
	UInt8 *itemString;
	UInt8 i;

	for (i=0; i<itemCount; ++i) {
		itemString = items[i];
		printString(itemString, x, y);
		++y;
	}
}

void setCursorPosition(PointU8 *newPos) {
	const UInt8 topMargin = 14;

	// Remove old sprite data
	drawSprite(NULL, selectionCursorSpriteHeight, 1, topMargin + 4 * cursorPosition.y);
	
	// Draw sprite in new position
	drawSprite(selectionCursorSprite, selectionCursorSpriteHeight, 1, topMargin + 4 * newPos->y);
	setSpriteHorizontalPosition(1, PM_LEFT_MARGIN - 8 + 4 * newPos->x);

	cursorPosition.x = newPos->x;
	cursorPosition.y = newPos->y;
}

void setCursorAtMenuItem(UInt8 index) {
	const UInt8 topMargin = 18;
	PointU8 cell;
	cell.x = menuPosition.x;
	cell.y = menuPosition.y + index + topMargin;
	setCursorPosition(&cell);
	selectedRow = index;
}

void presentDialog(void) {

	// Set up graphics window
	fadeOutColorTable(FadeTextBox);
	setScreenMode(ScreenModeOff);
	clearTextWindow();
	clearRasterScreen();
	setPlayerCursorVisible(0);

	// Turn on screen
	loadColorTable(temShopColorTable);
	// setTextWindowColorTheme(1);
	// setBackgroundGradient(gradient);
	setScreenMode(ScreenModeShop);

	// Set up sprites
	clearSpriteData(3);
	clearSpriteData(4);
	drawSprite(temFaceSprite, temFaceSpriteHeight, 3, 22+14);
	drawSprite(temFaceSprite+temFaceSpriteHeight, temFaceSpriteHeight, 4, 22+14);
	setSpriteHorizontalPosition(3, PM_LEFT_MARGIN + 72);
	setSpriteHorizontalPosition(4, PM_LEFT_MARGIN + 80);

	// Draw message and menu 
	{
		PointU8 pt;
		UInt8 *menu[5];

		layoutDialog(LayoutSplitA);

		pt.x = 4;
		pt.y = 1;
		drawTextBox(sansMessage, &pt, 20);
		// temMessage1

		menu[0] = temMenu1A;
		menu[1] = temMenu1B;
		menu[2] = temMenu1C;
		menu[3] = exitString;
		menuPosition.x = 32;
		menuPosition.y = 1;
		menuItemCount = 4;
		drawMenu(menu, menuItemCount, &menuPosition);
	}

	// Draw money and potion count.
	printString("$901", 29, 6);
	printString("21{", 37, 6);

	// Draw background image
	{
		SInt8 err = drawImage(temShopImage, temShopImageLength);
		if (err) {
			UInt8 *s;
			sprintf(s, "puff() error:%c", err);
			printString(s, 1, 1);
			waitForAnyInput();
		}
	}

	// Selection Cursor
	clearSpriteData(1);
	setPlayerCursorColorCycling(1);
	setCursorAtMenuItem(0);


	registerCursorEventHandler(dialogCursorHandler);
}

void exitDialog(void) {
	// Fade out
	fadeOutColorTable(FadeGradient | FadeTextBox);
	clearSpriteData(4);
	hideSprites();
}

SInt8 dialogCursorHandler(UInt8 event) {
	UInt8 newRow = selectedRow;

	switch (event) {
		case CursorClick:
			return MessageExitDialog;
		case CursorUp:
			--newRow;
			break;
		case CursorDown:
			++newRow;
			break;
		case CursorLeft:
			break;
		case CursorRight:
			break;
	}

	if (newRow != selectedRow && newRow < menuItemCount) {
		setCursorAtMenuItem(newRow);
	}

	return MessageNone;
}
