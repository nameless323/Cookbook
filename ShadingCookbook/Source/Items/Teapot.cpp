#include "Teapot.h"

Teapot::Teapot(int grid, const mat4& lidTransform)
{
	int verts = 32 * (grid + 1) * (grid + 1);
	_faces = grid * grid * 32;
	float* v = new float[verts * 3];
	float* n = new float[verts * 3];
	float* tc = new float[verts * 2];
	unsigned int* el = new unsigned int[_faces * 6];

	glGenVertexArrays(1, &_vaoHandle);
	glBindVertexArray(_vaoHandle);

	unsigned int handle[4];
	glGenBuffers(4, handle);

	GeneratePatches(v, n, tc, el, grid);
	MoveLid(grid, v, lidTransform);

	glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
	glBufferData(GL_ARRAY_BUFFER, (3 * verts) * sizeof(float), v, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
	glBufferData(GL_ARRAY_BUFFER, (3 * verts) * sizeof(float), n, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, handle[2]);
	glBufferData(GL_ARRAY_BUFFER, 2 * verts * sizeof(float), tc, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _faces * 6 * sizeof(unsigned int), el, GL_STATIC_DRAW);

	delete[] v;
	delete[] n;
	delete[] tc;
	delete[] el;

	glBindVertexArray(0);

}

void Teapot::Render()
{
	glBindVertexArray(_vaoHandle);
	glDrawElements(GL_TRIANGLES, 6 * _faces, GL_UNSIGNED_INT, nullptr);
}

void Teapot::GeneratePatches(float* v, float* n, float* tc, unsigned* el, int grid)
{
}

void Teapot::BuildPatchReflect(int patchNum, float* b, float* dB, float* v, float* n, float* tc, unsigned* el, int& index, int& elIndex, int& tcIndex, int grid, bool reflectX, bool reflectY)
{
}

void Teapot::BuildPatch(vec3 patch[][4], float* b, float* dB, float* v, float* n, float* tc, unsigned* el, int& index, int& elIndex, int& tcIndex, int grid, mat3 reflect, bool invertNormal)
{
}

void Teapot::GetPatch(int patchNum, vec3 patch[][4], bool reverseV)
{
}

void Teapot::ComputeBasisFunctions(float* b, float* dB, int grid)
{
}

vec3 Teapot::Evaluate(int gridU, int gridV, float* b, float* dB, vec3 patch[][4])
{
}

void Teapot::MoveLid(int grid, float* v, const mat4& lidTransform)
{
}