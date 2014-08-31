#ifndef SCREEN_H__
#define SCREEN_H__

#ifndef _DEBUG
#define ALLEGRO_STATICLINK
#endif 

#include "AchtungResources.h"
#include <allegro5/allegro.h>

#define SCREEN_WIDTH	1152
#define SCREEN_HEIGHT	864

class Screen
{
private:
	double m_timestamp;
public:
	virtual int Update() = 0;
	virtual void Draw(ALLEGRO_DISPLAY *display) = 0;

	void ResetTimeStamp();
	double GetTimeStamp();
};

#endif