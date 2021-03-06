// game_chara.c

#include "game_chara.h"
#include "text.h"


// Globals
PlayerChara partyChara[4];
UInt8 partySize = 4;
UInt8 partyPotions;
UInt16 partyMoney;
UInt16 partyFangs;
SInt16 partyReputation;


// ATK lookup table
const UInt8 baseAttackTable[] = {
	 3,  4,  5,  6,  7,  8, 10, 12, 
	14, 16, 19, 22, 25, 28, 32, 36
};


// Chara Stats

static UInt8 maxHpWithCharaLevel(UInt8 level) {
	UInt16 hp = (level + 1) * 8;
	if (hp > 255) {
		hp = 255;
	}
	return hp;
}

static UInt16 maxXpWithCharaLevel(UInt8 level) {
	UInt16 xp = level * (8 * 16);
	return xp;
}

// Battle 

UInt8 calculateDamage(UInt8 attack, UInt8 defense) {
	UInt16 def = defense + 16;
	UInt16 dmg = (attack * 16 + def - 1) / def; // = ceil(atk * 16 / def), but for ints
	return dmg;
}

UInt8 calculateXpGain(UInt8 playerLevel, UInt8 enemyLevel) {
	UInt8 xpg = 8;

	if (playerLevel >= enemyLevel) {
		if (playerLevel >= enemyLevel + 16) {
			xpg = 0;
		} else {
			xpg -= (playerLevel - enemyLevel) / 2;
		}
	} else {
		xpg += (enemyLevel - playerLevel) / 4;
	}
	return xpg;
}

UInt8 addExperienceToChara(UInt8 charaIndex, UInt8 xp) {
	PlayerChara *chara = &partyChara[charaIndex];
	UInt8 didLevelUp = 0;
	UInt8 lvl = chara->level;
	
	chara->xp += xp;
	if (chara->xp >= chara->maxXp) {
		if (lvl >= maxCharaLevel) {
			chara->xp = chara->maxXp;
		} else {
			++lvl;
			chara->level = lvl;
			chara->xp -= chara->maxXp;
			chara->maxHp = maxHpWithCharaLevel(lvl);
			chara->maxXp = maxXpWithCharaLevel(lvl);
			recalculateAttackDefense(charaIndex);
			didLevelUp = 1;
		}
	}
	return didLevelUp;
}

// Recalculates attack and defenese attributes based on level and equipment.
void recalculateAttackDefense(UInt8 charaIndex) {
	PlayerChara *chara = &partyChara[charaIndex];
	UInt8 lvl = chara->level;
	UInt8 atk = baseAttackTable[lvl - 1];
	UInt8 def = (lvl * 7 + 1) / 4 - 1;

	// Add weapon stat
	if (chara->weapon == equip_knife) {
		atk += 1;
	} else if (chara->weapon == equip_basic) {
		atk += 3;
	} else if (chara->weapon == equip_legendary) {
		atk += 6;
	} else if (chara->weapon == equip_ultimate) {
		atk += 10;
	}
	chara->attack = atk;

	// Add armor stat 
	if (chara->armor == equip_basic) {
		def += 5;
	} else if (chara->armor == equip_legendary) {
		def += 10;
	}

	// Add shield stat 
	if (chara->shield == equip_basic) {
		def += 5;
	} else if (chara->shield == equip_legendary) {
		def += 10;
	}
	chara->defense = def;
}

void initEnemyChara(PlayerChara *chara, UInt8 *name, UInt8 maxHp, UInt8 atk, UInt8 def) {
	chara->name = name;
	chara->hp = maxHp;
	chara->maxHp = maxHp;
	chara->attack = atk;
	chara->defense = def;

	chara->level = 0;
	chara->xp = 0;
	chara->maxXp = 0;
	chara->weapon = 0;
	chara->armor = 0;
	chara->shield = 0;
}

void initCharaAtIndex(UInt8 index, UInt8 *name, UInt8 level) {
	PlayerChara *chara = &partyChara[index];

	chara->name = name;
	chara->level = level;
	chara->hp = maxHpWithCharaLevel(level);
	chara->maxHp = maxHpWithCharaLevel(level);
	chara->xp = 0;
	chara->maxXp = maxXpWithCharaLevel(level);
	chara->weapon = 0;
	chara->armor = 0;
	chara->shield = 0;

	recalculateAttackDefense(index);
}

void initParty(void) {
	initCharaAtIndex(0, "Kim", maxCharaLevel);
	initCharaAtIndex(1, "Sam", maxCharaLevel-1);
	initCharaAtIndex(2, "Jony", 2);
	initCharaAtIndex(3, "Frisk", 1);

	// Testing
	// partyChara[1].hp = 18;
	// partyChara[2].hp = 25;
}
