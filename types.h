// types.h

#ifndef TYPES_H
#define TYPES_H

typedef unsigned char UInt8;
typedef unsigned int UInt16;
typedef unsigned long UInt32;
typedef signed char SInt8;
typedef signed int SInt16;
typedef signed long SInt32;

typedef struct {
	UInt8 x, y;
} PointU8;

typedef struct {
	UInt8 width, height;
} SizeU8;

#define NULL (0)

#endif