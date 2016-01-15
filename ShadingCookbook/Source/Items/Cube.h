#pragma once
#include "IDrawable.h"
#include <GL\glew.h>

class Cube : IDrawable
{
public:
	Cube();
	void Render() override;
private:
	GLuint _handle;
};