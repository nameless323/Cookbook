#include "Subroutine.h"
#include <gtx/transform.hpp>
#include <iostream>
#include <GLFW/glfw3.h>
using glm::vec3;

Subroutine::Subroutine() : _angle(0), _prevTime(0), _autorotate(1), _rotateLeft(0), _rotateRight(0)
{
}

void Subroutine::ProcessInput(int key, int action)
{
	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
		_autorotate = !_autorotate;

	if (key == GLFW_KEY_LEFT && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		_autorotate = false;
		_rotateLeft = true;
	}
	if (key == GLFW_KEY_RIGHT && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		_autorotate = false;
		_rotateRight = true;
	}
}

void Subroutine::InitScene()
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	CompileAndLinkShader();

	mat4 transform = glm::translate(mat4(1.0f), vec3(0.0f, 1.5f, 0.25f));
	_teapot = new Teapot(13, transform);

	_model = mat4(1.0f);
	_model *= rotate(glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
	//_model *= rotate(glm::radians(35.0f), vec3(0.0f, 1.0f, 0.0f));
	_view = lookAt(vec3(2.0f, 5.0f, 3.5f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

	_projection = mat4(1.0f);
	glm::vec4 worldLight = vec4(2.0f, 4.0f, 2.0f, 1.0f);


	_shader.SetUniform("Material.Kd", 5.0f, 0.9f, 0.3f);
	_shader.SetUniform("Light.Ld", 1.0f, 1.0f, 1.0f);
	_shader.SetUniform("Light.Position", _view * worldLight);
	_shader.SetUniform("Material.Ka", 0.9f, 0.5f, 0.3f);
	_shader.SetUniform("Light.La", 0.4f, 0.4f, 0.4f);
	_shader.SetUniform("Material.Ks", 1.0f, 1.0f, 1.0f);
	_shader.SetUniform("Light.Ls", 1.0f, 1.0f, 1.0f);
	_shader.SetUniform("Material.Shininess", 100.0f);

	_phongSubroutineInd = glGetSubroutineIndex(_shader.GetHandle(), GL_VERTEX_SHADER, "PhongModel");
	_diffuseSubroutineInd = glGetSubroutineIndex(_shader.GetHandle(), GL_VERTEX_SHADER, "DiffuseModel");
	glGetSubroutineUniformLocation(_shader.GetHandle(), GL_VERTEX_SHADER, "LightModel");
}

void Subroutine::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glm::vec4 worldLight = glm::rotate(_angle / 30.0f, vec3(1.0f, 0.0f, 0.0f)) * vec4(2.0f, 4.0f, 2.0f, 1.0f);
	_shader.SetUniform("Light.Position", _view*_model*worldLight);
	
	//	_model = mat4(1.0f);
	//	_model *= glm::rotate(glm::radians(_angle), vec3(0.0f, 1.0f, 0.0f));
	//	_model *= rotate(glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
	//	_model *= rotate(glm::radians(35.0f), vec3(0.0f, 1.0f, 0.0f));


	SetMatrices();
	_shader.Use();

	glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &_phongSubroutineInd);
	_teapot->Render();
}

void Subroutine::Update(float t)
{
	float dt = t - _prevTime;
	if (_autorotate)
		_angle += 30 * dt;
	if (_rotateRight)
		_angle += 30 * dt;
	if (_rotateLeft)
		_angle -= 30 * dt;

	_prevTime = t;


	_rotateLeft = false;
	_rotateRight = false;
}


void Subroutine::Shutdown()
{
	delete _teapot;
}

void Subroutine::SetMatrices()
{
	mat4 mv = _view * _model;
	_shader.SetUniform("ModelViewMatrix", mv);
	_shader.SetUniform("NormalMatrix", mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
	_shader.SetUniform("MVP", _projection * mv);
}

void Subroutine::Resize(int w, int h)
{
	glViewport(0, 0, w, h);
	_width = w;
	_height = h;
	_projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);
}

void Subroutine::CompileAndLinkShader()
{
	try
	{
		_shader.CompileShader("Shaders/Subroutine/Subroutine.vert");
		_shader.CompileShader("Shaders/Subroutine/Subroutine.frag");
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
