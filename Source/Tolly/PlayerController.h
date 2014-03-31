#ifndef PLAYERCONTROLLER_H__
#define PLAYERCONTROLLER_H__

#include <string>

enum CONROLS { NONE = 0, LEFT, RIGHT };

struct PlayerController
{
	std::string name;
	int input;	// NONE, LEFT or RIGHT
};

#endif