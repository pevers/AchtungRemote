#include "Screen.h"

InputState Screen::GetInputState()
{
	return m_input;
}

void Screen::UpdateInputState(InputState &input)
{
	m_input = input;
}