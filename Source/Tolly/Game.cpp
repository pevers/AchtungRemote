#include "Game.h"

// default constructor
Game::Game():m_gameSound(NULL), m_gameSoundInstance(NULL), m_smashSound(NULL), m_smashSoundInstance(NULL) {
	Settings sett;
	sett.gamespeed = SPEED_MEDIUM;
	sett.name = "Default Achtung Server";
	CreateGame(sett);

	srand(time(0));
}

Game::Game(Settings settings) {
	CreateGame(settings);
}

Game::~Game() {
	StopGame();

	if(m_gameSound)
		al_destroy_sample(m_gameSound);
	if(m_smashSound)
		al_destroy_sample(m_smashSound);
	if(m_gameSoundInstance)
		al_destroy_sample_instance(m_gameSoundInstance);
	if(m_smashSoundInstance)
		al_destroy_sample_instance(m_smashSoundInstance);
}

/**
 * Create a game with the given settings.
 */
void Game::CreateGame(Settings settings)
{
	if(settings.gamespeed <= SPEED_UNDEFINED)
		settings.gamespeed = SPEED_MEDIUM;
	
	m_settings = settings;

	// create thread to run server
	m_hive = boost::shared_ptr<Hive>(new Hive());
	m_server = boost::shared_ptr<GameServer>(new GameServer(m_hive->GetService(), settings.name));
	m_serverThread = boost::thread(&Game::StartServer, this);
	SetGameState(GSTATE_STOPPED);	// Game state is stopped because the players are waiting in queue

	// load sounds, don't bother any errors
	al_reserve_samples(1);
	m_gameSound = al_load_sample("Audio/rick.wav");
	m_smashSound = al_load_sample("Audio/smash.wav");

	if(m_gameSound && m_smashSound)
	{
		m_gameSoundInstance = al_create_sample_instance(m_gameSound);
		m_smashSoundInstance = al_create_sample_instance(m_smashSound);
		al_set_sample_instance_playmode(m_gameSoundInstance, ALLEGRO_PLAYMODE_LOOP);
		al_attach_sample_instance_to_mixer(m_gameSoundInstance, al_get_default_mixer());
		al_attach_sample_instance_to_mixer(m_smashSoundInstance, al_get_default_mixer());
	}
	else
	{
		std::cout << "[DEBUG]: Can't load the super awesome song!" << std::endl;
	}
}

void Game::StopGame()
{
	m_hive->Stop();
	m_serverThread.join();
	SetGameState(GSTATE_STOPPED);
}

/**
 * Do the game logic here
 */
int Game::Update()
{
	// pauze the first 3 seconds 
	if(GetGameState() == GSTATE_PRESTART && GetTimer() >= 2.5)
	{
		SetGameState(GSTATE_STARTED);
	}
	else if(GetGameState() == GSTATE_ENDED && GetTimer() >= 3)
	{
		RestartGame();
	}
	else if(GetGameState() == GSTATE_WIN && GetTimer() >= 2)
	{
		return SCREEN_WIN;
	}

	if(m_state != GSTATE_STARTED)
		return SCREEN_GAME;	// don't do any logic if the game is not started

	std::vector<PlayerController> *controllers = m_server->GetPlayerControllers();
	for(int i = 0; i < (int)m_players.size(); i++)
	{
		if(!m_players[i].isDead())
		{
			// get pieces
			std::vector<Position> pieces = m_players[i].GetPieces();
			float alpha = m_players[i].GetDirection();

			if(controllers->at(i).input == LEFT)
				alpha -= 0.001;
			else if(controllers->at(i).input == RIGHT)
				alpha += 0.001;

			float x = m_players[i].GetPosition().x + 2*cos(alpha * 180.0 / PI);
			float y = m_players[i].GetPosition().y + 2*sin(alpha * 180.0 / PI);
			if(m_players[i].GetLastGap() > 200 && rand() % 10 == 0)
			{
				// gap
				x += 15*cos(alpha * 180.0 / PI);
				y += 15*sin(alpha * 180.0 / PI);
				m_players[i].SetGap();
			}

			// check for collision
			bool collision = false;

			// check for walls
			if(x < 0 || y < 0 || x > GAME_WIDTH || y > GAME_HEIGHT)
				collision = true;
			else
			{
				for(int j = 0; j < (int)m_players.size(); j++)
				{
					std::vector<Position> collisionMap = m_players[j].GetPieces();
					for(int k = 0; k < (int)collisionMap.size(); k++)
					{
						if((j == i) && (k > ((int)collisionMap.size() - 2)))	// skip two last locations because they will collide
							continue;

						if(collisionMap[k].x > x - 2 && collisionMap[k].y > y - 2
							&& collisionMap[k].x < x + 2 && collisionMap[k].y < y + 2)
							collision = true;
					}
				}
			}
					
			if(!collision)
			{
				Position pos = { x, y }; 
				m_players[i].SetPosition(pos);
				m_players[i].SetDirection(alpha);
			} 
			else
			{
				// play collision sound
				al_play_sample_instance(m_smashSoundInstance);

				// player died!
				m_players[i].SetDead(true);
				// other alive players receive 1 point
				AddPointExceptFor(i);
				int player = PlayerWon();
				if(player != -1)
				{
					// some player won
					if(m_players[player].GetScore() >= TARGET_SCORE)
					{
						// go to win screen
						SetGameState(GSTATE_WIN);
					}
					else
					{
						// stop and restart in 3 seconds
						SetGameState(GSTATE_ENDED);
					}
				}
			}
		}
	}

	return SCREEN_GAME;
}

