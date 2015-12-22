#include "Plane.h"

Plane::Plane(float xSize, float zSize, int xDivs, int zDivs, float sMax, float tMax)
{
	_faces = xDivs * zDivs;
	float* v = new float[3 * (xDivs + 1) * (zDivs + 1)];
	float* n = new float[3 * (xDivs + 1) * (zDivs + 1)];
	float* tex = new float[2 * (xDivs + 1) * (zDivs + 1)];
	unsigned int* el = new unsigned int[6 * xDivs * zDivs];

	float x2 = xSize / 2.0f;
	float z2 = zSize / 2.0f;

	float iFactor = (float)zSize / zDivs;
	float jFactor = (float)xSize / xDivs;
	float texi = sMax / zDivs;
	float texj = tMax / xDivs;
	float x, z;
	int vIdx = 0, tIdx = 0;
	for (int i = 0; i <= zDivs; i++)
	{
		z = iFactor * i - z2;
		for (int j = 0; j <= xDivs; j++)
		{
			x = jFactor * j - x2;
			v[vIdx] = x;
			v[vIdx + 1] = 0.0f;
			v[vIdx + 2] = z;
			
			n[vIdx] = 0.0f;
			n[vIdx + 1] = 1.0f;
			n[vIdx + 2] = 0.0;

			vIdx += 3;
			tex[tIdx] = j * texi;
			tex[tIdx + 1] = i * texj;

			tIdx += 2;
		}
	}

	unsigned int rowStart, nextRowStart;
	int idx = 0;
	for (int i = 0; i < zDivs; i++)
	{
		rowStart = i * (xDivs + 1);
		nextRowStart = (i + 1) * (xDivs + 1);
		for (int j = 0; j < xDivs; j++)
		{
			el[idx] = rowStart + j;
			el[idx + 1] = nextRowStart + j;
			el[idx + 2] = nextRowStart + j + 1;
			el[idx + 3] = rowStart + j;
			el[idx + 4] = nextRowStart + j + 1;
			el[idx + 5] = rowStart + j + 1;
			idx += 6;
		}
	}

	unsigned int vbo[4];
	glGenBuffers(4, vbo);
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, 3 * (xDivs + 1) * (zDivs + 1) * sizeof(float), v, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, 3 * (xDivs + 1) * (zDivs + 1) * sizeof(float), n, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, 2 * (xDivs + 1) * (zDivs + 1) * sizeof(float), tex, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * xDivs * zDivs * sizeof(unsigned int), el, GL_STATIC_DRAW);

	glBindVertexArray(0);
	delete[] v;
	delete[] n;
	delete[] tex;
	delete[] el;
}

void Plane::Render()
{
	glBindVertexArray(_vao);
	glDrawElements(GL_TRIANGLES, 6 * _faces, GL_UNSIGNED_INT, nullptr);
}