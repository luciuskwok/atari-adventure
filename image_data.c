// image_data.c

#include "image_data.h"

// Tile Data

const UInt8 tileBitmaps[] = { 
	tCastle   , 0x00, 0x00, 0x22, 0x1C, 0x1C, 0x18, 0x22, 0x00, // Castle bkgnd
	tTown     , 0x00, 0x00, 0x28, 0x00, 0x28, 0x00, 0x00, 0x00, // Town bkgnd
	tVillage  , 0xFF, 0xD3, 0x89, 0xC9, 0xAC, 0x93, 0xCD, 0xF7, // Village bkgnd
	tMonument , 0xFF, 0xFF, 0xFF, 0xE7, 0xE3, 0xF1, 0xF8, 0xFC, // Monument bkgnd
	tCave     , 0xFF, 0xFF, 0xFF, 0xE7, 0xE7, 0xE7, 0xFF, 0xFF, // Cave bkgnd
	tPlains   , 0xFF, 0xF7, 0xFF, 0xBF, 0xFD, 0xFF, 0xEF, 0xFF, // Plains
	tBridgeH  , 0x7E, 0x7E, 0x00, 0x7E, 0x7E, 0x00, 0x7E, 0x7E, // Bridge (east-west)
	tBridgeV  , 0x00, 0xDB, 0xDB, 0xDB, 0xDB, 0xDB, 0xDB, 0x00, // Bridge (north-south)
	tForest   , 0xFF, 0xF7, 0xE3, 0xF7, 0xC1, 0xF7, 0x80, 0xF7, // Forest
	tShallows , 0xFE, 0xFF, 0xFB, 0xFF, 0xEF, 0xFF, 0xBF, 0xFF, // Shallow water
	tWater    , 0xCE, 0xB5, 0x5D, 0x73, 0xCE, 0xBA, 0xAD, 0x73, // Deep water
	tSolid    , 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // Desert
	tMountain , 0xF7, 0xE3, 0xC1, 0x80, 0xFF, 0xBF, 0x1F, 0x0E, // Mountains
	tPotion   , 0x3C, 0x18, 0x18, 0x2C, 0x5E, 0x7E, 0x3C, 0x00, // Health Potion
	tFang     , 0x04, 0x04, 0x0C, 0x0C, 0x1C, 0x7C, 0x78, 0x30, 
	tBrick    , 0xDF, 0xDF, 0xDF, 0x00, 0xFD, 0xFD, 0xFD, 0x00, // Dungeon tiles
	tFloor    , 0xFE, 0xFF, 0xBB, 0xFF, 0xEF, 0xFF, 0xBB, 0xFF, 
	tLadder   , 0x24, 0x3C, 0x24, 0x3C, 0x24, 0x3C, 0x24, 0x00,
	tChest    , 0x3C, 0xFF, 0x00, 0xE7, 0xFF, 0xFF, 0xFF, 0x00,
	tHouse1   , 0xC0, 0xDF, 0x9F, 0xBF, 0x3F, 0x7F, 0x7F, 0x00, // Town tiles
	tHouse2   , 0x07, 0xE3, 0xEB, 0xC9, 0xDD, 0x9C, 0xBE, 0x00, 
	tHouse3   , 0x7F, 0x7F, 0x7F, 0x4C, 0x4C, 0x7F, 0x7F, 0x00, 
	tHouse4   , 0x6D, 0x6D, 0x6D, 0x12, 0x6D, 0x6D, 0x6D, 0x00, 
	tHouseDoor, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 
	0x00 // null terminator
};

// Unused tile bitmaps
// 0x00, 0x00, 0x02, 0x04, 0x76, 0x54, 0x74, 0x00, // of (unused)
// 0x00, 0x04, 0x0E, 0x1F, 0x04, 0x04, 0x04, 0x00, // up arrow (unused)
// tLevel    , 0x00, 0x00, 0x40, 0x40, 0x45, 0x45, 0x72, 0x00, // Lv (Level)
// tHP       , 0x00, 0x00, 0x57, 0x55, 0x77, 0x54, 0x54, 0x00, // HP

// Sprite Data

