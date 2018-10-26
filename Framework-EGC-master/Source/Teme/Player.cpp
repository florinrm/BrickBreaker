#include "Player.h"

void Player::decreaseLives() {
	--lives;
}

int Player::getLives() {
	return lives;
}