#pragma once

#include "../../Core/ShaderProgram.h"
#include "../Scene.h"
#include <glm.hpp>
#include "../../ingredients/bmpreader.h"
#include "../../ingredients/vbomeshadj.h"
#include "../../Items/Mesh.h"
#include "../../Items/MeshAdj.h"

class Silhouette : public Scene
{
public:
	Silhouette();
	void ProcessInput(int key, int action) override;
	void InitScene() override;
	void Update(float t);
	void Render() override;
	void Shutdown();
	void Resize(int x, int y);
private:
	MeshAdj* _ogre;
	ShaderProgram _shader;
	glm::mat4 _model;
	glm::mat4 _view;
	glm::mat4 _viewport;
	glm::mat4 _projection;
	float _angle, _tPrev, _rotSpeed;

	void SetMatrices();
	void SetupFBO();
	void CompileAndLinkShader();
};