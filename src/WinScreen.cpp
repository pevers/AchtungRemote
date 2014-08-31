#include "WinScreen.h"

WinScreen::WinScreen(boost::shared_ptr<Game> &game)
{
	m_game = game;
	m_textFont = al_load_ttf_font_f(load_alfaslabone(), TITLE_FONT, 20, 0);
	m_winFont = al_load_ttf_font_f(load_alfaslabone(), TITLE_FONT, 50, 0);
}

WinScreen::~WinScreen()
{
	if(m_textFont)
		al_destroy_font(m_textFont);
	if(m_winFont)
		al_destroy_font(m_winFont);
}

/**
 * Draw the screen.
 */
void WinScreen::Draw(ALLEGRO_DISPLAY *display)
{
	al_clear_to_color(al_map_rgb(0, 0, 0));

	std::vector<Player> players = m_game->GetPlayers();
	std::sort(players.begin(), players.end(), std::greater<Player>());

	int remaining = static_cast<int>(GetTimeStamp() + WINDELAY - al_get_time());
	std::ostringstream tmp;
	tmp << "Returning to main screen in ... " << remaining;
	al_draw_text(m_textFont, al_map_rgb(200, 200, 200), 1, 1, ALLEGRO_ALIGN_LEFT, tmp.str().c_str());

	for(int i = 0; i < (int)players.size(); i++)
	{
		std::ostringstream tmp;
		tmp << players[i].GetScore();
		al_draw_text(m_textFont, players[i].GetRGBColor(), SCREEN_WIDTH/3 + 100, 100 + i * (m_textFont->height + 5), ALLEGRO_ALIGN_CENTER, players[i].GetController()->name.c_str());
		al_draw_text(m_textFont, players[i].GetRGBColor(), SCREEN_WIDTH/3 + 250, 100 + i * (m_textFont->height + 5), ALLEGRO_ALIGN_CENTER, tmp.str().c_str());
	}
	al_draw_text(m_winFont, al_map_rgb(200, 200, 200), SCREEN_WIDTH/2, SCREEN_HEIGHT - m_winFont->height - 100, ALLEGRO_ALIGN_CENTER, "KONEC HRY"); 
}

int WinScreen::Update()
{
	if(al_get_time() >= (GetTimeStamp() + WINDELAY))
	{
		m_game->ResetGame();
		return SCREEN_HOME;
	}
	return SCREEN_WIN;
}