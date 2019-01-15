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

void initCharaAtIndex(UInt8 index, UInt8 *name, UInt8 level, UInt8 atk, UInt8 def) {
	GameCharaPtr chara = &partyChara[index];

	chara->name = name;
	chara->level = level;
	chara->hp = maxHpWithCharaLevel(level);
	chara->maxHp = maxHpWithCharaLevel(level);
	chara->xp = 0;
	chara->baseAttack = atk;
	chara->baseDefense = def;
	chara->weapon = 0;
	chara->armor = 0;
	chara->shield = 0;
}

UInt8 charaAttackRating(UInt8 charaIndex) {
	GameCharaPtr chara = &partyChara[charaIndex];
	UInt8 attack = chara->baseAttack + chara->level;

	return attack;
}

UInt8 charaDefenseRating(UInt8 charaIndex) {
	GameCharaPtr chara = &partyChara[charaIndex];
	UInt8 defense = chara->baseDefense + chara->level;

	return defense;
}

UInt16 charaXpToNextLevel(UInt8 /* charaIndex */) {
	return 255;
}

void initParty(void) {
	initCharaAtIndex(0, "Kim", 8, 8, 9);
	initCharaAtIndex(1, "Sammy", 1, 6, 12);
	initCharaAtIndex(2, "Jean", 4, 6, 5);
	initCharaAtIndex(3, "Frisk", 1, 10, 8);

	// Testing
	partyChara[1].hp = 18;
	partyChara[2].hp = 25;
}

