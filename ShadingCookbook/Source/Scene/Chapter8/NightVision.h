#pragma once

#include "../../Core/ShaderProgram.h"
#include "../Scene.h"
#include "../../Items/Teapot.h"
#include "../../Items/Plane.h"
#include "../../Items/Torus.h"
#include "../../Items/Cube.h"
#include "../../Items/Skybox.h"

#include <glm.hpp>
#include <GL/glew.h>
using glm::mat4;

class NightVision : public Scene
{
public:
	NightVision();
	void ProcessInput(int key, int action) override;
	void InitScene() override;
	void Update(float t) override;
	void Render() override;
	void Shutdown() override;
	void Resize(int x, int y) override;
private:
	ShaderProgram _shader;
	int _width, _height;
	GLuint _renderFBO;
	GLuint _fsQuad;
	GLuint _renderTex;
	GLuint _noiseTex;
	Teapot* _teapot;
	Plane* _plane;
	Cube* _cube;
	Torus* _torus;
	mat4 _model;
	mat4 _view;
	mat4 _projection;
	float _angle;
	float _prevTime;
	float _rotationSpeed;
	bool _autorotate;
	bool _rotateLeft;
	bool _rotateRight;
	GLuint _pass1Index;
	GLuint _pass2Index;
	void SetMatrices();
	void CompileAndLinkShader();
	void SetupFBO();
	void Pass1();
	void Pass2();
};