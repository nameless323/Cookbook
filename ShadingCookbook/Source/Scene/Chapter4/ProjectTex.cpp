#include "ProjectTex.h"
#include <gtx/transform.hpp>
#include <iostream>
#include <GLFW/glfw3.h>
#include "../../Core/TGA.h"
using glm::vec3;

ProjectTex::ProjectTex() : _angle(0), _prevTime(0), _autorotate(1), _rotateLeft(0), _rotateRight(0)
{
}

void ProjectTex::ProcessInput(int key, int action)
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

void ProjectTex::InitScene()
{
	CompileAndLinkShader();

	glEnable(GL_DEPTH_TEST);

	_teapot = new Teapot(14, mat4(1.0f));
	_plane = new Plane(100.0f, 100.0f, 1, 1);

	_projection = mat4(1.0f);
	
	_angle = glm::radians(90.0f);

	vec3 projPos = vec3(2.0f, 5.0f, 5.0f);
	vec3 projAt = vec3(-2.0f, -4.0f, 0.0f);
	vec3 projUp = vec3(0.0f, 1.0f, 0.0f);
	mat4 projView = glm::lookAt(projPos, projAt, projUp);
	mat4 projProj = glm::perspective(glm::radians(30.0f), 1.0f, 0.2f, 1000.0f);
	mat4 projScaleTrans = glm::translate(vec3(0.5f)) * glm::scale(vec3(0.5f));
	_shader.SetUniform("ProjectorMatrix", projScaleTrans * projProj * projView);

	glActiveTexture(GL_TEXTURE0);
	TGA::LoadTex("./media/texture/flower.tga");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	_shader.SetUniform("Light.Position", vec4(0.0f, 0.0f, 0.0f, 1.0f));
	_shader.SetUniform("Light.Intensity", vec3(1.0f, 1.0f, 1.0f));
}

void ProjectTex::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	vec3 cameraPos = vec3(7.0f * cos(_angle), 2.0f, 7.0f * sin(_angle));
	_view = glm::lookAt(cameraPos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	
	_shader.SetUniform("Material.Kd", 0.5f, 0.2f, 0.1f);
	_shader.SetUniform("Material.Ks", 0.95f, 0.95f, 0.95f);
	_shader.SetUniform("Material.Ka", 0.1f, 0.1f, 0.1f);
	_shader.SetUniform("Material.Shininess", 100.0f);

	_model = mat4(1.0f);
	_model *= glm::translate(vec3(0.0f, -1.0f, 0.0f));
	_model *= glm::rotate(glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
	SetMatrices();
	_teapot->Render();

	_shader.SetUniform("Material.Kd", 0.4f, 0.4f, 0.4f);
	_shader.SetUniform("Material.Ks", 0.0f, 0.0f, 0.0f);
	_shader.SetUniform("Material.Ka", 0.1f, 0.1f, 0.1f);
	_shader.SetUniform("Material.Shininess", 1.0f);
	_model = mat4(1.0f);
	_model *= glm::translate(vec3(0.0f, -0.75f, 0.0f));
	SetMatrices();
	_plane->Render();
}

void ProjectTex::Update(float t)
{
	float dt = t - _prevTime;
	if (_prevTime == 0.0f) dt = 0.0f;
	_prevTime = t;

	_angle += 0.25f * dt;
	if (_angle > glm::two_pi<float>())
		_angle -= glm::two_pi<float>();
}


void ProjectTex::Shutdown()
{
	delete _plane;
	delete _teapot;
}

void ProjectTex::SetMatrices()
{
	mat4 mv = _view * _model;
	_shader.SetUniform("ModelMatrix", _model);
	_shader.SetUniform("ModelViewMatrix", mv);
	_shader.SetUniform("NormalMatrix",
		mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
	_shader.SetUniform("MVP", _projection * mv);
}

void ProjectTex::Resize(int w, int h)
{
	glViewport(0, 0, w, h);
	_width = w;
	_height = h;
	_projection = glm::perspective(glm::radians(50.0f), (float)w / h, 0.3f, 100.0f);
}

void ProjectTex::CompileAndLinkShader()
{
	try
	{
		_shader.CompileShader("Shaders/ProjectTex/ProjectTex.vert");
		_shader.CompileShader("Shaders/ProjectTex/ProjectTex.frag");
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
