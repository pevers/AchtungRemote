#include "GameScreen.h"

GameScreen::GameScreen(boost::shared_ptr<Game> &game) : m_timeOn(0.7), m_timeOff(0.3), m_timeStamp(al_current_time()), m_blink(false)
{
	m_game = game;
	m_nameFont = al_load_ttf_font(TITLE_FONT, 14, 0);
	m_scoreFont = al_load_ttf_font(TITLE_FONT, 30, 0);
}

GameScreen::~GameScreen()
{
	if(m_nameFont)
		al_destroy_font(m_nameFont);
	if(m_scoreFont)
		al_destroy_font(m_scoreFont);
}

int GameScreen::Update()
{
	return m_game->Update();
}

void GameScreen::Draw(ALLEGRO_DISPLAY *display)
{
	al_clear_to_color(al_map_rgb(0, 0, 0));

	// draw right box
	al_draw_filled_rectangle(GAME_WIDTH, 0, SCREEN_WIDTH, SCREEN_HEIGHT, al_map_rgb(160, 160, 160));

	// draw border
	al_draw_filled_rectangle(GAME_WIDTH, 0, GAME_WIDTH + 4, SCREEN_HEIGHT, al_map_rgb(195, 195, 195));

	std::vector<Player> players = m_game->GetPlayers();

	// sort by score
	std::sort(players.begin(), players.end(), std::greater<Player>());

	// draw snake parts
	float centerX = (SCREEN_WIDTH - GAME_WIDTH - 4) / 2;
	float marginY = SCREEN_HEIGHT / 6;
	for(int i = 0; i < (int)players.size(); i++)
	{
		// draw scores and name
		al_draw_text(m_nameFont, players[i].GetRGBColor(), GAME_WIDTH + 6, i * marginY, ALLEGRO_ALIGN_LEFT, players[i].GetController()->name.c_str());
		std::ostringstream convert;
		convert << players[i].GetScore();
		al_draw_text(m_scoreFont, players[i].GetRGBColor(), GAME_WIDTH + centerX, i * marginY + marginY/2-30, ALLEGRO_ALIGN_CENTER, convert.str().c_str());

		std::vector<Position> pieces = players[i].GetPieces();
		for(int j = 0; j < (int)pieces.size(); j++)
		{
			if(!(m_game->GetGameState() == GSTATE_PRESTART && !Blink()))
				al_draw_filled_circle(pieces[j].x, pieces[j].y, 2, players[i].GetRGBColor());
		}
	}
}

/**
 * Set the blink time in seconds for on and off.
 */
void GameScreen::SetBlinkTime(float on, float off)
{
	m_timeOn = on;
	m_timeOff = off;
}

/**
 * Returns true every m_timeOn seconds of m_timeOn + m_timeOff;
 */
bool GameScreen::Blink()
{
	if(m_blink && (al_current_time() - m_timeStamp >= m_timeOn))
	{
		m_timeStamp = al_current_time();
		return (m_blink = false);
	}
	else if(!m_blink && (al_current_time() - m_timeStamp >= m_timeOff))
	{
		m_timeStamp = al_current_time();
		return (m_blink = true);
	}

	return m_blink;
}