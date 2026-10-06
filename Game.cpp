#include "Game.h"
#include <chrono>
#include <thread>
#include <algorithm>

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
		char c = static_cast<char>(std::tolower(input[0]));
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
	std::cout << "コマンド: (C)lear  (X)die(ゲームオーバー)  (M)enu\n";
	std::cout << "> ";

	std::string input = readLine();
	if (!input.empty()) {
		char c = static_cast<char>(std::tolower(input[0]));
		if (c == 'c') {
			scene = Scene::GameClear;
		}
		else if (c == 'x') {
			scene = Scene::GameOver;
		}
		else if (c == 'm') {
			scene = Scene::Title;
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
