#include "Torus.h"
#include <GL/glew.h>

#include <gtc/constants.hpp>

Torus::Torus(float outerRadius, float innerRadius, int nSides, int nRings) : _rings(nRings), _sides(nSides)
{
	_faces = _sides * _rings;
	int nVerts = _sides * (_rings + 1);

	float* v = new float[3 * nVerts];
	float* n = new float[3 * nVerts];
	float* uv = new float[2 * nVerts];

	unsigned int* el = new unsigned int[6 * _faces];

	GenerateVerts(v, n, uv, el, outerRadius, innerRadius);

	unsigned int buffersHandle[4];

	glGenBuffers(4, buffersHandle);

	glBindBuffer(GL_ARRAY_BUFFER, buffersHandle[0]);
	glBufferData(GL_ARRAY_BUFFER, (3 * nVerts) * sizeof(float), v, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, buffersHandle[1]);
	glBufferData(GL_ARRAY_BUFFER, (3 * nVerts) * sizeof(float), n, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, buffersHandle[2]);
	glBufferData(GL_ARRAY_BUFFER, (2 * nVerts)*sizeof(float), uv, GL_STATIC_DRAW);

	delete[] v;
	delete[] n;
	delete[] uv;

	glGenVertexArrays(1, &_vaoHandle);
	glBindVertexArray(_vaoHandle);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, buffersHandle[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, buffersHandle[1]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, buffersHandle[2]);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffersHandle[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(el), el, GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void Torus::Render()
{
	glBindVertexArray(_vaoHandle);
	glDrawElements(GL_TRIANGLES, 6 * _faces, GL_UNSIGNED_INT, nullptr); //last parameter is offset from buffer
}

int Torus::GetVertexArrayHandle()
{
	return _vaoHandle;
}

void Torus::GenerateVerts(float* verts, float* norms, float* tex, unsigned* el, float outerRadius, float innerRadius)
{
	float ringFactor = glm::two_pi<float>() / _rings;
	float sideFactor = glm::two_pi<float>() / _sides;
	int idx = 0, tidx = 0;
	for (int ring = 0; ring <= _rings; ring++)
	{
		float u = ring*ringFactor;
		float cu = cosf(u);
		float su = sinf(u);
		for (int side = 0; side < _sides; side++)
		{
			float v = side * sideFactor;
			float cv = cosf(v);
			float sv = sinf(v);
			float r = (outerRadius + innerRadius*cv);
			verts[idx] = r * cu;
			verts[idx + 1] = r * su;
			verts[idx + 2] = innerRadius*sv;

			norms[idx] = cv * su * r;
			norms[idx + 1] = cv * su * r;
			norms[idx + 2] = sv * r;

			tex[tidx] = u / glm::two_pi<float>();
			tex[tidx + 1] = v / glm::two_pi<float>();
			tidx += 2;
			float len = sqrt(norms[idx] * norms[idx] + norms[idx + 1] * norms[idx + 1] + norms[idx + 2] * norms[idx + 2]);
			norms[idx] /= len;
			norms[idx + 1] /= len;
			norms[idx + 2] /= len;
			idx += 3;
		}
	}
	idx = 0;
	for (int ring = 0; ring < _rings; ring++)
	{
		int ringStart = ring*_sides;
		int nextRingStart = (ring + 1) * _sides;
		for (int side = 0; side < _sides; side++)
		{
			int nextSide = (side + 1) % _sides;
			el[idx] = (ringStart + side);
			el[idx + 1] = (nextRingStart + side);
			el[idx + 2] = (nextRingStart + nextSide);
			el[idx + 3] = (ringStart + side);
			el[idx + 4] = (nextRingStart + nextSide);
			el[idx + 5] = (ringStart + nextSide);
			idx += 6;
		}
	}
}
