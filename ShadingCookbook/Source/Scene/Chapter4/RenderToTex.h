#pragma once

#include "../../Core/ShaderProgram.h"
#include "../Scene.h"
#include "../../Items/Teapot.h"
#include "../../Items/Plane.h"
#include "../../Items/Torus.h"
#include "../../Items/Cube.h"
#include "../../Items/Skybox.h"

#include <glm.hpp>
using glm::mat4;

class RenderToTex : public Scene
{
public:
	RenderToTex();
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
	Plane* _plane;
	Skybox* _skybox;
	Cube* _cube;
	Torus* _torus;
	GLuint _fboHandle;
	mat4 _model;
	mat4 _view;
	mat4 _projection;
	float _angle;
	float _prevTime;
	float _rotationSpeed;
	bool _autorotate;
	bool _rotateLeft;
	bool _rotateRight;

	void SetMatrices();
	void CompileAndLinkShader();
	void SetupFBO();
	void RenderToTexture();
	void RenderScene();
};