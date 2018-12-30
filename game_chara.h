// game_chara.h

#include "types.h"

typedef struct GameEquipment* GameEquipmentPtr;
typedef struct GameEquipment {
	UInt8 name[9];
	UInt8 attack;
	UInt8 defense;
} GameEquipment;

typedef struct GameChara* GameCharaPtr;
typedef struct GameChara {
	UInt8 name[16];
	UInt8 level;
	UInt8 xp;
	UInt8 hp;
	UInt8 baseAttack;
	UInt8 baseDefense;
	GameEquipmentPtr weapon;
	GameEquipmentPtr shield;
	GameEquipmentPtr armor;
} GameChara;


// Character Stats
UInt8 charaMaxHp(GameCharaPtr chara);
UInt8 charaAttackRating(GameCharaPtr chara);
UInt8 charaDefenseRating(GameCharaPtr chara);

// Party Stats
UInt8 numberInParty(void);
GameCharaPtr charaAtIndex(UInt8 index);
void initParty(void);

