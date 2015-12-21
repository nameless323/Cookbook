#pragma once

#include "../../Core/ShaderProgram.h"
#include "../Scene.h"
#include "../../Items/Teapot.h"

#include <glm.hpp>
using glm::mat4;

class Discarding : public Scene
{
public:
	Discarding();
	void ProcessInput(int key, int action) override;
	void InitScene() override;
	void Update(float t);
	void Render() override;
	void Shutdown();
	void Resize(int x, int y);
private:
	ShaderProgram _shader;
	int _width, _height;
	Teapot* _teapot;
	mat4 _model;
	mat4 _view;
	mat4 _projection;
	float _angle;
	float _prevTime;
	bool _autorotate;
	bool _rotateLeft;
	bool _rotateRight;

	void SetMatrices();
	void CompileAndLinkShader();
};