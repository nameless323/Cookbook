#pragma once

#include "../../Core/ShaderProgram.h"
#include "../Scene.h"
#include "../../Items/Torus.h"
#include "../../Items/Teapot.h"

#include <glm.hpp>
using glm::mat4;

class DiffuseScene : public Scene
{
public:
	DiffuseScene();
	void InitScene() override;
	void Update(float t);
	void Render() override;
	void Shutdown();
	void Resize(int x, int y);
private:
	ShaderProgram _shader;
	int _width, _height;
	Teapot* _torus;
	mat4 _model;
	mat4 _view;
	mat4 _projection;

	void SetMatrices();
	void CompileAndLinkShader();
};