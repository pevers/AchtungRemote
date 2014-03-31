#ifndef SCREEN_H__
#define SCREEN_H__

#include <allegro5/allegro.h>

#define SCREEN_WIDTH	1152
#define SCREEN_HEIGHT	864

class Screen
{
public:
	virtual int Update() = 0;
	virtual void Draw(ALLEGRO_DISPLAY *display) = 0;
};

#endif