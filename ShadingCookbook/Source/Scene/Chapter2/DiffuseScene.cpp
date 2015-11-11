#include "DiffuseScene.h"
#include <gtx/transform.hpp>
using glm::vec3;

DiffuseScene::DiffuseScene()
{}

void DiffuseScene::InitScene()
{
	CompileAndLinkShader();
	glEnable(GL_DEPTH);
	glDepthFunc(GL_LEQUAL);

	_torus = new Torus(0.7f, 0.3f, 30, 30);

	_model = mat4(1.0f);
	_model *= glm::rotate(glm::radians(-35.0f), vec3(1.0f, 0.0f, 0.0f));
	_model *= glm::rotate(glm::radians(35.0f), vec3(0.0f, 1.0f, 0.0f));
	_view = glm::lookAt(vec3(0.0f, 0.0f, 2.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

	_projection = mat4(1.0f);

	_shader.SetUniform("Kd", 0.9f, 0.5f, 0.3f);
	_shader.SetUniform("Ld", 1.0f, 1.0f, 1.0f);
	_shader.SetUniform("LightPosition", _view*vec4(5.0f, 5.0f, 2.0f, 1.0f));
}

void DiffuseScene::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	SetMatrices();
	_torus->Render();
}

void DiffuseScene::Update(float t)
{}


void DiffuseScene::Shutdown()
{
	delete _torus;
}