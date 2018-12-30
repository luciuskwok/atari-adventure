// game_chara.c

#include "game_chara.h"
#include "text.h"


// Globals
UInt8 partySize = 4;
GameChara party[4];
UInt32 partyMoney;
UInt8 partyPotions;
UInt8 partyFangs;
SInt8 partyReputation;

// Chara Stats

void initChara(GameCharaPtr outChara, UInt8 *name, UInt8 level, UInt8 atk, UInt8 def) {
	stringCopy(outChara->name, name);
	outChara->level = level;
	outChara->xp = 0;
	outChara->hp = charaMaxHp(outChara);
	outChara->baseAttack = atk;
	outChara->baseDefense = def;
	outChara->weapon = NULL;
	outChara->armor = NULL;
	outChara->shield = NULL;
}

UInt8 charaMaxHp(GameCharaPtr chara) {
	if (chara->level > 24) {
		return 255;
	} else {
		return 10 + 10 * chara->level;
	}
}

UInt8 charaAttackRating(GameCharaPtr chara) {
	UInt8 attack = chara->baseAttack + chara->level;
	if (chara->weapon != NULL) {
		attack += chara->weapon->attack;
	}
	return attack;
}

UInt8 charaDefenseRating(GameCharaPtr chara) {
	UInt8 defense = chara->baseDefense + chara->level;
	if (chara->shield != NULL) {
		defense += chara->shield->defense;
	}
	if (chara->armor != NULL) {
		defense += chara->armor->defense;
	}
	return defense;
}

// Party Stats
UInt8 numberInParty(void) {
	return partySize;
}

GameCharaPtr charaAtIndex(UInt8 index) {
	return &party[index];
}

void initParty(void) {
	initChara(&party[0], "Alisa", 8, 8, 9);
	initChara(&party[1], "Nyorn", 1, 6, 12);
	initChara(&party[2], "Marisa", 4, 6, 5);
	initChara(&party[3], "Guy", 1, 10, 8);

	// Testing
	party[1].hp = 18;
	party[2].hp = 25;
}