/**
 * Starts listening to incoming players.
 */
void Game::StartServer()
{
	m_hive->Run();
}

boost::shared_ptr<GameServer>Game::GetServer()
{
	return m_server;
}

void Game::StartGame()
{
	// map players to the controllers
	std::vector<PlayerController> *controller = m_server->GetPlayerControllers();

	if((int)controller->size() < 1)
		return;

	for(int i = 0; i < (int)controller->size(); i++)
	{
		Position pos = GenerateRandomPosition();
		Player tmp(i, pos, rand() % 60, &controller->at(i));
		m_players.push_back(tmp);
	}

	// start the HIGHLY annoying sound!
	//al_play_sample_instance(m_gameSoundInstance);

	SetGameState(GSTATE_PRESTART);	// pre-start the game, blinking players
}

bool Game::isGameStarted()
{
	return m_state == GSTATE_STARTED;
}

double Game::GetTimer()
{
	return al_get_time() - m_timeSeed;
}

/**
 * Return the number of players connected to the server.
 */
int Game::GetNumberOfPlayersInQueue()
{
	return m_server->GetPlayerControllers()->size();
}

int Game::GetNumberOfPlayers()
{
	return m_players.size();
}

std::vector<Player> Game::GetPlayers()
{
	return m_players;
}

/**
 * Add one point to every other alive player who isn't <player>
 */
void Game::AddPointExceptFor(int player)
{
	for(int i = 0; i < (int)m_players.size(); i++)
	{
		if(i != player && !m_players[i].isDead())
			m_players[i].SetScore(m_players[i].GetScore() + 1);
	}
}

/**
 * Checks if there is only one player alive, if so it returns the index to that player, otherwise returns -1.
 */
int Game::PlayerWon()
{
	int alive = 0;
	int won = -1;
	for(int i = 0; i < (int)m_players.size(); i++)
	{
		if(!m_players[i].isDead())
		{
			alive++;
			won = i;
		}
	}
	if(alive == 1)
		return won;
	else
		return -1;
}

/**
 * Restart the game keeping the scores.
 */
void Game::RestartGame()
{
	for(int i = 0; i < (int)m_players.size(); i++)
	{
		m_players[i].ClearPieces();
	}

	for(int i = 0; i < (int)m_players.size(); i++)
	{
		Position pos = GenerateRandomPosition();
		m_players[i].SetPosition(pos);
		m_players[i].SetDirection(rand() % 60);
		m_players[i].SetDead(false);
	}
	SetGameState(GSTATE_PRESTART);
}

/**
 * Generate a random start position for a player.
 */
Position Game::GenerateRandomPosition()
{
	Position pos;
	do
	{
		pos.x = rand() % (GAME_WIDTH - 100);
		pos.y = rand() % (GAME_HEIGHT - 100);
	} while(PlayerAtPosition(pos));
	return pos;
}

/**
 * Checks if there is a player at this position.
 */
bool Game::PlayerAtPosition(Position pos)
{
	for(int i = 0; i < (int)m_players.size(); i++)
	{
		std::vector<Position> pieces = m_players[i].GetPieces();
		for(int j = 0; j < (int)pieces.size(); j++)
		{
			if(pieces[j].x == pos.x && pieces[j].y == pos.y)
				return true;
		}
	}
	return false;
}

/**
 * Get the game state.
 */
State Game::GetGameState()
{
	return m_state;
}

/**
 * Go to the gamestate state.
 */
void Game::SetGameState(State state)
{
	m_state = state;
	m_timeSeed = al_get_time();	// time seed, used for transition of different game states (like starting a game)

	// set server settings according to the game state
	if(state == GSTATE_PRESTART)
		m_server->SetServerState(SERVER_GAME_STARTED);	// close the server
	else if(state == GSTATE_ENDED)
		m_server->SetServerState(SERVER_ACCEPT);	// open server for connections		
}