const UInt8 cursorSprite1[] = { 0x1F, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F };
const UInt8 cursorSprite2[] = { 0xF8, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0xF8 };
const UInt8 tileSprites[] = { 
	0x66, 0x99, 0x81, 0x42, 0x42, 0x81, 0x99, 0x66, // Castle
	0xEE, 0x82, 0x82, 0x00, 0x82, 0x82, 0xEE, 0x00, // Town
	0x00, 0x2C, 0x60, 0x06, 0x50, 0x44, 0x10, 0x00, // Village
	0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, // Monument
	0x00, 0x00, 0x18, 0x24, 0x24, 0x24, 0x00, 0x00, // Cave
	0x3E, 0x3E, 0x22, 0x22, 0x22, 0x22, 0x22, 0x00, // House Door
};

const UInt8 temFaceSpriteHeight = 24;
const UInt8 temFaceSprite[] = {
	0x00, 0x38, 0x44, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x1E, 0x1E, 0x0C, 0x00, 0x00, 0x01, 0x00, 0x00, 0x04, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,  
	0x1C, 0x22, 0x00, 0x00, 0x00, 0x00, 0x06, 0x0F, 0x0F, 0x06, 0x00, 0x00, 0x00, 0x80, 0x00, 0x04, 0xE4, 0xF8, 0xE0, 0xE0, 0xE0, 0x40, 0x00, 0x00, 
};


const UInt8 selectionCursorSpriteHeight = 5;
const UInt8 selectionCursorSprite[] = { 0xC0, 0x78, 0x7F, 0x78, 0xC0 };

/*
const UInt8 sansSpriteOLD[] = {
	0x00, 0x01, 0x07, 0x07, 0x0F, 0x0F, 0x0F, 0x0C, 0x07, 0x05, 0x0C, 0x0E, 0x03, 0x00, 0x10, 0x36, 0x3B, 0x64, 0x7B, 0x79, 0x0B, 0x03, 0x0B, 0x08, 0x10, 0x10, 0x0F, 0x20, 0x7F, 0x7F, 0x00, 0x00, 
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFC, 0x6C, 0xC7, 0xFF, 0x00, 0xAA, 0x01, 0xFE, 0x00, 0xEE, 0x11, 0x7C, 0x7D, 0x39, 0x7D, 0x01, 0x01, 0x00, 0x10, 0x28, 0xEF, 0x00, 0xC7, 0x87, 0x00, 0x00, 
	0x00, 0x00, 0xC0, 0xC0, 0xE0, 0x60, 0x60, 0x60, 0xC0, 0x40, 0x60, 0xE0, 0x80, 0x00, 0x10, 0xD8, 0xB8, 0x4C, 0xBC, 0x3C, 0xA0, 0x80, 0xA0, 0x20, 0x10, 0x10, 0xE0, 0x18, 0x7C, 0xFC, 0x00, 0x00,
};
const SizeU8 sansSpriteSize = { 24, 32 };
*/

const UInt8 sansMegaSpriteLength = 32;
const UInt8 sansMegaSprite[] = {
	0x00, 0x00, 0x80, 0x80, 0x48, 0x48, 0x48, 0x48, 0x80, 0xC0, 0x48, 0x48, 0x80, 0xC0, 0xA4, 0x96, 0x1E, 0xD2, 0x1E, 0x1E, 0x2C, 0x48, 0x80, 0x00, 0x00, 0x80, 0x80, 0x80, 0x48, 0x2C, 0xC0, 0x00, 
	0x00, 0x07, 0x1F, 0x1F, 0x3F, 0x3F, 0x3F, 0x31, 0x1F, 0x17, 0x30, 0x3A, 0x1C, 0x07, 0x40, 0xDB, 0xEC, 0x91, 0xED, 0xF4, 0x6D, 0x2C, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1E, 0x7E, 0x00, 0x00, 
	0x00, 0xFC, 0xFF, 0xFF, 0xFF, 0xF1, 0xF1, 0xB1, 0x1F, 0xFD, 0x01, 0xAB, 0x06, 0xF8, 0x00, 0xBB, 0x46, 0xF1, 0xF6, 0xE5, 0xF6, 0x06, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00, 
	0x07, 0x18, 0x20, 0x20, 0x40, 0x40, 0x40, 0x4E, 0x20, 0x68, 0x4F, 0x45, 0x23, 0x78, 0xBF, 0x24, 0x13, 0x6E, 0x12, 0x0B, 0x92, 0x53, 0x33, 0x3F, 0x3F, 0x7F, 0x7F, 0x3F, 0x61, 0x81, 0x7E, 0x00, 
	0xFC, 0x03, 0x00, 0x00, 0x00, 0x0E, 0x0E, 0x4E, 0xE0, 0x02, 0xFE, 0x54, 0xF9, 0x07, 0xFF, 0x44, 0xB9, 0x0E, 0x09, 0x1A, 0x09, 0xF9, 0xF9, 0xFF, 0xFF, 0xFF, 0xBF, 0x1F, 0x10, 0x10, 0x1F, 0x00,  
};

