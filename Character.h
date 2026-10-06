#pragma once
#include <string>
#include <iostream>

class Player {
public:
    std::string name;
	int hp;
    int attack;
	int defense;

	Player(const std::string& name = "—EŽÒ", int hp, int attack, int defense)
		:name(name), hp(hp), attack(attack), defense(defense) {}
	/*void takeDamage(int damage) {
		int actualDamage = damage - defense;
		if (actualDamage < 0) {
			actualDamage = 0;
		}
		hp -= actualDamage;
		if (hp < 0) {
			hp = 0;
		}
	}
	bool isAlive() const {
		return hp > 0;
	}*/
	
};

