#pragma once

#include <Component/SimpleScene.h>
#include <string>
#include <Core/Engine.h>
#include "Player.h"
#include <map>


class Tema1 : public SimpleScene {
public:
	Tema1();
	~Tema1();
	void Init() override;
private:
	void FrameStart() override;
	void Update(float deltaTimeSeconds) override;
	void FrameEnd() override;

	void DrawScene(glm::mat3 visMatrix);

	void OnInputUpdate(float deltaTime, int mods) override;
	void OnKeyPress(int key, int mods) override;
	void OnKeyRelease(int key, int mods) override;
	void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
	void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;

	bool AllBricksDissappeared() {
		for (int i = 0; i < 50; ++i) {
			if (!blocksHit[i])
				return false;
		}
		return true;
	}

protected:
	glm::mat3 modelMatrix;
	float translateX, translateY;
	float scaleX, scaleY;
	float angularStep;
	float cx, cy;
	bool ok;
	bool direction;
	float rotation = 0;

	int signX = 1, signY = 1;
	float angle = M_PI / 2;

	Player player;
	float mouseMoveOX = 0; // mouse moving
	std::vector<bool> blocksHit;

	const float platformX = 600, platformY = 30;
	const float brickLength = 50, brickHeight = 30;
	const float platformLength = 160, platformHeight = 15;

	bool hasGameBegun = false;

	float initialBallPosX = 680 + mouseMoveOX;
	float initialBallPosY = 58.5;

	std::vector<float> scaling;

	// power up # 1 - saving wall
	bool savingWall = false;
	float powerUpTime1 = 0;
	float powerUp1X = 275 + 80 * 7;
	float powerUp1Y = 405;
	float powerUp1Rotation = 0;

	// power up # 2 - destroy bricks without reflection
	bool noBrickReflection = false;
	float powerUpTime2 = 0;
	float powerUp2X = 275 + 80 * 5;
	float powerUp2Y = 465;
	float powerUp2Rotation = 0;

	// some very solid bricks
	std::map<int, int> solidBricks; // solid bricks: 4, 30, 42, 24, 17
	// if value of a brick is 3 => the brick dissapears
	// power up # 3 - sticky platform
};
