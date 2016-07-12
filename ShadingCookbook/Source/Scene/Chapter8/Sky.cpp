#include "Sky.h"
#include <ostream>
#include <iostream>
#include <ctime>
#include "../../Items/NoiseTex.h"
using glm::vec3;

#include <gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <gtx/transform.hpp>

Sky::Sky() : _width(1024), _height(768)
{
}

void Sky::ProcessInput(int key, int action)
{
}

void Sky::InitScene()
{
	CompileAndLinkShader();

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	glEnable(GL_DEPTH_TEST);
	_projection = mat4(1.0);

	GLfloat verts[] = 
	{
		-1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f
	};
	GLfloat tc[] = 
	{
		0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
	};

	unsigned int handle[2];
	glGenBuffers(2, handle);

	glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
	glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(float), verts, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
	glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), tc, GL_STATIC_DRAW);

	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	_shader.SetUniform("NoiseTex", 0);

	GLuint noiseTex = NoiseTex::Generate2DTex(6.0f);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, noiseTex);
}

void Sky::Update(float t)
{
}

void Sky::Render()
{
	_view = mat4(1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	DrawScene();
	glFinish();
}

void Sky::Shutdown()
{
}

void Sky::Resize(int w, int h)
{
	glViewport(0, 0, w, h);
	_width = w;
	_height = h;
}

void Sky::DrawScene()
{
	_model = mat4(1.0f);
	SetMatrices();

	glBindVertexArray(_vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Sky::SetMatrices()
{
	glm::mat4 mv = _view * _model;
	_shader.Use();
	_shader.SetUniform("MVP", _projection*mv);
}

void Sky::CompileAndLinkShader()
{
	try
	{
		_shader.CompileShader("Shaders/Sky/Sky.vert");
		_shader.CompileShader("Shaders/Sky/Sky.frag");
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