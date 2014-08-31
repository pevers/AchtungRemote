#ifndef MAINSCREEN_H__
#define MAINSCREEN_H__

#include "Game.h"
#include "Screen.h"
#include "InputStateManager.h"
#include "Metrics.h"

#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>

#define BOX_WIDTH 250
#define BOX_HEIGHT 250
#define MARGIN_Y 80
#define MARGIN_X 38

class MainScreen : public Screen 
{
private:
	ALLEGRO_FONT *m_playerFont, *m_titleFont, *m_instrFont;
	boost::shared_ptr<Game> m_game;

	ALLEGRO_BITMAP *m_playerBitmaps[6];
public:
	MainScreen(boost::shared_ptr<Game> &game);
	~MainScreen();

	int Update();
	void Draw(ALLEGRO_DISPLAY *display);
	void DrawPlayerBoxes();
	void DrawPlayerName(std::string name, float x, float y);
};

#endif