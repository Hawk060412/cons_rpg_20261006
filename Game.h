#pragma once
#include <string>
#include <iostream>

class Game {
public:
	enum class Scene {
		Title,
		Play,
		GameOver,
		GameClear,
		Exit
	};

	Game();
	void Init();
	void Runloop();

	void title();
	void play();
	void gameOver();
	void gameClear();

	void RunBattle();

private:
	Scene scene;
	bool isRunning;
	bool initialized;

	// ÉwÉãÉpÅ[
	std::string readLine();
	void clearScreen();
	void sleepMs(int ms);
};