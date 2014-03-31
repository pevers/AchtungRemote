#include "InputStateManager.h"

InputState Input;

KeyboardState::KeyboardState()
{
}

KeyboardState::~KeyboardState()
{
}

void KeyboardState::SetKey(unsigned int key, bool value)
{
	m_keys[key] = value;
}

bool KeyboardState::GetKeyPressed(unsigned int key)
{
	if(m_keys.find(key) == m_keys.end())
		return false;
	else
		return m_keys.at(key);
}