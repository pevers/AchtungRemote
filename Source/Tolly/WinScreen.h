#ifndef WINSCREEN_H__
#define WINSCREEN_H__

#include "Game.h"
#include "Screen.h"
#include "InputStateManager.h"

#include <boost/shared_ptr.hpp>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <string>

class WinScreen : public Screen
{
private:
	boost::shared_ptr<Game> m_game;
	ALLEGRO_FONT *m_textFont;
	ALLEGRO_FONT *m_winFont;
public:
	WinScreen(boost::shared_ptr<Game> &game);
	~WinScreen();

	int Update();
	void Draw(ALLEGRO_DISPLAY *display);
};

#endif