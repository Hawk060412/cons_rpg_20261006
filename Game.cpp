#include "Game.h"
#include "player.h"
#include "enemy.h"
#include <chrono>
#include <thread>
#include <algorithm>
#include <random>
#include <cctype>
#include <cmath> // std::round を使うために追加

// コンストラクタ
Game::Game()
	: scene(Scene::Title)
	, isRunning(false)
	, initialized(false) {
}

void Game::Init() {
	scene = Scene::Title;
	isRunning = true;
	initialized = true;
}

void Game::Runloop() {
	if (!initialized) {
		Init();
	}

	const int targetFPS = 30;
	const int frameMs = 1000 / targetFPS;

	while (isRunning && scene != Scene::Exit) {
		auto frameStart = std::chrono::steady_clock::now();

		switch (scene) {
		case Scene::Title:
			title();
			break;
		case Scene::Play:
			play();
			break;
		case Scene::GameOver:
			gameOver();
			break;
		case Scene::GameClear:
			gameClear();
			break;
		default:
			break;
		}

		auto frameEnd = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(frameEnd - frameStart).count();
		int wait = frameMs - static_cast<int>(elapsed);
		if (wait > 0) sleepMs(wait);
	}
}

// タイトル画面
void Game::title() {
	clearScreen();
	std::cout << "=== TITLE ===\n";
	std::cout << "(S)tart  (E)xit\n";
	std::cout << "> ";

	std::string input = readLine();
	if (!input.empty()) {
		char c = static_cast<char>(std::tolower(static_cast<unsigned char>(input[0])));
		if (c == 's') {
			scene = Scene::Play;
		}
		else if (c == 'e') {
			scene = Scene::Exit;
			isRunning = false;
		}
	}
}

// プレイ画面（簡易）
void Game::play() {
	clearScreen();
	std::cout << "=== PLAY ===\n";
	std::cout << "コマンド: (B)attle  (C)lear  (X)die(ゲームオーバー)  (M)enu\n";
	std::cout << "> ";

	std::string input = readLine();
	if (!input.empty()) {
		char c = static_cast<char>(std::tolower(static_cast<unsigned char>(input[0])));
		if (c == 'c') {
			scene = Scene::GameClear;
		}
		else if (c == 'x') {
			scene = Scene::GameOver;
		}
		else if (c == 'm') {
			scene = Scene::Title;
		}
		else if (c == 'b') {
			RunBattle();
		}
	}
}

// ゲームオーバー
void Game::gameOver() {
	clearScreen();
	std::cout << "=== GAME OVER ===\n";
	std::cout << "Enterキーでタイトルに戻ります。\n";
	readLine();
	scene = Scene::Title;
}

// ゲームクリア
void Game::gameClear() {
	clearScreen();
	std::cout << "=== GAME CLEAR ===\n";
	std::cout << "Enterキーでタイトルに戻ります。\n";
	readLine();
	scene = Scene::Title;
}

// 標準入力から1行読み取り（空行も取得）
std::string Game::readLine() {
	std::string line;
	std::getline(std::cin, line);
	// Windows のコンソールで前の入力が残っている可能性を配慮
	if (!std::cin) {
		std::cin.clear();
	}
	// 先頭/末尾の空白をトリム
	auto notSpace = [](int ch) { return !std::isspace(ch); };
	line.erase(line.begin(), std::find_if(line.begin(), line.end(), notSpace));
	line.erase(std::find_if(line.rbegin(), line.rend(), notSpace).base(), line.end());
	return line;
}

// 画面クリア（Windows のみなら system("cls")。他環境では clear）
void Game::clearScreen() {
	#ifdef _WIN32
		std::system("cls");
	#else
		std::system("clear");
	#endif
}

