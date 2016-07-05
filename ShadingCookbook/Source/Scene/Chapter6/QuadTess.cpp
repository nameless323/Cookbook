#include "QuadTess.h"
#include <ostream>
#include <iostream>
#include <ctime>
#include "../../Items/MeshAdj.h"
#include "../../../ingredients/vbomeshadj.h"
using glm::vec3;

#include <gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <gtx/transform.hpp>

QuadTess::QuadTess() : _inner(4), _outer(4)
{
}

void QuadTess::ProcessInput(int key, int action)
{
	if (key == GLFW_KEY_RIGHT && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		_inner += 1;
		_inner = glm::min<int>(_inner, 32);
	}
	if (key == GLFW_KEY_LEFT && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		_inner -= 1;
		_inner = glm::max<int>(_inner, 1);
	}
	if (key == GLFW_KEY_UP && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		_outer += 1;
		_outer = glm::min<int>(_outer, 32);
	}
	if (key == GLFW_KEY_DOWN && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		_outer -= 1;
		_outer = glm::max<int>(_outer, 1);
	}
}

void QuadTess::InitScene()
{
	CompileAndLinkShader();

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	glEnable(GL_DEPTH_TEST);
	float c = 3.5f;
	_projection = glm::ortho(-0.4f * c, 0.4f * c, -0.3f *c, 0.3f*c, 0.1f, 100.0f);

	_shader.Use();
	_shader.SetUniform("Inner", 4);
	_shader.SetUniform("Outer", 4);
	_shader.SetUniform("LineWidth", 1.5f);
	_shader.SetUniform("LineColor", vec4(0.05f, 0.0f, 0.05f, 1.0f));
	_shader.SetUniform("QuadColor", vec4(1.0f, 1.0f, 1.0f, 1.0f));

	float v[] = { -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f };
	GLuint vboHandle;
	glGenBuffers(1, &vboHandle);

	glBindBuffer(GL_ARRAY_BUFFER, vboHandle);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), v, GL_STATIC_DRAW);

	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	glBindBuffer(GL_ARRAY_BUFFER, vboHandle);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	glPatchParameteri(GL_PATCH_VERTICES, 4);

	GLint maxVerts;
	glGetIntegerv(GL_MAX_PATCH_VERTICES, &maxVerts);
	printf("Max patch vertices: %d\n", maxVerts);
}

void QuadTess::Update(float t)
{
}

void QuadTess::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	vec3 cameraPos(0.0f, 0.0f, 1.5f);
	_view = glm::lookAt(cameraPos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	_model = glm::mat4(1.0f);
	glBindVertexArray(_vao);
	SetMatrices();

	_shader.Use();
	glPatchParameteri(GL_PATCH_DEFAULT_INNER_LEVEL, _inner);
	glPatchParameteri(GL_PATCH_DEFAULT_OUTER_LEVEL, _outer);
	_shader.SetUniform("Inner", _inner);
	_shader.SetUniform("Outer", _outer);
	glBindVertexArray(_vao);
	glDrawArrays(GL_PATCHES, 0, 4);

	glFinish();
}

void QuadTess::Shutdown()
{
}

void QuadTess::Resize(int w, int h)
{
	glViewport(0, 0, w, h);
	float c = 4.5f;
	_projection = glm::ortho(-0.4f * c, 0.4f * c, -0.3f *c, 0.3f*c, 0.1f, 100.0f);

	float w2 = w / 2.0f;
	float h2 = h / 2.0f;
	_viewport = glm::mat4(vec4(w2, 0.0f, 0.0f, 0.0f),
		vec4(0.0f, h2, 0.0f, 0.0f),
		vec4(0.0f, 0.0f, 1.0f, 0.0f),
		vec4(w2 + 0, h2 + 0, 0.0f, 1.0f));
}

void QuadTess::SetMatrices()
{
	glm::mat4 mv = _view * _model;
	_shader.Use();
	_shader.SetUniform("MVP", _projection*mv);
	_shader.SetUniform("ViewportMatrix", _viewport);
}

void QuadTess::CompileAndLinkShader()
{
	try
	{
		_shader.CompileShader("Shaders/QuadTess/QuadTess.vert");
		_shader.CompileShader("Shaders/QuadTess/QuadTess.tesc");
		_shader.CompileShader("Shaders/QuadTess/QuadTess.tese");
		_shader.CompileShader("Shaders/QuadTess/QuadTess.geom");
		_shader.CompileShader("Shaders/QuadTess/QuadTess.frag");
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