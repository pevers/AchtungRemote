#include "Screen.h"

/**
 * Every screen need to know when it started drawing. This function will reset the timestamp.
 */
void Screen::ResetTimeStamp()
{
	m_timestamp = al_get_time();
}

/**
 * Return the timestamp from the moment this screen began drawing.
 */
double Screen::GetTimeStamp()
{
	return m_timestamp;
}