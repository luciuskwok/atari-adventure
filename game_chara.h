// game_chara.h

#include "types.h"

typedef struct GameChara* GameCharaPtr;
typedef struct GameChara {
	UInt8 name[9];
	UInt8 level;
	UInt8 hp;
	UInt16 xp;
	UInt8 baseAttack;
	UInt8 baseDefense;
	UInt8 equipment; // bit field:
		// bits 0-1: weapon
		// bits 2-3: armor
		// bits 3-4: shield

} GameChara; // 16 bytes

// Globals
extern GameChara partyChara[];
extern UInt8 partySize;
extern UInt8 partyPotions;
extern UInt16 partyMoney;
extern UInt16 partyFangs;
extern SInt16 partyReputation;



// Character Stats
UInt8 charaAttackRating(UInt8 charaIndex);
UInt8 charaDefenseRating(UInt8 charaIndex);
UInt16 charaXpToNextLevel(UInt8 charaIndex);

// Party Stats
void initParty(void);

// In misc.asm (temporarily)
UInt8 maxHpWithCharaLevel(UInt8 level);
