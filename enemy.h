#pragma once
#include <string>
#include <iostream>

class Enemy {
public:
	enum class Type {
		Slime,
		Goblin,
		Orc,
		Dragon
	};

	// 基本データ
	Type type;
	std::string name;
	int level;
	int hp;
	int maxHp;
	int attack;
	int defense;
	int expReward;

	// コンストラクタ（デフォルト引数あり）
	Enemy(Type type = Type::Slime,
		  const std::string& name = "Slime",
		  int level = 1,
		  int hp = 8,
		  int attack = 3,
		  int defense = 0,
		  int expReward = 4);

	// 種別・レベルからの生成ファクトリ
	static Enemy CreateByType(Type type, int level = 1);

	// ダメージ処理 / 生存判定 / 与ダメージ算出
	void takeDamage(int damage);
	bool isAlive() const;
	int dealDamage() const;

	// デバッグ用表示
	std::string toString() const;
};

