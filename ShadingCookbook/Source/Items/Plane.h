#pragma once

#include "IDrawable.h"
#include "GL/glew.h"

class Plane : public IDrawable
{
public:
	Plane(float xSize, float ySize, int dDivs, int zDivs, float sMax, float tMax);
	void Render() override;
private:
	unsigned int _vao;
	int _faces;
};