// 簡易フレーム待機
void Game::sleepMs(int ms) {
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void Game::RunBattle() {
	// 累積勝利数を保持（ゲーム起動中に持たせる）
	static int winCount = 0;

	// 簡易ターン制バトル実装（複数回の敵と戦える）
	clearScreen();
	std::cout << "=== BATTLE ===\n";
	std::cout << "バトルが始まります！ 3 回勝利でゲームクリア。\n\n";
	std::cout << "Enterキーで開始...\n";
	readLine();

	// プレイヤー初期化（この RunBattle 呼び出し中は継続）
	Player player("Hero", 30, 8, 2);

	// 乱数作成
	static thread_local std::mt19937 rng(static_cast<unsigned int>(std::random_device{}()));
	std::uniform_real_distribution<double> varDist(0.9, 1.1); // ±10% のばらつき
	std::uniform_int_distribution<int> escapeDist(0, 1); // 逃走成功判定（50%）
	std::uniform_int_distribution<int> typeDist(0, 3); // 0..3 (Slime,Goblin,Orc,Dragon)
	std::uniform_int_distribution<int> levelDist(1, 3); // 敵レベル1〜3

	// 続けて敵と戦うループ（敗北・逃走で途中終了、勝利が3回でクリア）
	while (player.hp > 0 && winCount < 3) {
		// 敵をランダム生成
		Enemy::Type etype = static_cast<Enemy::Type>(typeDist(rng));
		int elevel = levelDist(rng);
		Enemy enemy = Enemy::CreateByType(etype, elevel);

		// 1戦ごとのループ
		while (player.hp > 0 && enemy.hp > 0) {
			// ステータス表示
			clearScreen();
			std::cout << "=== BATTLE ===\n\n";
			std::cout << "勝利数: " << winCount << " / 3\n\n";
			std::cout << "Player: " << player.name << "  HP:" << player.hp << "\n";
			std::cout << "Enemy : " << enemy.name << " (Lv" << enemy.level << ")  HP:" << enemy.hp << "\n\n";

			std::cout << "行動を選択してください: (A)ttack  (R)un\n> ";
			std::string input = readLine();
			if (input.empty()) continue;
			char cmd = static_cast<char>(std::tolower(static_cast<unsigned char>(input[0])));

			if (cmd == 'a') {
				// プレイヤー攻撃（ばらつきあり）
				double factor = varDist(rng);
				int raw = static_cast<int>(std::round(player.attack * factor));
				enemy.takeDamage(std::max(0, raw));
				int dealt = std::max(0, raw - enemy.defense);
				std::cout << "あなたは " << enemy.name << " に " << dealt << " のダメージを与えた。\n";
				if (enemy.hp <= 0) {
					enemy.hp = 0;
					std::cout << enemy.name << " を倒した！\n";
					winCount++;
					std::cout << "現在の勝利数: " << winCount << " / 3\n";
					std::cout << "Enterキーで続行...\n";
					readLine();

					if (winCount >= 3) {
						scene = Scene::GameClear;
						return;
					}
					// 次の敵へ（この戦闘終了）
					break;
				}
			}
			else if (cmd == 'r') {
				// 逃走判定
				int escape = escapeDist(rng);
				if (escape == 1) {
					std::cout << "逃走に成功した。\n";
					std::cout << "Enterキーで続行...\n";
					readLine();
					// 逃走は勝利カウントしない。プレイ画面へ戻る。
					scene = Scene::Play;
					return;
				}
				else {
					std::cout << "逃走に失敗した。\n";
				}
			}
			else {
				// 無効な入力は再ループ
				continue;
			}

			// 敵ターン（生存している場合）
			if (enemy.hp > 0) {
				int rawE = enemy.dealDamage();
				int dmgE = rawE - player.defense;
				if (dmgE < 0) dmgE = 0;
				player.hp -= dmgE;
				std::cout << enemy.name << " はあなたに " << dmgE << " のダメージを与えた。\n";
				if (player.hp <= 0) {
					player.hp = 0;
					std::cout << "あなたは倒れた...\n";
					std::cout << "Enterキーで続行...\n";
					readLine();
					scene = Scene::GameOver;
					return;
				}
			}

			std::cout << "Enterキーで次へ...\n";
			readLine();
		} // 1戦ループ終了

		// ループを抜けて次の敵へ。player.hp は継続、winCount は保持。
	} // 複数戦ループ終了

	// ループを抜ける理由のチェック
	if (winCount >= 3) {
		scene = Scene::GameClear;
		return;
	}
	// プレイヤーがまだ生きていて勝利数に到達していない場合はプレイ画面へ戻る
	scene = Scene::Play;
}