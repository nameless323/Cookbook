#pragma once

#include "../../Core/ShaderProgram.h"
#include "../../Items/Mesh.h"
#include "../Scene.h"
#include <glm.hpp>

class Flat : public Scene
{
public:
	Flat();
	void ProcessInput(int key, int action) override;
	void InitScene() override;
	void Update(float t) override;
	void Render() override;
	void Shutdown() override;
	void Resize(int w, int h) override;
private:
	ShaderProgram _shader;
	float _angle;
	Mesh* _ogre;
	int _width;
	int _height;
	mat4 _model;
	mat4 _view;
	mat4 _projection;
	
	void CompileAndLinkShader();
	void SetMatrices();
};