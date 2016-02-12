#include "Skybox.h"

Skybox::Skybox()
{
	float side = 50.0f;
	float sideHalf = side * 0.5f;

	float verts[24 * 3] = {
		// Front
		-sideHalf, -sideHalf, sideHalf,
		sideHalf, -sideHalf, sideHalf,
		sideHalf, sideHalf, sideHalf,
		-sideHalf, sideHalf, sideHalf,
		// Right
		sideHalf, -sideHalf, sideHalf,
		sideHalf, -sideHalf, -sideHalf,
		sideHalf, sideHalf, -sideHalf,
		sideHalf, sideHalf, sideHalf,
		// Back
		-sideHalf, -sideHalf, -sideHalf,
		-sideHalf, sideHalf, -sideHalf,
		sideHalf, sideHalf, -sideHalf,
		sideHalf, -sideHalf, -sideHalf,
		// Left
		-sideHalf, -sideHalf, sideHalf,
		sideHalf, sideHalf, sideHalf,
		-sideHalf, sideHalf, -sideHalf,
		-sideHalf, -sideHalf, -sideHalf,
		// Bottom
		-sideHalf, -sideHalf, sideHalf,
		-sideHalf, -sideHalf, -sideHalf,
		sideHalf, -sideHalf, -sideHalf,
		sideHalf, -sideHalf, sideHalf,
		// Top
		-sideHalf, sideHalf, sideHalf,
		sideHalf, sideHalf, sideHalf,
		sideHalf, sideHalf, -sideHalf,
		-sideHalf, sideHalf, -sideHalf
	};
	GLubyte indices[] =
		{
			0,2,1,0,3,2,
			4,6,5,4,7,6,
			8,10,9,8,11,10,
			12,14,13,12,15,14,
			16,18,17,16,19,18,
			20,22,21,20,23,22
		};

	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	GLuint handles[2];
	glGenBuffers(2, handles);

	glBindBuffer(GL_ARRAY_BUFFER, handles[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handles[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void Skybox::Render()
{
	glBindVertexArray(_vao);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, nullptr);
}

