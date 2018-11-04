#include "Object2D.h"

#include <Core/Engine.h>

Mesh* Object2D::CreateSquare(std::string name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill)
{
	glm::vec3 corner = leftBottomCorner;

	std::vector<VertexFormat> vertices =
	{
		VertexFormat(corner, color),
		VertexFormat(corner + glm::vec3(length, 0, 0), color),
		VertexFormat(corner + glm::vec3(length, length, 0), color),
		VertexFormat(corner + glm::vec3(0, length, 0), color)
	};

	Mesh* square = new Mesh(name);
	std::vector<unsigned short> indices = { 0, 1, 2, 3 };
	
	if (!fill) {
		square->SetDrawMode(GL_LINE_LOOP);
	}
	else {
		// draw 2 triangles. Add the remaining 2 indices
		indices.push_back(0);
		indices.push_back(2);
		indices.push_back(4);
		indices.push_back(2);
	}

	square->InitFromData(vertices, indices);
	return square;
}

Mesh* Object2D::CreateRectangle(std::string name, glm::vec3 leftBottomCorner, float length, float width, glm::vec3 color, bool fill) {

	glm::vec3 corner = leftBottomCorner;

	std::vector<VertexFormat> vertices =
	{
		VertexFormat(corner, color),
		VertexFormat(corner + glm::vec3(length, 0, 0), color),
		VertexFormat(corner + glm::vec3(length, width, 0), color),
		VertexFormat(corner + glm::vec3(0, width, 0), color)
	};

	Mesh* square = new Mesh(name);
	std::vector<unsigned short> indices = { 0, 1, 2, 3 };

	if (!fill) {
		square->SetDrawMode(GL_LINE_LOOP);
	}
	else {
		// draw 2 triangles. Add the remaining 2 indices
		indices.push_back(0);
		indices.push_back(2);
		indices.push_back(4);
		indices.push_back(2);
	}

	square->InitFromData(vertices, indices);
	return square;
}

Mesh* Object2D::CreateCircle(std::string name, glm::vec3 leftBottomCorner, int nr_triangles, glm::vec3 color)
{
	glm::vec3 corner = leftBottomCorner;

	std::vector<VertexFormat> vertices_circle;

	vertices_circle.push_back(VertexFormat(corner, color));
	for (int i = 0; i < nr_triangles; i++) 
	{
		float angle = 2.0f * M_PI * float(i) / float(nr_triangles);

		float x = cosf(angle), y = sinf(angle);

		vertices_circle.push_back(VertexFormat(corner + glm::vec3(x, y, 0), color));
	}

	std::vector<ushort> circle_indices;

	for (int i = 2; i <= nr_triangles; i++)
	{
		circle_indices.push_back(0);
		circle_indices.push_back(i - 1);
		circle_indices.push_back(i);
	};

	// add the last triangle
	circle_indices.push_back(0);
	circle_indices.push_back(nr_triangles);
	circle_indices.push_back(1);

	Mesh* circle = new Mesh(name);

	circle->InitFromData(vertices_circle, circle_indices);
	return circle;
}