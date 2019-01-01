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
	UInt8 attackRate;		// 
	UInt8 decayRate;
	UInt8 sustainLevel;
	UInt8 sustainTime;
	UInt8 releaseRate; 
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

enum Envelopes {
	BasicEnvelope, 
	Envelope1
};

static void applyEnvelope(ChannelState *ch, UInt8 envelope, UInt8 sustainLevel, UInt8 sustainTime) {
	switch (envelope) {
		case Envelope1:
			ch->currentLevel = 0;
			ch->releaseRate = 1;
			ch->sustainTime = sustainTime;
			ch->sustainLevel = sustainLevel;
			ch->decayRate = 1;
			ch->attackRate = 8;
			ch->attackTime = 2;
			break;

		case BasicEnvelope:
		default:
			ch->currentLevel = 0;
			ch->releaseRate = 1;
			ch->sustainTime = sustainTime;
			ch->sustainLevel = sustainLevel;
			ch->decayRate = 1;
			ch->attackRate = 1;
			ch->attackTime = 15;
			break;
	}
}

void noteOn(UInt8 note, UInt8 volume, UInt8 channel) {
	UInt8 audf = noteTable[note].audf;
	UInt8 vibrato = noteTable[note].vibrato;
	//UInt8 ctrl = (0xE0) | (volume & 0x0F);
	ChannelState *ch = &soundState.channelState[channel];

	ch->frequency = audf;
	ch->vibrato = vibrato;
	applyEnvelope(ch, Envelope1, volume, 60);
}

void noteOff(UInt8 channel) {
	ChannelState *ch = &soundState.channelState[channel];

	ch->releaseRate = 1;
	ch->sustainTime = 0;
	ch->sustainLevel = 0;
	ch->decayRate = 1;
	ch->attackRate = 1;
	ch->attackTime = 0;
}

void stopSound(void) {
	UInt8 i;
	for (i=0; i<4; ++i) {
		noteOff(i);
	}
}

void initSound(void) {
	stopSound();
	POKE(AUDCTL, 0);
	POKE(SKCTL, 3);
}
