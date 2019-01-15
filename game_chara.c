// game_chara.c

#include "game_chara.h"
#include "text.h"


// Globals
GameChara partyChara[4];
UInt8 partySize = 4;
UInt8 partyPotions;
UInt16 partyMoney;
UInt16 partyFangs;
SInt16 partyReputation;


// Chara Stats

static UInt8 maxHpWithCharaLevel(UInt8 level) {
	UInt16 hp = 10 + 10 * level;
	if (hp > 255) {
		hp = 255;
	}
	return hp;
}

static UInt16 maxXpWithCharaLevel(UInt8 level) {
	UInt16 xp = 75 + 25 * level;
	return xp;
}

// Battle 

UInt8 calculateDamage(UInt8 attack, UInt8 defense) {
	UInt16 def = defense + 256;
	UInt16 dmg = attack * 256 / def; 
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
	GameChara *chara = &partyChara[charaIndex];
	UInt8 didLevelUp = 0;
	
	chara->xp += xp;
	if (chara->xp >= chara->maxXp) {
		if (chara->level >= maxCharaLevel) {
			chara->xp = chara->maxXp;
		} else {
			UInt8 lv = chara->level;
			chara->xp -= chara->maxXp;
			chara->maxHp = maxHpWithCharaLevel(lv);
			chara->maxXp = maxXpWithCharaLevel(lv);
			didLevelUp = 1;
		}
	}
	return didLevelUp;
}

UInt8 charaAttackRating(UInt8 charaIndex) {
	GameChara *chara = &partyChara[charaIndex];
	UInt8 attack = chara->baseAttack + chara->level;
	return attack;
}

UInt8 charaDefenseRating(UInt8 charaIndex) {
	GameChara *chara = &partyChara[charaIndex];
	UInt8 defense = chara->baseDefense + chara->level;
	return defense;
}

void levelUpChara(UInt8 charaIndex) {
	UInt8 level;

	GameChara *chara = &partyChara[charaIndex];
	if (chara->level < 25) {
		// Max level is 25
		level = chara->level + 1;
		chara->level = level;
		chara->maxHp = maxHpWithCharaLevel(chara->level);
		chara->maxXp = maxXpWithCharaLevel(chara->level);
	}

}

void initChara(GameChara *chara, UInt8 *name, UInt8 level, UInt8 atk, UInt8 def) {
	chara->name = name;
	chara->level = level;
	chara->hp = maxHpWithCharaLevel(level);
	chara->maxHp = maxHpWithCharaLevel(level);
	chara->xp = 0;
	chara->maxXp = maxXpWithCharaLevel(level);
	chara->baseAttack = atk;
	chara->baseDefense = def;
	chara->weapon = 0;
	chara->armor = 0;
	chara->shield = 0;
}

void initCharaAtIndex(UInt8 index, UInt8 *name, UInt8 level, UInt8 atk, UInt8 def) {
	GameChara *chara = &partyChara[index];
	initChara(chara, name, level, atk, def);
}

void initParty(void) {
	initCharaAtIndex(0, "Kim", 25, 8, 9);
	initCharaAtIndex(1, "Sam", 3, 6, 12);
	initCharaAtIndex(2, "Jony", 2, 6, 5);
	initCharaAtIndex(3, "Frisk", 1, 10, 8);

	// Testing
	// partyChara[1].hp = 18;
	// partyChara[2].hp = 25;
}
