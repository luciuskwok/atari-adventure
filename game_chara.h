// game_chara.h

#include "types.h"

typedef struct GameChara* GameCharaPtr;
typedef struct GameChara {
	UInt8 *name;
	UInt8 level;
	UInt8 hp;
	UInt8 maxHp;
	UInt16 xp;
	UInt8 baseAttack;
	UInt8 baseDefense;
	UInt8 weapon;
	UInt8 armor; 
	UInt8 shield;
	UInt8 padding[4];
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
