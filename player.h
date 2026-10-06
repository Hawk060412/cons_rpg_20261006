#pragma once
#include <string>
#include <iostream>

class Player {
public:
    std::string name;
	int hp;
    int attack;
	int defense;

	// デフォルト値をすべて明示してコンストラクタの不整合を解消
	Player(const std::string& name = "勇者", int hp = 30, int attack = 8, int defense = 2)
		: name(name)
		, hp(hp)
		, attack(attack)
		, defense(defense) {}
};

