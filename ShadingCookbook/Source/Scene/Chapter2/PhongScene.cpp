#include "PhongScene.h"
#include <gtx/transform.hpp>
#include <iostream>
using glm::vec3;

PhongScene::PhongScene() : _angle(0), _prevTime(0)
{
}

void PhongScene::InitScene()
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	CompileAndLinkShader();

	_torus = new Torus(0.7f, 0.3f, 50, 50);

	_model = mat4(1.0f);
	_model *= rotate(glm::radians(-35.0f), vec3(1.0f, 0.0f, 0.0f));
	_model *= rotate(glm::radians(35.0f), vec3(0.0f, 1.0f, 0.0f));
	_view = lookAt(vec3(0.0f, 0.0f, -2.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

	_projection = mat4(1.0f);
	glm::vec4 worldLight = vec4(-5.0f, 5.0f, -2.0f, 1.0f);


	_shader.SetUniform("Material.Kd", 0.9f, 0.5f, 0.3f);
	_shader.SetUniform("Light.Ld", 1.0f, 1.0f, 1.0f);
	_shader.SetUniform("Light.Position", _view * worldLight);
	_shader.SetUniform("Material.Ka", 0.9f, 0.5f, 0.3f);
	_shader.SetUniform("Light.La", 0.4f, 0.4f, 0.4f);
	_shader.SetUniform("Material.Ks", 0.8f, 0.8f, 0.8f);
	_shader.SetUniform("Light.Ls", 1.0f, 1.0f, 1.0f);
	_shader.SetUniform("Material.Shininess", 100.0f);
}

void PhongScene::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	_model = mat4(1.0f);
	_model *= glm::rotate(glm::radians(_angle), vec3(0.0f, 1.0f, 0.0f));
	_model *= rotate(glm::radians(-35.0f), vec3(1.0f, 0.0f, 0.0f));
	_model *= rotate(glm::radians(35.0f), vec3(0.0f, 1.0f, 0.0f));

	SetMatrices();
	_shader.Use();
	_torus->Render();
}

void PhongScene::Update(float t)
{
	float dt = t - _prevTime;
	_angle += 30*dt;
	_prevTime = t;
}


void PhongScene::Shutdown()
{
	delete _torus;
}

void PhongScene::SetMatrices()
{
	mat4 mv = _view * _model;
	_shader.SetUniform("ModelViewMatrix", mv);
	_shader.SetUniform("NormalMatrix", mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
	_shader.SetUniform("MVP", _projection * mv);
}

void PhongScene::Resize(int w, int h)
{
	glViewport(0, 0, w, h);
	_width = w;
	_height = h;
	_projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);
}

void PhongScene::CompileAndLinkShader()
{
	try
	{
		_shader.CompileShader("Shaders/Phong/Phong.vert");
		_shader.CompileShader("Shaders/Phong/Phong.frag");
		_shader.Link();
		_shader.Validate();
		_shader.Use();
	}
	catch (ShaderProgramException& e)
	{
		std::cerr << e.what() << std::endl;
		//exit(EXIT_FAILURE);
	}
}
