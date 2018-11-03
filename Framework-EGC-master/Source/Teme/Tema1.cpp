#include "Tema1.h"

#include <Core/Engine.h>
#include "Laboratoare/Laborator3/Transform2D.h"
#include "Laboratoare/Laborator3/Object2D.h"
#include <iostream>
#include "Player.h"

Tema1::Tema1() {

}

Tema1::~Tema1() {

}

void Tema1::Init() {
	glm::ivec2 resolution = window->GetResolution();
	auto camera = GetSceneCamera();
	camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
	camera->SetPosition(glm::vec3(0, 0, 50));
	camera->SetRotation(glm::vec3(0, 0, 0));
	camera->Update();
	GetCameraInput()->SetActive(false);

	glm::vec3 corner = glm::vec3(0, 0, 0);
	float squareSide = 100;

	// compute coordinates of square center
	cx = corner.x + squareSide / 16;
	cy = corner.y + squareSide / 16;

	// initialize tx and ty (the translation steps)
	translateX = 0;
	translateY = 0;

	// initialize sx and sy (the scale factors)
	scaleX = 1;
	scaleY = 1;

	// initialize angularStep
	angularStep = 0;


	Mesh* left = Object2D::CreateRectangle("left", corner, squareSide * 2 / 4, resolution.y, glm::vec3(1, 0, 0), true);
	AddMeshToList(left);

	Mesh* right = Object2D::CreateRectangle("right", corner, resolution.x, squareSide / 2, glm::vec3(1, 0, 0), true);
	AddMeshToList(right);

	Mesh* top = Object2D::CreateRectangle("top", corner, squareSide * 2 / 4, resolution.y, glm::vec3(1, 0, 0), true);
	AddMeshToList(top);

	Mesh* platform = Object2D::CreateRectangle("platform", corner, 160, 15, glm::vec3(0, 1, 0), true);
	AddMeshToList(platform);

	Mesh* life1 = Object2D::CreateCircle("life1", corner, 360, glm::vec3(1, 1, 1));
	AddMeshToList(life1);

	Mesh* life2 = Object2D::CreateCircle("life2", corner, 360, glm::vec3(1, 1, 1));
	AddMeshToList(life2);

	Mesh* life3 = Object2D::CreateCircle("life3", corner, 360, glm::vec3(1, 1, 1));
	AddMeshToList(life3);

	Mesh* ball = Object2D::CreateCircle("ball", corner, 360, glm::vec3(1, 1, 1));
	AddMeshToList(ball);

	Mesh* brick = Object2D::CreateRectangle("brick", corner, 50, 30, glm::vec3(1, 0, 0), true);
	AddMeshToList(brick);

	Mesh* bricks[50];
	blocksHit.resize(50, false);
	for (int i = 0; i < 50; ++i) {
		bricks[i] = Object2D::CreateRectangle("brick" + std::to_string(i), corner, brickLength, brickHeight, glm::vec3(1, 0, 0), true);
		AddMeshToList(bricks[i]);
	}

	player = Player();
	scaling.resize(50, 1.f);

	Mesh* powerup1 = Object2D::CreateSquare("powerup1", corner, squareSide / 8, glm::vec3(0.5f, 0.5f, 0), true);
	AddMeshToList(powerup1);

	Mesh* powerup2 = Object2D::CreateSquare("powerup2", corner, squareSide / 8, glm::vec3(1.f, 0.5f, 1.f), true);
	AddMeshToList(powerup2);

	Mesh* powerup3 = Object2D::CreateSquare("powerup3", corner, squareSide / 8, glm::vec3(0.5f, 0.5f, 0.7f), true);
	AddMeshToList(powerup3);

	Mesh* bottom = Object2D::CreateRectangle("bottom", corner, 1200, squareSide / 6, glm::vec3(1, 0, 1), true);
	AddMeshToList(bottom);

	// the very solid bricks, which are not destroyed at the first collision
	solidBricks.insert(std::make_pair(4, 0));
	solidBricks.insert(std::make_pair(42, 0));
	solidBricks.insert(std::make_pair(24, 0));
	solidBricks.insert(std::make_pair(30, 0));
	solidBricks.insert(std::make_pair(17, 0));
}

