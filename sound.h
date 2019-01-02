// sound.h

#include "types.h"

void noteOn(UInt8 note, UInt8 volume, UInt8 channel);
void noteOff(UInt8 channel);

void stopSound(void);
void startSequence(void);

void initSound(void);


