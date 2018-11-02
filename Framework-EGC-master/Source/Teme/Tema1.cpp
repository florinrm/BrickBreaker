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
	std::cout << resolution.x;

	glm::vec3 corner = glm::vec3(0, 0, 0);
	float squareSide = 100;

	// compute coordinates of square center
	cx = corner.x + squareSide / 2;
	cy = corner.y + squareSide / 2;

	// initialize tx and ty (the translation steps)
	translateX = 0;
	translateY = 0;

	// initialize sx and sy (the scale factors)
	scaleX = 1;
	scaleY = 1;

	// initialize angularStep
	angularStep = 0;


	Mesh* square1 = Object2D::CreateRectangle("left", corner, squareSide * 2 / 4, resolution.y, glm::vec3(1, 0, 0), true);
	AddMeshToList(square1);

	Mesh* square2 = Object2D::CreateRectangle("right", corner, resolution.x, squareSide / 2, glm::vec3(1, 0, 0), true);
	AddMeshToList(square2);

	Mesh* square3 = Object2D::CreateRectangle("bottom", corner, squareSide * 2 / 4, resolution.y, glm::vec3(1, 0, 0), true);
	AddMeshToList(square3);

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
	brickStatus.resize(50, 0);
	scaleTimes.resize(50, 0.0F);
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
	RenderMesh2D(meshes["bottom"], shaders["VertexColor"], modelMatrix);

	modelMatrix = glm::mat3(1);
	modelMatrix *= Transform2D::Translate(platformX + mouseMoveOX, platformY);
	rotation += deltaTimeSeconds;
	modelMatrix *= Transform2D::Translate(translateX, translateY);
	RenderMesh2D(meshes["platform"], shaders["VertexColor"], modelMatrix);

	// collisions with walls
	if (hasGameBegun) {
		if ((initialBallPosX >= platformX + mouseMoveOX)
			&& (initialBallPosX <= platformX + mouseMoveOX + 160) && initialBallPosY < 58.6 && signY == -1) {
			
			signY *= -1;
			float distance = platformX + mouseMoveOX + platformLength - initialBallPosX;
			angle = M_PI * distance / platformLength;
			if (signX == -1)
				signX = 1;
		}
		if ((initialBallPosY > 650) && (signY == 1)) {
			signY *= -1;
		}
		if ((initialBallPosX > 1220) && (signX * cos(angle)) > 0 && (initialBallPosY > 60)) {
			signX *= -1;
		}
		if ((initialBallPosX < 60) && (signX * cos(angle)) < 0 && (initialBallPosY > 60)) {
			signX *= -1;
		}
		std::cout << signX << std::endl;
		initialBallPosY += signY * 200 * deltaTimeSeconds;
		initialBallPosX += signX * 200 * deltaTimeSeconds * cos(angle);
	}

	if (hasGameBegun && ((initialBallPosX < platformX + mouseMoveOX) 
		|| (initialBallPosX > platformX + mouseMoveOX + 160)) && initialBallPosY < 58.5) {
		hasGameBegun = false;
		player.decreaseLives();
		if ((player.getLives() == 0)) {
			signX = 1;
			signY = 1;
			initialBallPosX = 680 + mouseMoveOX;
			initialBallPosY = 58.7;
			player.setLives(3);
			hasGameBegun = false;
			for (int i = 0; i < 50; ++i)
				blocksHit[i] = false;
		}
	}

	if (hasGameBegun) {
		if ((player.getLives() > 0 && AllBricksDissappeared())) {
			signX = 1;
			signY = 1;
			initialBallPosX = 680 + mouseMoveOX;
			initialBallPosY = 58.7;
			player.setLives(3);
			hasGameBegun = false;
			for (int i = 0; i < 50; ++i)
				blocksHit[i] = false;
		}
	}

	modelMatrix = glm::mat3(1);
	if (!hasGameBegun) {
		modelMatrix *= Transform2D::Translate(680 + mouseMoveOX, initialBallPosY);
		initialBallPosX = 680 + mouseMoveOX;
	}
	else {
		modelMatrix *= Transform2D::Translate(initialBallPosX, initialBallPosY); 
	}
	modelMatrix *= Transform2D::Scale(13, 13);
	modelMatrix *= Transform2D::Translate(translateX, translateY);
	RenderMesh2D(meshes["ball"], shaders["VertexColor"], modelMatrix);

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
						(fabs(initialBallPosY - 270) < 3 || fabs(initialBallPosY - 270 - brickHeight) < 3)) {
					blocksHit[i] = true;
					signY *= -1;
				}

				if (initialBallPosY >= 270 && initialBallPosY <= 270 + brickHeight 
						&& (fabs(initialBallPosX - (250 + 80 * i)) < 3 || fabs(initialBallPosX - (250 + 80 * i + brickLength)) < 3)) {
					blocksHit[i] = true;
					signX *= -1;
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
					(fabs(initialBallPosY - 330) < 3 || fabs(initialBallPosY - 330 - brickHeight) < 3)) {
					blocksHit[i] = true;
					signY *= -1;
				}

				if (initialBallPosY >= 330 && initialBallPosY <= 330 + brickHeight
					&& (fabs(initialBallPosX - (250 + 80 * (i - 10))) < 3 || fabs(initialBallPosX - (250 + 80 * (i - 10) + brickLength)) < 3)) {
					blocksHit[i] = true;
					signX *= -1;
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
					(fabs(initialBallPosY - 390) < 3 || fabs(initialBallPosY - 390 - brickHeight) < 3)) {
					blocksHit[i] = true;
					signY *= -1;
				}

				if (initialBallPosY >= 390 && initialBallPosY <= 390 + brickHeight
					&& (fabs(initialBallPosX - (250 + 80 * (i - 20))) < 3 || fabs(initialBallPosX - (250 + 80 * (i - 20) + brickLength)) < 3)) {
					blocksHit[i] = true;
					signX *= -1;
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
					(fabs(initialBallPosY - 450) < 3 || fabs(initialBallPosY - 450 - brickHeight) < 3)) {
					blocksHit[i] = true;
					signY *= -1;
				}

				if (initialBallPosY >= 450 && initialBallPosY <= 450 + brickHeight
					&& (fabs(initialBallPosX - (250 + 80 * (i - 30))) < 3 || fabs(initialBallPosX - (250 + 80 * (i - 30) + brickLength)) < 3)) {
					blocksHit[i] = true;
					signX *= -1;
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
					(fabs(initialBallPosY - 510) < 3 || fabs(initialBallPosY - 510 - brickHeight) < 3)) {
					blocksHit[i] = true;
					signY *= -1;
				}

				if (initialBallPosY >= 510 && initialBallPosY <= 510 + brickHeight
					&& (fabs(initialBallPosX - (250 + 80 * (i - 40))) < 3 || fabs(initialBallPosX - (250 + 80 * (i - 40) + brickLength)) < 3)) {
					blocksHit[i] = true;
					signX *= -1;
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
		hasGameBegun = true;
	}
	else if (hasGameBegun && player.getLives() == 0) {
		hasGameBegun = false;
	}
	//std::cout << "muie muie" << std::endl;
}
void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) {

}
void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) {

}