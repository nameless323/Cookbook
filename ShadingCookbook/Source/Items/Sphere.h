#pragma once

#include "IDrawable.h"
#include "GL/glew.h"

class Sphere : IDrawable
{
public:
	Sphere(float radius, GLuint slices, GLuint stacks);
	void Render() override;
	int GetVAOHandle();
private:
	GLuint _vao;
	GLuint _nVerts, _elements;
	GLuint _slices, _stacks;
	float _radius;

	void GenerateVerts(float* verts, float* norms, float* tex, unsigned int* el);
};