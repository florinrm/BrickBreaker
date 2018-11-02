#pragma once

#include <Component/SimpleScene.h>
#include <string>
#include <Core/Engine.h>
#include "Player.h"


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
	std::vector<int> brickStatus;

	const float platformX = 600, platformY = 30;
	const float brickLength = 50, brickHeight = 30;
	const float platformLength = 160, platformHeight = 15;

	bool hasGameBegun = false;
	bool lostLife = false;

	float initialBallPosX = 680 + mouseMoveOX;
	float initialBallPosY = 58.5;

	bool justStarted = true;
	bool isBallSticky = false;
	bool hitPlatform = false;

	float bouncePlatformX = 0;
	float pula = 0;

	std::vector<float> scaleTimes;
};
