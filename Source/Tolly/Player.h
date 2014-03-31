#ifndef PLAYER_H__
#define PLAYER_H__

#include "PlayerController.h"
#include "Metrics.h"
#include <allegro5/allegro.h>
#include <vector>

enum COLORS { RED = 0, BLUE, GREEN, ORANGE, PINK, YELLOW, NUM_COLORS };

ALLEGRO_COLOR ColorToRGB(COLORS color);

/**
 * Player class
 */
class Player
{
private:
	Position m_pos;
	float m_alpha;
	int m_gap;

	COLORS m_color;
	PlayerController *m_controller;
	std::vector<Position> m_pieces;

	bool m_dead;
	int m_score;
public:
	Player(int color, Position pos, float direction, PlayerController *controller);
	~Player();

	Position GetPosition();
	void SetPosition(Position pos);
	void SetPosition(float x, float y);

	float GetDirection();
	void SetDirection(float alpha);

	void SetColor(COLORS color);
	ALLEGRO_COLOR GetRGBColor() const;
	COLORS GetColor() const;

	void SetController(PlayerController *controller);
	PlayerController *GetController();

	void SetPiece(int x, int y);
	std::vector<Position> GetPieces();
	void ClearPieces();

	int GetLastGap();
	void SetGap();

	void SetDead(bool dead);
	bool isDead();

	void SetScore(int score);
	int GetScore() const;

	bool operator>(const Player &pl) const;
};

#endif