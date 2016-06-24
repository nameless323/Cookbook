#pragma once

#include "../../Core/ShaderProgram.h"
#include "../Scene.h"
#include <glm.hpp>
#include "../../ingredients/bmpreader.h"
#include "../../Items/Mesh.h"

class ShadedWireframe : public Scene
{
public:
	ShadedWireframe();
	void ProcessInput(int key, int action) override;
	void InitScene() override;
	void Update(float t);
	void Render() override;
	void Shutdown();
	void Resize(int x, int y);
private:
	Mesh* _ogre;
	ShaderProgram _shader;
	glm::mat4 _model;
	glm::mat4 _view;
	glm::mat4 _viewport;
	glm::mat4 _projection;

	void SetMatrices();
	void CompileAndLinkShader();
};