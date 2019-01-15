// game_chara.h

#include "types.h"

typedef struct GameChara {
	UInt8 *name;
	UInt8 level;
	UInt8 hp;
	UInt8 maxHp;
	UInt16 xp;
	UInt16 maxXp;
	UInt8 baseAttack;
	UInt8 baseDefense;
	UInt8 weapon;
	UInt8 armor; 
	UInt8 shield;
	UInt8 padding[2];
} GameChara; // 16 bytes

// Globals
extern GameChara partyChara[];
extern UInt8 partySize;
extern UInt8 partyPotions;
extern UInt16 partyMoney;
extern UInt16 partyFangs;
extern SInt16 partyReputation;

// Constants
#define maxCharaLevel (25)


// Battle 
UInt8 calculateDamage(UInt8 attack, UInt8 defense);
UInt8 calculateXpGain(UInt8 playerLevel, UInt8 enemyLevel);
UInt8 addExperienceToChara(UInt8 charaIndex, UInt8 xp);

UInt8 charaAttackRating(UInt8 charaIndex);
UInt8 charaDefenseRating(UInt8 charaIndex);

// Init
void initChara(GameChara *chara, UInt8 *name, UInt8 level, UInt8 atk, UInt8 def);
void initParty(void);

