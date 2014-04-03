#ifndef GAME_H__
#define GAME_H__

#include "GameServer.h"
#include "Player.h"
#include "Settings.h"
#include "ScreenManager.h"
#include "Settings.h"
#include "InputStateManager.h"
#include "Screen.h"
#include <math.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <string>
#include <vector>

#define PI				3.14159265
#define GAME_WIDTH		1000
#define GAME_HEIGHT		SCREEN_HEIGHT
#define TARGET_SCORE	1

/**
 * GSTATE_STOPPED	Game stopped
 * GSTATE_PRESTART	Couple of seconds before the player can actually control the game
 * GSTATE_STARTED	Game started
 * GSTATE_ENDED		Round finished
 * GSTATE_WIN		Complete game finished
 */
enum State { GSTATE_STOPPED = 0, GSTATE_PRESTART, GSTATE_STARTED, GSTATE_ENDED, GSTATE_WIN };

/**
 * Representation of the game it self
 */
class Game
{
private:
	Settings m_settings;

	boost::shared_ptr<Hive> m_hive;
	boost::shared_ptr<GameServer> m_server;
	boost::thread m_serverThread;

	State m_state;
	double m_timeSeed;

	std::vector<Player> m_players;
	ALLEGRO_SAMPLE *m_gameSound;
	ALLEGRO_SAMPLE *m_smashSound;
	ALLEGRO_SAMPLE_INSTANCE *m_gameSoundInstance;
	ALLEGRO_SAMPLE_INSTANCE *m_smashSoundInstance;
private:
	void InitSounds();
	void StartServer();
	void AddPointExceptFor(int player);
	int PlayerWon();
	void RestartGame();

	bool PlayerAtPosition(Position pos);
	Position GenerateRandomPosition();
	void SetGameState(State state);
public:
	Game();
	Game(Settings settings);
	~Game();
	
	void CreateGame(Settings settings);
	void StopGame();
	int GetNumberOfPlayersInQueue();
	int GetNumberOfPlayers();
	std::vector<Player> GetPlayers();

	int Update();
	
	boost::shared_ptr<GameServer> GetServer();

	void StartGame();
	bool isGameStarted();
	double GetTimer();
	State GetGameState();

	void ResetGame();
};

#endif