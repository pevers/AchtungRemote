#ifndef SCREENMANAGER_H__
#define SCREENMANAGER_H__

#include "Screen.h"
#include <map>

#define SCREEN_HOME	0
#define SCREEN_GAME	1
#define SCREEN_WIN	2

#define TITLE_FONT	"Font/AlfaSlabOne-Regular.ttf"

class ScreenManager
{
private:
	std::map<int, Screen*> m_screens;
	Screen *m_currentScreen;
public:
	ScreenManager();
	~ScreenManager();

	void Draw(ALLEGRO_DISPLAY *display);
	void Update();

	void AddScreen(int id, Screen *screen);
	bool SetScreen(int id);
};

#endif