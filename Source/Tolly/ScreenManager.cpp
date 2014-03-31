#include "ScreenManager.h"

ScreenManager::ScreenManager() : m_currentScreen(NULL)
{
}

ScreenManager::~ScreenManager()
{
	for(std::map<int, Screen*>::const_iterator it = m_screens.begin(); it != m_screens.end(); ++it)
	{
		delete it->second;
	}
}

void ScreenManager::Update()
{
	int screen = m_currentScreen->Update();
	if(m_screens.at(screen) != m_currentScreen)
		SetScreen(screen);
}

void ScreenManager::Draw(ALLEGRO_DISPLAY *display)
{
	if(m_currentScreen)
		m_currentScreen->Draw(display);
}

void ScreenManager::AddScreen(int id, Screen *screen)
{
	if(m_screens.find(id) != m_screens.end())
		delete m_screens[id];
	m_screens[id] = screen;
	m_currentScreen = screen;
}

bool ScreenManager::SetScreen(int id)
{
	if(m_screens.find(id) == m_screens.end())
		return false;
	m_currentScreen = m_screens[id];
	return true;
}

