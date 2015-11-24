#include "DiffuseScene.h"
#include <gtx/transform.hpp>
#include <iostream>
using glm::vec3;

DiffuseScene::DiffuseScene()
{}

void DiffuseScene::InitScene()
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	CompileAndLinkShader();

	mat4 transform = glm::translate(mat4(1.0f), vec3(0.0f, 1.5f, 0.25f));
	_torus = new Torus(0.7f, 0.3f, 30, 30);

	_model = mat4(1.0f);
	_model *= rotate(glm::radians(-30.0f), vec3(1.0f, 0.0f, 0.0f));
	_model *= rotate(glm::radians(35.0f), vec3(0.0f, 1.0f, 0.0f));
	_view = lookAt(vec3(0.0f, 0.0f, 2.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

	_projection = mat4(1.0f);

	_shader.SetUniform("Kd", 0.9f, 0.5f, 0.3f);
	_shader.SetUniform("Ld", 1.0f, 1.0f, 1.0f);
	_shader.SetUniform("LightPosition", _view*vec4(5.0f, 5.0f, 2.0f, 1.0f));
}

void DiffuseScene::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	SetMatrices();
	_shader.Use();
	_torus->Render();
}

void DiffuseScene::Update(float t)
{}


void DiffuseScene::Shutdown()
{
	delete _torus;
}

void DiffuseScene::SetMatrices()
{
	mat4 mv = _view*_model;
	_shader.SetUniform("ModelViewMatrix", mv);
	_shader.SetUniform("NormalMatrix", mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
	_shader.SetUniform("MVP", _projection*mv);
}

void DiffuseScene::Resize(int w, int h)
{
	glViewport(0, 0, w, h);
	_width = w;
	_height = h;
	_projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);
}

void DiffuseScene::CompileAndLinkShader()
{
	try
	{
		_shader.CompileShader("Shaders/Diffuse/Diffuse.vert");
		_shader.CompileShader("Shaders/Diffuse/Diffuse.frag");
		_shader.Link();
		_shader.Validate();
		_shader.Use();
	} catch(ShaderProgramException& e)
	{
		std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
}