// Raster Data

const UInt16 testImageLength = 495;
const UInt8 testImage[] = {
	0xED,0xD4,0x3D,0x6E,0xDB,0x30,0x14,0x07,0x70,0xD6,0x42,0x16,0x01,0xAD,0x96,0xA4,
	0x01,0x7A,0x82,0x22,0x47,0xF0,0xA2,0x2B,0x64,0xE8,0x53,0x00,0x01,0x4D,0xA6,0x2C,
	0xBC,0x43,0xD1,0x00,0xDD,0x3A,0x57,0xED,0x9A,0x85,0x80,0xF4,0x04,0xA4,0x9D,0x8B,
	0xA2,0xD1,0x25,0x9C,0x21,0x47,0xF0,0x64,0x74,0x34,0x12,0xBF,0x90,0x94,0x28,0x53,
	0x14,0x4D,0x79,0x0A,0x32,0xF4,0xBF,0xD8,0xC0,0xFB,0x49,0xFC,0x7A,0x14,0x63,0xFF,
	0xF3,0x8C,0x89,0xB1,0xCD,0xD5,0x2E,0x80,0xC3,0xBC,0x58,0x57,0xEE,0x02,0x2F,0x27,
	0x09,0x51,0x33,0x65,0xA2,0x87,0x49,0x97,0xAA,0x72,0x22,0xDD,0x0C,0x31,0xF4,0x2A,
	0xED,0x52,0x62,0x6A,0x8F,0x0E,0x03,0x2E,0x43,0x5C,0x49,0xD7,0x28,0xF7,0x2D,0xE0,
	0x00,0xF1,0x17,0x23,0x7A,0x60,0xD1,0x72,0x1F,0x47,0xD2,0x7D,0x0F,0xB9,0x73,0xE3,
	0x66,0x55,0xB6,0x8F,0x6B,0x66,0xC5,0xBE,0x2E,0x38,0xBF,0x8F,0x5C,0xAE,0x57,0xB9,
	0x03,0xC4,0xEC,0x3A,0xB0,0x0E,0x5E,0xAF,0x51,0xBA,0xA5,0xE0,0xE0,0xBA,0x44,0xCD,
	0x5B,0x07,0xF1,0x8C,0xE8,0xAE,0x75,0x00,0xB9,0xE3,0xDE,0x20,0xD6,0xC6,0x01,0x80,
	0xDC,0x6A,0x22,0xAC,0x20,0x47,0xC7,0xC5,0x23,0xD7,0x65,0xEC,0x4A,0xE3,0x06,0x71,
	0xE6,0x1E,0x7F,0x59,0x5E,0x6D,0x5D,0x60,0x91,0x91,0x71,0x13,0xF7,0xA7,0x77,0x13,
	0x49,0x96,0xC3,0xC1,0xDE,0xB3,0xA4,0xF1,0xB9,0x74,0x78,0x61,0x67,0x25,0x3B,0xC1,
	0xBF,0x9E,0x21,0xC8,0x72,0xA9,0x5C,0x97,0x72,0xBE,0x8F,0x81,0xE5,0x62,0x2C,0x53,
	0xFC,0xC3,0x8E,0x37,0x1E,0x17,0x91,0xD9,0x67,0xC6,0xE6,0x58,0xBE,0xC5,0x5B,0x76,
	0x6C,0x4E,0xD2,0x75,0x66,0xDE,0xF3,0xDF,0xE5,0xBB,0x9A,0x76,0xB8,0x0D,0xA2,0x71,
	0x47,0xB4,0x9A,0xDF,0x06,0xDC,0x87,0xAD,0x7B,0x7D,0xA1,0x5D,0xE3,0x73,0xD5,0xD0,
	0x35,0x96,0x8B,0xD1,0x72,0xC2,0x72,0xAF,0xCE,0x68,0x65,0xDC,0x89,0xDD,0x16,0xF5,
	0x02,0xE0,0xB4,0x77,0xFF,0x2A,0xA0,0xF5,0x0F,0xD9,0xA9,0x38,0x8A,0x74,0x5C,0xF7,
	0x09,0x62,0x41,0x6B,0x84,0xCF,0x1B,0xBF,0x2B,0x00,0xBE,0xEA,0x6E,0x53,0x8E,0xA4,
	0x5B,0x80,0xD7,0xDD,0x60,0x8D,0xBD,0x7B,0xE4,0xC5,0x27,0xB8,0x94,0xB7,0x63,0xEC,
	0xFA,0xAF,0xBB,0x7E,0x35,0x17,0x90,0x57,0x01,0x77,0xDD,0x39,0x14,0xE0,0xAD,0x53,
	0x97,0x55,0x3B,0x2E,0xA2,0xC8,0xBD,0xF5,0x81,0x13,0xB2,0xF0,0xF3,0xBE,0x13,0xB5,
	0x4B,0xB6,0x4E,0x3F,0xBF,0xF6,0xD6,0x2D,0x17,0xAC,0xEB,0x2C,0x2E,0xA6,0x1D,0xA8,
	0x9C,0x46,0x3B,0x1D,0xD8,0xD1,0x07,0xEC,0x8A,0x47,0x18,0xC5,0x72,0xE3,0xA2,0xEB,
	0x82,0x62,0x2F,0xD7,0xED,0xA8,0xE5,0xF2,0x22,0xE0,0x44,0xEB,0xD4,0xF1,0x2A,0x27,
	0x0F,0xAE,0x1A,0x30,0xEE,0xB8,0xAE,0x1B,0xF3,0x76,0x0C,0xCB,0xB5,0x8F,0x71,0xDB,
	0x89,0xBE,0xA7,0xC4,0xD6,0xB5,0x3F,0x59,0xE7,0xF4,0x34,0x2A,0x3D,0x36,0x0F,0x3A,
	0xF5,0xBF,0x75,0xEA,0x33,0x1D,0x74,0x50,0x99,0x35,0x67,0xCF,0xE6,0x9E,0x00
};


