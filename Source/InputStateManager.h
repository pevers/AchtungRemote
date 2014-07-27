#ifndef INPUTSTATE_MANAGER_H__
#define INPUTSTATE_MANAGER_H__

#include <allegro5/allegro.h>
#include <map>

class KeyboardState
{
private:
	std::map<unsigned int, bool> m_keys;
public:
	KeyboardState();
	~KeyboardState();

	void SetKey(unsigned int key, bool value);
	bool GetKeyPressed(unsigned int key);	
};

/**
 * Structure holdin global keyboard/mouse state.
 */
struct InputState
{
	KeyboardState keyboard;
	// TODO: Mouse
};

extern InputState Input;

#endif