#include "Flat.h"
#include <gtx/transform.hpp>
#include <iostream>
using glm::vec3;

Flat::Flat() : _angle(0)
{}

void Flat::ProcessInput(int key, int action)
{}

void Flat::InitScene()
{
	CompileAndLinkShader();
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	_ogre = new Mesh("./media/bs_ears.obj");

	_view = glm::lookAt(vec3(0.0f, 0.35f, 0.85f), vec3(0.0f, -0.25f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	_projection = mat4(1.0f);

	_shader.SetUniform("Material.Ka", 0.9f, 0.5f, 0.3f);
	_shader.SetUniform("Material.Kd", 0.9f, 0.5f, 0.3f);
	_shader.SetUniform("Light.Ld", 1.0f, 1.0f, 1.0f);
	_shader.SetUniform("Light.La", 0.4f, 0.4f, 0.4f);
	_shader.SetUniform("Material.Ks", 0.8f, 0.8f, 0.8f);
	_shader.SetUniform("Light.Ls", 1.0f, 1.0f, 1.0f);
	_shader.SetUniform("Material.Shininess", 100.0f);
}

void Flat::Update(float t)
{	
}

void Flat::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	vec4 worldLight = vec4(2.0f, 4.0f, 1.0f, 1.0f);
	_model = glm::rotate(glm::radians(_angle), vec3(0.0f, 1.0f, 0.0f));
	_shader.SetUniform("Light.Position", _view * _model * worldLight);

	_model = mat4(1.0f);
	_model *= glm::rotate(glm::radians(25.0f), vec3(0.0f, 1.0f, 0.0f));

	SetMatrices();
	_ogre->Render();
}

void Flat::Shutdown()
{
	delete _ogre;
}

void Flat::SetMatrices()
{
	mat4 mv = _view * _model;
	_shader.SetUniform("ModelViewMatrix", mv);
	_shader.SetUniform("NormalMatrix", mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
	_shader.SetUniform("MVP", _projection*mv);
}

void Flat::Resize(int w, int h)
{
	glViewport(0, 0, w, h);
	_width = w;
	_height = h;
	_projection = glm::perspective(glm::radians(70.0f), (float)w / (float)h, 0.3f, 100.0f);
}

void Flat::CompileAndLinkShader()
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
