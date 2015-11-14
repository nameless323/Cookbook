#pragma once

#include "IDrawable.h"
#include <GL/glew.h>

class Torus : public IDrawable
{
public:
	Torus(float outerRadius, float innerRadius, int nSides, int nRings);
	virtual void Render() override;
	int GetVertexArrayHandle();
private:
	GLuint _vaoHandle;
	int _faces, _rings, _sides;

	void GenerateVerts(float* verts, float* norms, float* tex, unsigned int* el, float outerRadius, float innerRadius);

};