const UInt16 temShopImageLength = 1150;
const UInt8 temShopImage[] = {
	0x8D,0xD6,0x31,0x6B,0x23,0x47,0x14,0x00,0xE0,0xD9,0xAC,0xB6,0x31,0xF8,0x56,0x02,
	0xED,0x9F,0x70,0x95,0xDA,0xE0,0x68,0x16,0xAC,0x3A,0x23,0xD8,0xB7,0x42,0x22,0xB1,
	0x2A,0x1F,0x9C,0x2B,0x43,0x0A,0x81,0x73,0x89,0x0E,0x52,0xA7,0xB4,0x83,0x53,0x5D,
	0x63,0x13,0x8F,0x7C,0x38,0x8D,0x49,0x65,0xBC,0xF9,0x01,0x21,0x69,0x96,0xAB,0x36,
	0x6E,0x83,0x52,0x9C,0x8A,0x08,0x81,0x91,0xB2,0x79,0xF3,0x66,0x66,0x77,0xB5,0x76,
	0xE0,0xA6,0x90,0xAD,0xD5,0xA7,0x37,0xF3,0xDE,0xCC,0x3C,0x9B,0x01,0x03,0xF6,0x31,
	0x23,0xFA,0x38,0xE7,0xA0,0xE3,0x1E,0xE0,0x88,0xD4,0x0B,0x8C,0xFE,0xA1,0xC7,0x9D,
	0xE4,0x89,0x73,0x80,0x07,0x50,0x8C,0x83,0x9B,0x3F,0xF0,0xA9,0x3B,0x7B,0xCE,0x89,
	0xAA,0x93,0x77,0xC0,0xD9,0x67,0x32,0x71,0x70,0x06,0xC6,0x8A,0x4F,0xC8,0x79,0x82,
	0xA9,0x47,0xEA,0x7B,0x2F,0xE4,0x34,0x05,0x98,0xC9,0xD4,0x7C,0xDE,0x35,0x3F,0x43,
	0xE1,0xF5,0x44,0xA0,0x9C,0xCE,0x67,0x57,0xCA,0x05,0xF4,0x7F,0xDA,0xB1,0xAE,0xB5,
	0xE1,0x78,0xB1,0x8E,0xDD,0xB7,0x3B,0xD7,0x70,0xF0,0x76,0x8B,0x5C,0x88,0xCE,0x2C,
	0x0F,0x5D,0xC8,0x83,0x72,0xBD,0xBB,0x6F,0x3E,0x99,0xE2,0xCB,0x16,0xE5,0xC1,0x43,
	0x11,0xB2,0x88,0x3E,0xF5,0x78,0x50,0x73,0x0C,0x1D,0x23,0x17,0xF0,0x96,0x00,0x1E,
	0x85,0xD6,0x61,0xAD,0xAB,0x6E,0x27,0xF9,0x4B,0x3B,0x81,0x2E,0xE0,0xD0,0x50,0xCF,
	0x03,0x16,0x38,0x35,0xB7,0x65,0x1D,0x67,0x21,0xA6,0x08,0x42,0x3B,0x4C,0x53,0x6C,
	0xB8,0x4E,0x9E,0x97,0xCE,0xEB,0xE9,0x20,0x58,0x3E,0xE6,0x54,0xDC,0x8B,0x37,0x1D,
	0x4C,0x75,0x55,0x3A,0xB0,0x2E,0xDA,0x74,0xDF,0xA9,0x8A,0xF4,0xFF,0xD6,0x73,0x85,
	0xC2,0x29,0x1C,0xC7,0x5C,0x2A,0xEE,0x7B,0x2A,0xEB,0xD7,0xF4,0x06,0xF3,0x70,0xF4,
	0xF2,0x98,0xF0,0x6A,0xEE,0x16,0xA3,0x01,0x7C,0x61,0x1D,0x33,0x8E,0x23,0xAA,0x6C,
	0x07,0x73,0x53,0x18,0xFD,0x99,0x1A,0xD7,0xC4,0x8D,0x07,0xF3,0x21,0xD6,0x18,0xAA,
	0x0E,0xE0,0xE8,0xF1,0x16,0xFA,0x85,0x0B,0x4A,0x57,0x29,0x9F,0x72,0x52,0x5E,0x60,
	0xC6,0xEA,0x4D,0xA3,0xE2,0x22,0x75,0xA4,0xCB,0xB1,0x0D,0xB1,0x94,0x57,0x7A,0x81,
	0x1E,0x16,0x8D,0x06,0xA3,0x73,0xEF,0x6D,0xBA,0x21,0xB9,0x63,0xFC,0x1D,0x4F,0xA7,
	0x76,0x9C,0x6A,0x17,0x54,0x5C,0xBE,0x56,0x4E,0x02,0x24,0xE4,0x8A,0xF3,0xAC,0x6A,
	0x52,0x49,0xC3,0x97,0xA7,0x70,0xA4,0xDC,0x68,0xAE,0xEE,0x56,0xC8,0x1A,0x34,0xA1,
	0x9E,0xBB,0x70,0x1E,0x9E,0xE6,0x53,0x4C,0x43,0xCA,0xD3,0xE1,0x0D,0x6E,0x4B,0xE4,
	0x95,0x0E,0x67,0x56,0xAF,0x51,0xE0,0xE1,0xE1,0x8D,0x66,0x85,0x93,0x58,0xEF,0xA8,
	0xE1,0x34,0x74,0xA2,0x1E,0x56,0xBB,0x72,0xD9,0x94,0x3B,0x30,0xEE,0x9A,0x05,0xC2,
	0xD3,0x29,0xAA,0xBD,0xC3,0x82,0x97,0xA3,0x8F,0xE2,0x5B,0xE5,0x2E,0x62,0x72,0xB6,
	0x62,0x98,0x29,0x40,0xCD,0x1D,0x18,0x37,0x65,0x41,0x71,0x8C,0xB4,0x33,0xB7,0xC3,
	0x21,0x07,0xE4,0xAE,0x40,0x39,0x9B,0x9F,0xCA,0x02,0xC0,0x56,0x0F,0x2B,0xE9,0x62,
	0x1A,0x27,0xDA,0xC5,0x53,0x15,0x42,0x78,0x7A,0xA2,0x62,0x75,0xC2,0xB8,0x5B,0xEB,
	0x0E,0xA6,0xD5,0x25,0x29,0xD7,0x53,0xE1,0x9B,0xE4,0xD8,0xEC,0x1D,0xB6,0x0C,0x72,
	0x27,0x15,0x27,0x68,0xEE,0x50,0xB5,0x10,0xEB,0xA6,0x29,0xB9,0x29,0x9C,0x5C,0xAB,
	0xC2,0x53,0x2B,0xA1,0x75,0x46,0x9E,0xD3,0x8D,0xA0,0xDB,0xD6,0x6E,0x57,0xDE,0x92,
	0x93,0xF9,0x63,0xDD,0x39,0xE8,0x98,0xD7,0xB0,0xEE,0x5E,0xBB,0x5F,0x4E,0x7E,0xAE,
	0xBB,0x30,0x14,0xCC,0xD9,0xD7,0xCE,0x9F,0x19,0x77,0xF7,0x98,0x50,0xA9,0xA8,0x2C,
	0x9C,0x8E,0x33,0xEF,0x89,0x66,0xD8,0xD5,0x8E,0x75,0xD6,0x26,0xDE,0x07,0x66,0x9D,
	0x53,0x3A,0x6C,0x5B,0xC6,0xF9,0x4B,0x13,0x6F,0xC5,0x2C,0xD1,0x9B,0x47,0xAE,0xC1,
	0x7A,0xC6,0x6D,0x67,0x26,0xDE,0x31,0xB3,0x53,0x5A,0x17,0x0A,0xB1,0xCF,0xB1,0x88,
	0x8E,0xF0,0xF3,0x95,0x3F,0xA6,0x3A,0xCB,0xF5,0xDC,0x5C,0x1E,0xF3,0x7D,0x74,0xDD,
	0x48,0x04,0xD8,0xC2,0xF0,0x7D,0x27,0x5F,0x6D,0x67,0x8F,0xE4,0x96,0xC7,0xC5,0x19,
	0x27,0x87,0x87,0x00,0xDA,0xF8,0x4B,0xDB,0x38,0x7F,0x4C,0xE7,0x6A,0x9A,0xCD,0x59,
	0x75,0x38,0x80,0x7D,0xB0,0x1D,0x59,0x07,0xAF,0xDD,0xEC,0x41,0xB9,0x9B,0xB1,0x2A,
	0x8B,0x4B,0xB5,0xD1,0x27,0x06,0x9D,0x3E,0xD0,0x42,0xBB,0xF1,0x58,0x25,0x72,0x97,
	0x29,0xE2,0xAF,0x88,0xA8,0x3F,0x2F,0x91,0xC7,0xCD,0x46,0x37,0xD1,0xA5,0x18,0x2F,
	0xFB,0x17,0xA7,0x5D,0x52,0xBC,0xBD,0x55,0x00,0x42,0x3B,0x6C,0x43,0x6D,0x7D,0x1C,
	0xD1,0xF9,0xE9,0x04,0xE3,0x4D,0x1E,0x65,0x9E,0x51,0xBC,0xC5,0x04,0x4F,0x13,0xDE,
	0x75,0x75,0x8B,0xAA,0xCE,0x5D,0x4C,0xDC,0x6C,0x9D,0xE5,0xF9,0x72,0x3C,0x56,0xCB,
	0x4B,0x73,0x74,0xB8,0xF9,0x74,0x67,0x6B,0x6E,0xFC,0xFB,0x38,0xCF,0xD7,0xD9,0x43,
	0xE7,0x57,0xF8,0xBC,0xBF,0xA2,0x2A,0x07,0x1B,0x0E,0x17,0xD1,0xC7,0x89,0xB2,0xDF,
	0x28,0xDE,0x72,0x76,0x07,0xDF,0x8C,0x12,0xBA,0x35,0x50,0x77,0xB0,0x98,0xA4,0x0F,
	0xEF,0x73,0x35,0x26,0xF7,0x1F,0x92,0xBD,0x11,0xD3,0x2D,0x03,0xC8,0x79,0xA5,0x5B,
	0x8F,0xD2,0x2F,0xD3,0xAF,0xF2,0x7C,0xB5,0x37,0x92,0xD7,0xC9,0xE2,0x35,0x5D,0x35,
	0x1D,0xB1,0x70,0x4D,0x55,0x88,0xD1,0xDC,0x4F,0x5C,0x74,0x9F,0xBA,0xF2,0x26,0x49,
	0xE7,0xAA,0xE5,0xE1,0x95,0xEC,0x52,0xBE,0x9E,0x28,0x5C,0x67,0x34,0x77,0xC9,0xE1,
	0x4D,0xB9,0x77,0xDF,0x27,0xB6,0x35,0xE2,0x0F,0xED,0xA8,0x1F,0x46,0xCC,0xEF,0xE3,
	0xB6,0xFA,0xB9,0x1D,0xAC,0x68,0x8D,0xE4,0xCC,0x4A,0x59,0x1B,0xC9,0xAA,0x53,0x28,
	0x15,0x75,0xC3,0xF1,0xA6,0x7E,0xD3,0xB0,0xA1,0xD6,0xBA,0xD9,0xCC,0x55,0x72,0xD1,
	0xB3,0x6E,0x99,0x2F,0xCA,0xA6,0x34,0xD7,0x45,0xC0,0x34,0x75,0x1E,0xA5,0x73,0xD7,
	0xD5,0xE6,0x35,0xB7,0xE5,0x03,0x72,0x0E,0x6F,0xE9,0xCE,0x8B,0x77,0xB8,0xEE,0x98,
	0xF9,0x7F,0x83,0x93,0x73,0x78,0xD5,0xF5,0x73,0x9A,0x7C,0xF8,0x83,0x72,0x01,0x37,
	0x1B,0xF2,0x34,0x1E,0xB9,0x41,0x7C,0x6E,0x1C,0x4D,0x8E,0x2F,0xC2,0xB3,0x6D,0xB3,
	0x9C,0x37,0xBE,0x1C,0x0C,0x7E,0x2C,0x1C,0xDD,0x4C,0xE5,0x6C,0x17,0x27,0x77,0x74,
	0x39,0x84,0x61,0x3C,0x78,0x75,0x4E,0xEB,0xF3,0xAC,0x73,0xF0,0x5E,0x18,0x87,0x4F,
	0x72,0x18,0x1E,0xC6,0x12,0x67,0x3D,0x3A,0x34,0x4E,0x3C,0xEF,0xE2,0xAB,0x57,0xF1,
	0x25,0x0C,0xCF,0xCE,0x5F,0x9E,0x95,0xAE,0x25,0xEA,0xAE,0x03,0x87,0xC3,0xB3,0x97,
	0x00,0x67,0xF1,0x51,0xAC,0x5D,0xC4,0x59,0xA3,0xFB,0x9C,0x1B,0xC4,0x31,0xC6,0x3B,
	0x33,0xF5,0x73,0x7A,0x2D,0xA7,0x15,0xFC,0x9F,0x03,0x38,0xB5,0x2E,0xDA,0xDF,0xDF,
	0xF7,0x9E,0x9D,0xF7,0xA2,0xBA,0x1F,0xF8,0xEF,0x61,0x8B,0x8A,0x53,0x77,0x83,0xCD,
	0x7D,0xA3,0xA6,0xF6,0xD4,0xE5,0xF5,0xFD,0xA5,0xCE,0xA6,0xB6,0xEB,0x3F,
};



