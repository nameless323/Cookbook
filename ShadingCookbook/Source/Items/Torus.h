#pragma once

#include "IDrawable.h"

class Torus : public IDrawable
{
public:
	Torus(float outerRadius, float innerRadius, int nSides, int nRings);
	void Render() const;
	int GetVertexArrayHandle();
private:
	unsigned int _vaoHandle;
	int _faces, _rings, _sides;

	void GenerateVerts(float* verts, float* norms, float* tex, unsigned int* el, float outerRadius, float innerRadius);


};