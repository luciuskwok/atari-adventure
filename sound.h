// sound.h

#include "types.h"

void __fastcall__ noteOn(UInt8 note, UInt8 duration, UInt8 volume, UInt8 envelope, UInt8 noise, UInt8 channel);
void __fastcall__ noteOff(UInt8 channel);
void __fastcall__ startSong(UInt8 song);
void __fastcall__ stopSong(void);

enum NotesOctaves {
	NoteC  = 1,
	NoteDb = 2,
	NoteD  = 3,
	NoteEb = 4,
	NoteE  = 5,
	NoteF  = 6,
	NoteGb = 7,
	NoteG  = 8,
	NoteAb = 9,
	NoteA  = 10,
	NoteBb = 11,
	NoteB  = 12,
	Oct3   = 0,
	Oct4   = 12,
	Oct5   = 24,
	Oct6   = 36,
	Oct7   = 48  // Only C7 exists. Do not use above C7.
};
