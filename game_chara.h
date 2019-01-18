// game_chara.h

#include "types.h"

typedef struct PlayerChara {
	UInt8 *name;
	UInt8 level;
	UInt8 hp;
	UInt8 maxHp;
	UInt16 xp;
	UInt16 maxXp;
	UInt8 attack;
	UInt8 defense;
	UInt8 weapon;
	UInt8 armor; 
	UInt8 shield;
	UInt8 padding[2];
} PlayerChara; // 16 bytes

typedef struct EnemyChara {
	UInt8 *name;
	UInt8 hp;
	UInt8 maxHp;
	UInt8 attack;
	UInt8 defense;
} EnemyChara; 

enum Equipment {
	equip_none = 0,
	equip_knife = 1,
	equip_basic = 2,
	equip_legendary = 3,
	equip_ultimate = 4
};

// Globals
extern PlayerChara partyChara[];
extern UInt8 partySize;
extern UInt8 partyPotions;
extern UInt16 partyMoney;
extern UInt16 partyFangs;
extern SInt16 partyReputation;

// Constants
#define maxCharaLevel (16)


// Battle 
UInt8 calculateDamage(UInt8 attack, UInt8 defense);
UInt8 calculateXpGain(UInt8 playerLevel, UInt8 enemyLevel);
UInt8 addExperienceToChara(UInt8 charaIndex, UInt8 xp);

void recalculateAttackDefense(UInt8 charaIndex);

// Init
void initEnemyChara(PlayerChara *chara, UInt8 *name, UInt8 maxHp, UInt8 atk, UInt8 def);
void initChara(PlayerChara *chara, UInt8 *name, UInt8 level);
void initParty(void);

