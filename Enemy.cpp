#include "enemy.h"
#include <sstream>
#include <random>
#include <algorithm>
#include <cmath>

// コンストラクタ実体化
Enemy::Enemy(Type type, const std::string& name, int level, int hp, int attack, int defense, int expReward)
	: type(type)
	, name(name)
	, level(std::max(1, level))
	, hp(hp)
	, maxHp(hp)
	, attack(attack)
	, defense(defense)
	, expReward(expReward) {
	if (this->hp < 1) this->hp = this->maxHp = 1;
}

// 種別とレベルに基づいた敵を返す（簡易スケーリング）
Enemy Enemy::CreateByType(Type type, int level) {
	level = std::max(1, level);

	auto scale = [&](int base, double growth) -> int {
		// 成長率に基づいて丸めた値を返す
		return static_cast<int>(std::round(base * std::pow(growth, level - 1)));
	};

	switch (type) {
	case Type::Slime: {
		int hp = scale(8, 1.10);
		int atk = scale(3, 1.08);
		int def = scale(0, 1.03);
		int exp = 4 + level;
		return Enemy(type, "Slime", level, hp, atk, def, exp);
	}
	case Type::Goblin: {
		int hp = scale(18, 1.12);
		int atk = scale(6, 1.10);
		int def = scale(1, 1.05);
		int exp = 8 + level * 2;
		return Enemy(type, "Goblin", level, hp, atk, def, exp);
	}
	case Type::Orc: {
		int hp = scale(36, 1.14);
		int atk = scale(12, 1.12);
		int def = scale(3, 1.06);
		int exp = 18 + level * 4;
		return Enemy(type, "Orc", level, hp, atk, def, exp);
	}
	case Type::Dragon: {
		int hp = scale(120, 1.18);
		int atk = scale(28, 1.15);
		int def = scale(10, 1.10);
		int exp = 80 + level * 10;
		return Enemy(type, "Dragon", level, hp, atk, def, exp);
	}
	default: {
		return Enemy(Type::Slime, "Slime", level, 8, 3, 0, 4);
	}
	}
}

// ダメージ処理（防御を差し引いて最低0ダメージ）
void Enemy::takeDamage(int damage) {
	int actual = damage - defense;
	if (actual < 0) actual = 0;
	hp -= actual;
	if (hp < 0) hp = 0;
}

// 生存判定
bool Enemy::isAlive() const {
	return hp > 0;
}

// 与ダメージ算出：攻撃力にランダム性を付与（±10%）
int Enemy::dealDamage() const {
	static thread_local std::mt19937 rng(static_cast<unsigned int>(std::random_device{}()));
	double variance = 0.1;
	std::uniform_real_distribution<double> dist(1.0 - variance, 1.0 + variance);
	double factor = dist(rng);
	int dmg = static_cast<int>(std::round(attack * factor));
	return std::max(0, dmg);
}

// 表示用文字列（デバッグ表示）
std::string Enemy::toString() const {
	std::ostringstream sb;
	sb << name << " (Lv" << level << ") HP:" << hp << "/" << maxHp << " ATK:" << attack << " DEF:" << defense << " EXP:" << expReward;
	return sb.str();
}

