#pragma once
#include <Component/SimpleScene.h>
#include <Core/GPU/Mesh.h>

class Player {
	private:
		int lives = 3;
	public:
		int getLives();
		void decreaseLives();
};
