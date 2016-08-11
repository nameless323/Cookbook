#include "Waves.h"

#include <ostream>
#include <iostream>
#include <ctime>
using glm::vec3;
using glm::mat4;

#include <gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <gtx/transform.hpp>

Waves::Waves() : _width(1024), _height(768), _time(0)
{	
}

void Waves::ProcessInput(int key, int action)
{
}

void Waves::InitScene()
{
	CompileAndLinkShader();
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	_plane = new Plane(13.0f, 10.0f, 200, 2);
	_projection = mat4(1.0f);
	_shader.SetUniform("Light.Intensity", vec3(1.0f, 1.0f, 1.0f));
	_angle = glm::half_pi<float>();
}

void Waves::Update(float t)
{
	_time = t;
}

void Waves::Render()
{
	_shader.SetUniform("Time", _time);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	_view = glm::lookAt(vec3(10.0f * cos(_angle), 4.0f, 10.0f * sin(_angle)), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	_projection = glm::perspective(glm::radians(60.0f), (float)_width / _height, 0.3f, 100.0f);

	_shader.SetUniform("Material.Kd", 0.9f, 0.5f, 0.3f);
	_shader.SetUniform("Material.Ks", 0.8f, 0.8f, 0.8f);
	_shader.SetUniform("Material.Ka", 0.2f, 0.2f, 0.2f);
	_shader.SetUniform("Material.Shininess", 100.0f);
	_model = mat4(1.0f);
	_model *= glm::rotate(glm::radians(-10.0f), vec3(0.0f, 0.0f, 1.0f));
	_model *= glm::rotate(glm::radians(50.0f), vec3(1.0f, 0.0f, 0.0f));
	SetMatrices();
	_plane->Render();
}

void Waves::Shutdown()
{
	delete _plane;
}

void Waves::Resize(int x, int y)
{
	glViewport(0, 0, x, y);
	_width = x;
	_height = y;
	_projection = glm::perspective(glm::radians(60.0f), (float)x / y, 0.3f, 100.0f);
}

void Waves::SetMatrices()
{
	mat4 mv = _view * _model;
	_shader.SetUniform("ModelViewMatrix", mv);
	_shader.SetUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
	_shader.SetUniform("MVP", _projection * mv);
}

void Waves::CompileAndLinkShader()
{
	try
	{
		_shader.CompileShader("Shaders/Waves/Waves.vert");
		_shader.CompileShader("Shaders/Waves/Waves.frag");
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