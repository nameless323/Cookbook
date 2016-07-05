#pragma once

#include "../../Core/ShaderProgram.h"
#include "../Scene.h"
#include <glm.hpp>
#include <GL/glew.h>

class QuadTess : public Scene
{
public:
	QuadTess();
	void ProcessInput(int key, int action) override;
	void InitScene() override;
	void Update(float t);
	void Render() override;
	void Shutdown();
	void Resize(int x, int y);
private:
	ShaderProgram _shader;
	glm::mat4 _model;
	glm::mat4 _view;
	glm::mat4 _viewport;
	glm::mat4 _projection;
	GLuint _vao;
	int _inner;
	int _outer;

	void SetMatrices();
	void CompileAndLinkShader();
};