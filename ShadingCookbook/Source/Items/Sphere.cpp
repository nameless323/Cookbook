#include "Sphere.h"

inline Sphere::Sphere(float radius, GLuint slices, GLuint stacks) : _radius(radius), _slices(slices), _stacks(_stacks)
{
	_nVerts = (slices + 1) * (stacks + 1);
	_elements = (slices * 2 * (stacks - 1)) * 3;

	float* v = new float[3 * _nVerts];
	float* n = new float[3 * _nVerts];
	float* tex = new float[2 * _nVerts];
	unsigned int* el = new unsigned int[_elements];

	GenerateVerts(v, n, tex, el);
	unsigned int handles[4];
	glGenBuffers(4, handles);

	glBindBuffer(GL_ARRAY_BUFFER, handles[0]);
	glBufferData(GL_ARRAY_BUFFER, (3 * _nVerts) * sizeof(float), v, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, handles[1]);
	glBufferData(GL_ARRAY_BUFFER, (3 * _nVerts) * sizeof(float), n, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, handles[2]);
	glBufferData(GL_ARRAY_BUFFER, (2 * _nVerts) * sizeof(float), tex, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handles[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _elements * sizeof(unsigned int), el, GL_STATIC_DRAW);

	delete[] v;
	delete[] n;
	delete[] tex;
	delete[] el;

	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, handles[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, handles[1]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, handles[2]);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handles[3]);

	glBindVertexArray(0);
}

void Sphere::Render()
{
	glBindVertexArray(_vao);
	glDrawElements(GL_TRIANGLES, _elements, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}

int Sphere::GetVAOHandle()
{
	return _vao;
}

void Sphere::GenerateVerts(float* verts, float* norms, float* tex, unsigned* el)
{
}