#ifndef GAMESCREEN_H__
#define GAMESCREEN_H__

#include "Game.h"
#include "Screen.h"
#include "InputStateManager.h"

#include <boost/shared_ptr.hpp>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <string>

class GameScreen : public Screen
{
private:
	boost::shared_ptr<Game> m_game;
	ALLEGRO_FONT *m_nameFont, *m_scoreFont;

	float m_timeOn, m_timeOff;
	double m_timeStamp;
	bool m_blink;
public:
	GameScreen(boost::shared_ptr<Game> &game);
	~GameScreen();

	int Update();
	void Draw(ALLEGRO_DISPLAY *display);

	void SetBlinkTime(float on, float off);
	bool Blink();
};

#endif