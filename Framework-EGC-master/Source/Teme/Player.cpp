#include "Player.h"

void Player::decreaseLives() {
	--lives;
}

int Player::getLives() {
	return lives;
}

void Player::setLives(int n) {
	lives = n;
}

void Player::increaseLives() {
	++lives;
}