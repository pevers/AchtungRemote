#ifndef SETTINGS_H__
#define SETTINGS_H__

#define MAX_PLAYERS				6

enum GAMESPEED 
{
	SPEED_UNDEFINED = -1,
	SPEED_SLOW,
	SPEED_MEDIUM,
	SPEED_FAST
};

struct Settings
{
	GAMESPEED gamespeed;	// integer between 1 - 3
	std::string name;
};

#endif