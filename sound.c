// sound.c

/*
	Audio Formula

	Freq = Clock / (2*(1+AUDF)) for 64 kHz or 16 kHz clocks.
	Freq = Clock / (7*(1+AUDF)) for 1.79 MHz clock.

	Clock / Freq = 2*(1+AUDF)
	Clock / (Freq * 2) = 1 + AUDF
	AUDF = (Clock / (Freq * 2)) - 1

*/

#include "sound.h"
#include "atari_memmap.h"

typedef struct ChannelState {
	UInt8 frequency; 		// clock frequency divider value
	UInt8 vibrato;			// quickly switches between two frequencies
	UInt8 currentLevel;		// current volume level
	UInt8 attackTime;		// countdown of tick count for attack
	UInt8 attackRate;		// 0 = instant attack; 1 = incremental attack
	UInt8 decayRate;
	UInt8 sustainLevel;
	UInt8 sustainTime;
	UInt8 releaseTime; 		// should be releaseRate?
} ChannelState;

typedef struct SoundState {
	UInt8 vibratoTimer;
	ChannelState channelState[4];
} SoundState;

typedef struct NoteFreqAdjust {
	UInt8 audf;
	UInt8 vibrato;
} NoteFreqAdjust;

// Globals
struct SoundState soundState;

// Data
const NoteFreqAdjust noteTable[] = {
	{ 255, 0 }, // C3
	{ 240, 5 }, // C#
	{ 227, 1 }, // D
	{ 214, 2 }, // D#
	{ 202, 1 }, // E
	{ 190, 6 }, // F
	{ 180, 0 }, // F#
	{ 169, 7 }, // G
	{ 160, 2 }, // G#
	{ 151, 2 }, // A
	{ 142, 5 }, // A#
	{ 134, 5 }, // B
	{ 127, 0 }, // C4
	{ 119, 7 }, // C#
	{ 113, 1 }, // D
	{ 106, 5 }, // D#
	{ 100, 5 }, // E
	{  94, 7 }, // F
	{  89, 4 }, // F#
	{  84, 3 }, // G
	{  79, 5 }, // G#
	{  75, 1 }, // A
	{  70, 7 }, // A#
	{  66, 6 }, // B
	{  63, 0 }, // C5
	{  59, 3 }, // C#
	{  56, 0 }, // D
	{  52, 7 }, // D#
	{  49, 6 }, // E
	{  47, 0 }, // F
	{  44, 2 }, // F#
	{  41, 6 }, // G
	{  39, 3 }, // G#
	{  37, 0 }, // A
	{  34, 7 }, // A#
	{  32, 7 }, // B
	{  31, 0 }, // C6
	{  29, 2 }, // C#
	{  27, 4 }, // D
	{  25, 7 }, // D#
	{  24, 3 }, // E
	{  23, 0 }, // F
	{  21, 5 }, // F#
	{  20, 3 }, // G
	{  19, 1 }, // G#
	{  18, 0 }, // A
	{  17, 0 }, // A#
	{  16, 0 }, // B
};

void noteOn(UInt8 note, UInt8 volume, UInt8 channel) {
	UInt8 audf = noteTable[note].audf;
	UInt8 vibrato = noteTable[note].vibrato;
	//UInt8 ctrl = (0xE0) | (volume & 0x0F);
	ChannelState *channelPtr = &soundState.channelState[channel];

	channelPtr->frequency = audf;
	channelPtr->vibrato = vibrato;
	channelPtr->currentLevel = 0;

	channelPtr->sustainTime = 60;
	channelPtr->sustainLevel = volume;
	channelPtr->decayRate = 1;
	channelPtr->attackRate = 1;
	channelPtr->attackTime = 15;
}

void noteOff(UInt8 channel) {
	ChannelState *channelPtr = &soundState.channelState[channel];

	channelPtr->frequency = 0;
	channelPtr->vibrato = 0;
	channelPtr->currentLevel = 0;

	channelPtr->sustainTime = 0;
	channelPtr->sustainLevel = 0;
	channelPtr->decayRate = 1;
	channelPtr->attackRate = 1;
	channelPtr->attackTime = 0;
}

void stopSound(void) {
	UInt8 i;
	for (i=0; i<4; ++i) {
		noteOff(i);
	}
		for (i=0; i<8; ++i) {
		POKE(AUDF1 + i, 0);
	}
}

void initSound(void) {
	stopSound();
	POKE(AUDCTL, 0);
	POKE(SKCTL, 3);
}
