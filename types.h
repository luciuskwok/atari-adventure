// types.h

#ifndef TYPES_H
#define TYPES_H

#include <stddef.h>

typedef unsigned char UInt8;
typedef unsigned int UInt16;
typedef unsigned long UInt32;
typedef signed char SInt8;
typedef signed int SInt16;
typedef signed long SInt32;

typedef struct PointU8 {
	UInt8 x, y;
} PointU8;

typedef struct SizeU8 {
	UInt8 width, height;
} SizeU8;

typedef struct RectU8 {
	PointU8 origin;
	SizeU8 size;
} RectU8;

typedef struct DeflatedImage {
	UInt16 length;
	UInt8 data[];
} DeflatedImage;

#endif