void Tema1::FrameStart() {
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}
void Tema1::Update(float deltaTimeSeconds) {
	glm::ivec2 resolution = window->GetResolution();
	modelMatrix = glm::mat3(1);
	modelMatrix *= Transform2D::Translate(0, 75);
	rotation += deltaTimeSeconds;
	modelMatrix *= Transform2D::Translate(translateX, translateY);
	RenderMesh2D(meshes["left"], shaders["VertexColor"], modelMatrix);

	modelMatrix = glm::mat3(1);
	modelMatrix *= Transform2D::Translate(0, 670);
	rotation += deltaTimeSeconds;
	modelMatrix *= Transform2D::Translate(translateX, translateY);
	RenderMesh2D(meshes["right"], shaders["VertexColor"], modelMatrix);

	modelMatrix = glm::mat3(1);
	modelMatrix *= Transform2D::Translate(1230, 75);
	rotation += deltaTimeSeconds;
	modelMatrix *= Transform2D::Translate(translateX, translateY);
	RenderMesh2D(meshes["top"], shaders["VertexColor"], modelMatrix);

	modelMatrix = glm::mat3(1);
	modelMatrix *= Transform2D::Translate(platformX + mouseMoveOX, platformY);
	rotation += deltaTimeSeconds;
	modelMatrix *= Transform2D::Translate(translateX, translateY);
	RenderMesh2D(meshes["platform"], shaders["VertexColor"], modelMatrix);

	//std::cout << initialBallPosX << std::endl;

	// collisions with walls
	if (hasGameBegun) {
		// if ball touches the platform
		if ((initialBallPosX >= platformX + mouseMoveOX)
			&& (initialBallPosX <= platformX + mouseMoveOX + 160) && initialBallPosY < 58.6 && signY == -1) {
			if (signX == -1)
				signX = 1;
			signY *= -1;
			float distance = platformX + mouseMoveOX + platformLength - initialBallPosX;
			angle = M_PI * distance / platformLength;
		}
		// if ball touches the top
		if ((initialBallPosY > 650) && (signY == 1)) {
			signY *= -1;
		}
		// if ball touches the right wall
		if ((initialBallPosX > 1220) && (signX * cos(angle)) > 0 && (initialBallPosY > 60)) {
			signX *= -1;
		}
		// if ball touches the left wall
		if ((initialBallPosX < 60) && (signX * cos(angle)) < 0 && (initialBallPosY > 60)) {
			signX *= -1;
		}
		initialBallPosY += signY * 300 * deltaTimeSeconds;
		initialBallPosX += signX * 320 * deltaTimeSeconds * cos(angle);
	}

	// collision with the wall which saves the ball
	if (hasGameBegun && savingWall && initialBallPosY < 45) {
		signY *= -1;
	}

	// if player lost the ball, he / she loses a life. If player lost all lives, game is restarted
	if (hasGameBegun && ((initialBallPosX < platformX + mouseMoveOX) 
		|| (initialBallPosX > platformX + mouseMoveOX + 160)) && initialBallPosY < 10 && !savingWall) {
		hasGameBegun = false;
		signX = 1;
		signY = 1;
		angle = M_PI / 2;
		initialBallPosX = 680 + mouseMoveOX;
		initialBallPosY = 58.7;
		player.decreaseLives();
		if ((player.getLives() == 0)) {
			player.setLives(3);
			hasGameBegun = false;
			solidBricks[4] = 0;
			solidBricks[42] = 0;
			solidBricks[24] = 0;
			solidBricks[30] = 0;
			solidBricks[17] = 0;
			for (int i = 0; i < 50; ++i) {
				blocksHit[i] = false;
				scaling[i] = 1.f;
			}
		}
	}

	// if player has won the game, the game restarts
	if (hasGameBegun) {
		if ((player.getLives() > 0 && AllBricksDissappeared())) {
			signX = 1;
			signY = 1;
			angle = M_PI / 2;
			initialBallPosX = 680 + mouseMoveOX;
			initialBallPosY = 58.7;
			player.setLives(3);
			solidBricks[4] = 0;
			solidBricks[42] = 0;
			solidBricks[24] = 0;
			solidBricks[30] = 0;
			solidBricks[17] = 0;
			hasGameBegun = false;
			for (int i = 0; i < 50; ++i) {
				blocksHit[i] = false;
				scaling[i] = 1.f;
			}
		}
	}

	// render of game ball
	modelMatrix = glm::mat3(1);
	if (!hasGameBegun) {
		modelMatrix *= Transform2D::Translate(680 + mouseMoveOX, initialBallPosY);
		initialBallPosX = 680 + mouseMoveOX;
	}
	else {
		modelMatrix *= Transform2D::Translate(initialBallPosX, initialBallPosY); 
	}
	modelMatrix *= Transform2D::Scale(10, 10);
	modelMatrix *= Transform2D::Translate(translateX, translateY);
	RenderMesh2D(meshes["ball"], shaders["VertexColor"], modelMatrix);

	// lifes
	if (player.getLives() > 0) {
		modelMatrix = glm::mat3(1);
		modelMatrix *= Transform2D::Translate(30, 30);
		modelMatrix *= Transform2D::Scale(13, 13);
		modelMatrix *= Transform2D::Translate(translateX, translateY);
		RenderMesh2D(meshes["life1"], shaders["VertexColor"], modelMatrix);
	}

	if (player.getLives() > 1) {
		modelMatrix = glm::mat3(1);
		modelMatrix *= Transform2D::Translate(65, 30);
		modelMatrix *= Transform2D::Scale(13, 13);
		modelMatrix *= Transform2D::Translate(translateX, translateY);
		RenderMesh2D(meshes["life2"], shaders["VertexColor"], modelMatrix);
	}

	if (player.getLives() > 2) {
		modelMatrix = glm::mat3(1);
		modelMatrix *= Transform2D::Translate(100, 30);
		modelMatrix *= Transform2D::Scale(13, 13);
		modelMatrix *= Transform2D::Translate(translateX, translateY);
		RenderMesh2D(meshes["life3"], shaders["VertexColor"], modelMatrix);
	}


	// power up #1 square
	if (blocksHit[27] && !savingWall && powerUpTime1 == 0.f) {
		modelMatrix = glm::mat3(1);
		modelMatrix *= Transform2D::Translate(powerUp1X, powerUp1Y);
		powerUp1Y -= 100 * deltaTimeSeconds;
		modelMatrix *= Transform2D::Scale(1.5, 1.5);
		powerUp1Rotation += 4 * deltaTimeSeconds;
		modelMatrix *= Transform2D::Rotate(powerUp1Rotation);
		modelMatrix *= Transform2D::Translate(-cx, -cy);
		RenderMesh2D(meshes["powerup1"], shaders["VertexColor"], modelMatrix);
		if ((powerUp1X >= platformX + mouseMoveOX)
			&& (powerUp1X <= platformX + mouseMoveOX + 160) && powerUp1Y < 58.6)
			savingWall = true;
	}

	// time for power up #1
	if (savingWall) {
		if (powerUpTime1 < 30.f)
			powerUpTime1 += deltaTimeSeconds;
		else {
			savingWall = false;
		}
	}

	// bottom wall for power up #1
	if (savingWall) {
		modelMatrix = glm::mat3(1);
		modelMatrix *= Transform2D::Translate(30, 5);
		modelMatrix *= Transform2D::Translate(translateX, translateY);
		RenderMesh2D(meshes["bottom"], shaders["VertexColor"], modelMatrix);
	}

	// power up #2
	if (blocksHit[35] && !noBrickReflection && powerUpTime2 == 0.f) {
		modelMatrix = glm::mat3(1);
		modelMatrix *= Transform2D::Translate(powerUp2X, powerUp2Y);
		powerUp2Y -= 100 * deltaTimeSeconds;
		modelMatrix *= Transform2D::Scale(1.5, 1.5);
		powerUp2Rotation += 4 * deltaTimeSeconds;
		modelMatrix *= Transform2D::Rotate(powerUp2Rotation);
		modelMatrix *= Transform2D::Translate(-cx, -cy);
		RenderMesh2D(meshes["powerup2"], shaders["VertexColor"], modelMatrix);
		if ((powerUp2X >= platformX + mouseMoveOX)
			&& (powerUp2X <= platformX + mouseMoveOX + 160) && powerUp2Y < 58.6)
			noBrickReflection = true;
	}

	// time for power up #2
	if (noBrickReflection) {
		if (powerUpTime2 < 30.f)
			powerUpTime2 += deltaTimeSeconds;
		else {
			noBrickReflection = false;
		}
	}

	for (int i = 0; i < 10; ++i) {
		if (!blocksHit[i]) {
			modelMatrix = glm::mat3(1);
			modelMatrix *= Transform2D::Translate(250 + 80 * i, 270);
			modelMatrix *= Transform2D::Translate(translateX, translateY);
			RenderMesh2D(meshes["brick" + std::to_string(i)], shaders["VertexColor"], modelMatrix);
		} 
		
		if (hasGameBegun) {
			if (!blocksHit[i]) {
				if (initialBallPosX >= (250 + 80 * i) && initialBallPosX <= (250 + 80 * i + brickLength) && 
						(fabs(initialBallPosY - 270) < 3.f || fabs(initialBallPosY - 270 - brickHeight) < 3.f)) {
					// 4 is the very solid brick
					if (i == 4) {
						solidBricks[i]++;
					}
					if (i != 4)
						blocksHit[i] = true;
					if (i == 4 && solidBricks[i] == 3)
						blocksHit[i] = true;
					if (i == 4 && noBrickReflection)
						blocksHit[i] = true;
					if (!noBrickReflection)
						signY *= -1;
				}

				if (initialBallPosY >= 270 && initialBallPosY <= 270 + brickHeight 
						&& (fabs(initialBallPosX - (250 + 80 * i)) < 3.f || fabs(initialBallPosX - (250 + 80 * i + brickLength)) < 3.f)) {
					if (i == 4) {
						solidBricks[i]++;
					}
					if (i != 4)
						blocksHit[i] = true;
					if (i == 4 && solidBricks[i] == 3)
						blocksHit[i] = true;
					if (i == 4 && noBrickReflection)
						blocksHit[i] = true;
					if (!noBrickReflection)
						signX *= -1;
				}
			}
			else {
				if (scaling[i] > 0.f) {
					scaling[i] -= 0.05f;
					modelMatrix = glm::mat3(1);
					modelMatrix *= Transform2D::Translate(250 + 80 * i + 20, 270 + 30);
					modelMatrix *= Transform2D::Scale(scaling[i], scaling[i]);
					modelMatrix *= Transform2D::Translate(-20, -30);
					RenderMesh2D(meshes["brick" + std::to_string(i)], shaders["VertexColor"], modelMatrix);
				}
			}
		}
	}

	for (int i = 10; i < 20; ++i) {
		if (!blocksHit[i]) {
			modelMatrix = glm::mat3(1);
			modelMatrix *= Transform2D::Translate(250 + 80 * (i - 10), 330);
			modelMatrix *= Transform2D::Translate(translateX, translateY);
			RenderMesh2D(meshes["brick" + std::to_string(i)], shaders["VertexColor"], modelMatrix);
		}
		if (hasGameBegun) {
			if (!blocksHit[i]) {
				if (initialBallPosX >= (250 + 80 * (i - 10)) && initialBallPosX <= (250 + 80 * (i - 10) + brickLength) &&
					(fabs(initialBallPosY - 330) < 3.f || fabs(initialBallPosY - 330 - brickHeight) < 3.f)) {
					// 17 is the very solid brick
					if (i == 17) {
						solidBricks[i]++;
					}
					if (i != 17)
						blocksHit[i] = true;
					if (i == 17 && solidBricks[i] == 3)
						blocksHit[i] = true;
					if (i == 17 && noBrickReflection)
						blocksHit[i] = true;
					if (!noBrickReflection)
						signY *= -1;
				}

				if (initialBallPosY >= 330 && initialBallPosY <= 330 + brickHeight
					&& (fabs(initialBallPosX - (250 + 80 * (i - 10))) < 3.f || fabs(initialBallPosX - (250 + 80 * (i - 10) + brickLength)) < 3.f)) {
					// 17 is the very solid brick
					if (i == 17) {
						solidBricks[i]++;
					}
					if (i != 17)
						blocksHit[i] = true;
					if (i == 17 && solidBricks[i] == 3)
						blocksHit[i] = true;
					if (i == 17 && noBrickReflection)
						blocksHit[i] = true;
					if (!noBrickReflection)
						signX *= -1;
				}
			} else {
				if (scaling[i] > 0.f) {
					scaling[i] -= 0.05f;
					modelMatrix = glm::mat3(1);
					modelMatrix *= Transform2D::Translate(250 + 80 * (i - 10) + 20, 330 + 30);
					modelMatrix *= Transform2D::Scale(scaling[i], scaling[i]);
					modelMatrix *= Transform2D::Translate(-20, -30);
					RenderMesh2D(meshes["brick" + std::to_string(i)], shaders["VertexColor"], modelMatrix);
				}
			}
		}
	}

	for (int i = 20; i < 30; ++i) {
		if (!blocksHit[i]) {
			modelMatrix = glm::mat3(1);
			modelMatrix *= Transform2D::Translate(250 + 80 * (i - 20), 390);
			modelMatrix *= Transform2D::Translate(translateX, translateY);
			RenderMesh2D(meshes["brick" + std::to_string(i)], shaders["VertexColor"], modelMatrix);
		}
		if (hasGameBegun) {
			if (!blocksHit[i]) {
				if (initialBallPosX >= (250 + 80 * (i - 20)) && initialBallPosX <= (250 + 80 * (i - 20) + brickLength) &&
					(fabs(initialBallPosY - 390) < 3.f || fabs(initialBallPosY - 390 - brickHeight) < 3.f)) {
					// 24 is the very solid brick
					if (i == 24) {
						solidBricks[i]++;
					}
					if (i != 24)
						blocksHit[i] = true;
					if (i == 24 && solidBricks[i] == 3)
						blocksHit[i] = true;
					if (i == 24 && noBrickReflection)
						blocksHit[i] = true;
					if (!noBrickReflection)
						signY *= -1;
				}

				if (initialBallPosY >= 390 && initialBallPosY <= 390 + brickHeight
					&& (fabs(initialBallPosX - (250 + 80 * (i - 20))) < 3.f || fabs(initialBallPosX - (250 + 80 * (i - 20) + brickLength)) < 3.f)) {
					// 24 is the very solid brick
					if (i == 24) {
						solidBricks[i]++;
					}
					if (i != 24)
						blocksHit[i] = true;
					if (i == 24 && solidBricks[i] == 3)
						blocksHit[i] = true;
					if (i == 24 && noBrickReflection)
						blocksHit[i] = true;
					if (!noBrickReflection)
						signX *= -1;
				}
			} else {
				if (scaling[i] > 0.f) {
					scaling[i] -= 0.05f;
					modelMatrix = glm::mat3(1);
					modelMatrix *= Transform2D::Translate(250 + 80 * (i - 20) + 20, 390 + 30);
					modelMatrix *= Transform2D::Scale(scaling[i], scaling[i]);
					modelMatrix *= Transform2D::Translate(-20, -30);
					RenderMesh2D(meshes["brick" + std::to_string(i)], shaders["VertexColor"], modelMatrix);
				}
			}
		}
	}

	for (int i = 30; i < 40; ++i) {
		if (!blocksHit[i]) {
			modelMatrix = glm::mat3(1);
			modelMatrix *= Transform2D::Translate(250 + 80 * (i - 30), 450);
			modelMatrix *= Transform2D::Translate(translateX, translateY);
			RenderMesh2D(meshes["brick" + std::to_string(i)], shaders["VertexColor"], modelMatrix);
		}
		if (hasGameBegun) {
			if (!blocksHit[i]) {
				if (initialBallPosX >= (250 + 80 * (i - 30)) && initialBallPosX <= (250 + 80 * (i - 30) + brickLength) &&
					(fabs(initialBallPosY - 450) < 3.f || fabs(initialBallPosY - 450 - brickHeight) < 3.f)) {
					// 30 is the very solid brick
					if (i == 30) {
						solidBricks[i]++;
					}
					if (i != 30)
						blocksHit[i] = true;
					if (i == 30 && solidBricks[i] == 3)
						blocksHit[i] = true;
					if (i == 30 && noBrickReflection)
						blocksHit[i] = true;
					if (!noBrickReflection)
						signY *= -1;
				}

				if (initialBallPosY >= 450 && initialBallPosY <= 450 + brickHeight
					&& (fabs(initialBallPosX - (250 + 80 * (i - 30))) < 3.f || fabs(initialBallPosX - (250 + 80 * (i - 30) + brickLength)) < 3.f)) {
					// 30 is the very solid brick
					if (i == 30) {
						solidBricks[i]++;
					}
					if (i != 30)
						blocksHit[i] = true;
					if (i == 30 && solidBricks[i] == 3)
						blocksHit[i] = true;
					if (i == 30 && noBrickReflection)
						blocksHit[i] = true;
					if (!noBrickReflection)
						signX *= -1;
				}
			} else {
				if (scaling[i] > 0.f) {
					scaling[i] -= 0.05f;
					modelMatrix = glm::mat3(1);
					modelMatrix *= Transform2D::Translate(250 + 80 * (i - 30) + 20, 450 + 30);
					modelMatrix *= Transform2D::Scale(scaling[i], scaling[i]);
					modelMatrix *= Transform2D::Translate(-20, -30);
					RenderMesh2D(meshes["brick" + std::to_string(i)], shaders["VertexColor"], modelMatrix);
				}
			}
		}
	}

	for (int i = 40; i < 50; ++i) {
		if (!blocksHit[i]) {
			modelMatrix = glm::mat3(1);
			modelMatrix *= Transform2D::Translate(250 + 80 * (i - 40), 510);
			modelMatrix *= Transform2D::Translate(translateX, translateY);
			RenderMesh2D(meshes["brick" + std::to_string(i)], shaders["VertexColor"], modelMatrix);
		}
		if (hasGameBegun) {
			if (!blocksHit[i]) {
				if (initialBallPosX >= (250 + 80 * (i - 40)) && initialBallPosX <= (250 + 80 * (i - 40) + brickLength) &&
					(fabs(initialBallPosY - 510) < 3.f || fabs(initialBallPosY - 510 - brickHeight) < 3.f)) {
					// 42 is the very solid brick
					if (i == 42) {
						solidBricks[i]++;
					}
					if (i != 42)
						blocksHit[i] = true;
					if (i == 42 && solidBricks[i] == 3)
						blocksHit[i] = true;
					if (i == 42 && noBrickReflection)
						blocksHit[i] = true;
					if (!noBrickReflection)
						signY *= -1;
				}

				if (initialBallPosY >= 510 && initialBallPosY <= 510 + brickHeight
					&& (fabs(initialBallPosX - (250 + 80 * (i - 40))) < 3.f || fabs(initialBallPosX - (250 + 80 * (i - 40) + brickLength)) < 3.f)) {
					// 42 is the very solid brick
					if (i == 42) {
						solidBricks[i]++;
					}
					if (i != 42)
						blocksHit[i] = true;
					if (i == 42 && solidBricks[i] == 3)
						blocksHit[i] = true;
					if (i == 42 && noBrickReflection)
						blocksHit[i] = true;
					if (!noBrickReflection)
						signX *= -1;
				}
			} else {
				if (scaling[i] > 0.f) {
					scaling[i] -= 0.05f;
					modelMatrix = glm::mat3(1);
					modelMatrix *= Transform2D::Translate(250 + 80 * (i - 40) + 20, 510 + 30);
					modelMatrix *= Transform2D::Scale(scaling[i], scaling[i]);
					modelMatrix *= Transform2D::Translate(-20, -30);
					RenderMesh2D(meshes["brick" + std::to_string(i)], shaders["VertexColor"], modelMatrix);
				}
			}
		}
	}
}
void Tema1::FrameEnd() {

}
void Tema1::DrawScene(glm::mat3 visMatrix) {

}

void Tema1::OnInputUpdate(float deltaTime, int mods) {

}
void Tema1::OnKeyPress(int key, int mods) {

}
void Tema1::OnKeyRelease(int key, int mods) {

}
void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {
	mouseMoveOX += deltaX;
}
void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) {
	if (!hasGameBegun) {
		if ((initialBallPosX <= 1220 && initialBallPosX >= 58))
			hasGameBegun = true;
	}
	else if (hasGameBegun && ((player.getLives() == 0) || (player.getLives() > 0 && AllBricksDissappeared()))) {
		hasGameBegun = false;
	}
}
void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) {

}
void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) {

}