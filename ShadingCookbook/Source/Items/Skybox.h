#pragma once
#include "IDrawable.h"
#include "GL/glew.h"

class Skybox : public IDrawable
{
public:
	Skybox();
	void Render() override;
	//todo shutdown
private:
	GLuint _vao;
};

