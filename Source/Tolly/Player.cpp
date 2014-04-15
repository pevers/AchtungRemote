#include "Player.h"

ALLEGRO_COLOR ColorToRGB(COLORS color)
{
	ALLEGRO_COLOR rgb[NUM_COLORS] = { al_map_rgb(220, 10, 0), al_map_rgb(0, 10, 220), al_map_rgb(10, 220, 0), 
		al_map_rgb(220, 128, 10), al_map_rgb(220, 30, 147), al_map_rgb(220, 220, 10)};
	return rgb[color];
}

Player::Player(int color, Position pos, float alpha, PlayerController *controller) : m_gap(10), m_dead(false), m_score(0) {
	m_color = static_cast<COLORS>(color);
	m_alpha = alpha;
	m_controller = controller;
	SetPosition(pos);
}

// default destructor
Player::~Player() {
}

/**
 * Returns the position of the player.
 */
Position Player::GetPosition() {
	return m_pos;
}

void Player::SetPosition(Position pos) {
	m_pos = pos;
	
	// create a gap
	if(m_gap > 6)
		m_pieces.push_back(pos);
	m_gap++;
}

void Player::SetPosition(float x, float y) {
	Position pos = { x, y };
	SetPosition(pos);
}

/**
 * Returns the direction of the player in degrees.
 */
float Player::GetDirection() {
	return m_alpha;
}

void Player::SetDirection(float alpha) {
	m_alpha = alpha;
}

void Player::SetColor(COLORS color) {
	m_color = color;
}

ALLEGRO_COLOR Player::GetRGBColor() const
{
	return ColorToRGB(m_color);
}

COLORS Player::GetColor() const
{
	return m_color;
}

bool Player::operator>(const Player &pl) const
{
	return m_score > pl.GetScore();
}

void Player::SetController(PlayerController *controller)
{
	m_controller = controller;
}

PlayerController *Player::GetController()
{
	return m_controller;
}

void Player::SetPiece(int x, int y)
{
	Position pos = {x, y};
	m_pieces.push_back(pos);
}

std::vector<Position> Player::GetPieces()
{
	return m_pieces;
}

void Player::SetGap()
{
	m_gap = 0;
}

int Player::GetLastGap()
{
	return m_gap;
}

void Player::SetDead(bool dead)
{
	m_dead = dead;
}

bool Player::isDead()
{
	return m_dead;
}

void Player::SetScore(int score)
{
	m_score = score;
}

int Player::GetScore() const
{
	return m_score;
}

void Player::ClearPieces()
{
	m_pieces.clear();
}