#pragma once

#include "../../Core/ShaderProgram.h"
#include "../Scene.h"
#include <glm.hpp>
#include "../../ingredients/bmpreader.h"

class PointSprite : public Scene
{
public:
	PointSprite();
	void ProcessInput(int key, int action) override;
	void InitScene() override;
	void Update(float t);
	void Render() override;
	void Shutdown();
	void Resize(int x, int y);
private:
	ShaderProgram _shader;
	GLuint _sprites;
	glm::mat4 _model;
	glm::mat4 _view;
	glm::mat4 _projection;
	int _numSprites;
	float* _locations;

	void SetMatrices();
	void CompileAndLinkShader();
};