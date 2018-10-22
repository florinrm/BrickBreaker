#pragma once

class Player {
	private:
		int lives = 3;
	public:
		int getLives() {
			return lives;
		}

		void decreaseLives() {
			--lives;
		}
};
