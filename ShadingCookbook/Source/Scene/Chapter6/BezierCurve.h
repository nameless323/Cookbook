#pragma once

#include "../../Core/ShaderProgram.h"
#include "../Scene.h"
#include <glm.hpp>
#include "../../ingredients/bmpreader.h"
#include "../../ingredients/vbomeshadj.h"
#include "../../Items/Mesh.h"
#include "../../Items/MeshAdj.h"

class BezierCurve : public Scene
{
public:
	BezierCurve();
	void ProcessInput(int key, int action) override;
	void InitScene() override;
	void Update(float t);
	void Render() override;
	void Shutdown();
	void Resize(int x, int y);
private:
	ShaderProgram _shader;
	ShaderProgram _solidShader;
	glm::mat4 _model;
	glm::mat4 _view;
	glm::mat4 _viewport;
	glm::mat4 _projection;
	GLuint _vao;
	float _angle, _tPrev, _rotSpeed;

	void SetMatrices();
	void CompileAndLinkShader();
};