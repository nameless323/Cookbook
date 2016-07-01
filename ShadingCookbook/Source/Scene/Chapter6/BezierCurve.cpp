#include "BezierCurve.h"
#include <ostream>
#include <iostream>
#include <ctime>
#include "../../Items/MeshAdj.h"
#include "../../../ingredients/vbomeshadj.h"
using glm::vec3;

#include <gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <gtx/transform.hpp>

BezierCurve::BezierCurve() : _angle(0.0f), _tPrev(0.0f), _rotSpeed(glm::pi<float>() / 8.0f), _numSegments(50), _uniformDirty(false)
{
}

void BezierCurve::ProcessInput(int key, int action)
{
	if (key == GLFW_KEY_RIGHT && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		_numSegments += 5;
		_uniformDirty = true;
	}
	if (key == GLFW_KEY_LEFT && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		_numSegments -= 5;
		_numSegments = glm::max(0, _numSegments);
		_uniformDirty = true;
	}
}

void BezierCurve::InitScene()
{
	CompileAndLinkShader();

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	glEnable(GL_DEPTH_TEST);
	float c = 1.5f;
	_projection = glm::ortho(-0.4f * c, 0.4f * c, -0.3f *c, 0.3f*c, 0.1f, 100.0f);
	glPointSize(10.0f);
	float v[] = { -1.0f, -1.0f, -0.5f, 1.0f, 0.5f, -1.0f, 1.0f, 1.0f };
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

	_shader.Use();
	_shader.SetUniform("NumSegments", _numSegments);
	_shader.SetUniform("NumStrips", 1);
	_shader.SetUniform("LineColor", vec4(1.0f, 1.0f, 0.5f, 1.0f));

	_solidShader.Use();
	_solidShader.SetUniform("Color", vec4(0.5f, 1.0f, 1.0f, 1.0f));
}

void BezierCurve::Update(float t)
{
	float dt = t - _tPrev;
	if (_tPrev == 0.0f)
		dt = 0.0f;
	_tPrev = t;

	_angle += _rotSpeed * dt;
	if (_angle > glm::two_pi<float>()) _angle -= glm::two_pi<float>();

	if (_uniformDirty)
	{
		_uniformDirty = false;
		_shader.Use();
		_shader.SetUniform("NumSegments", _numSegments);
	}
}

void BezierCurve::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	vec3 cameraPos(0.0f, 0.0f, 1.5f);
	_view = glm::lookAt(cameraPos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	_model = glm::mat4(1.0f);
	glBindVertexArray(_vao);
	SetMatrices();

	_shader.Use();
	glDrawArrays(GL_PATCHES, 0, 4);

	_solidShader.Use();
	glDrawArrays(GL_POINTS, 0, 4);

	glFinish();
}

void BezierCurve::Shutdown()
{
}

void BezierCurve::Resize(int w, int h)
{
	glViewport(0, 0, w, h);
	float c = 4.5f;
	_projection = glm::ortho(-0.4f * c, 0.4f * c, -0.3f *c, 0.3f*c, 0.1f, 100.0f);
}

void BezierCurve::SetMatrices()
{
	glm::mat4 mv = _view * _model;
	_shader.Use();
	_shader.SetUniform("MVP", _projection*mv);
	_solidShader.Use();
	_solidShader.SetUniform("MVP", _projection*mv);
}

void BezierCurve::CompileAndLinkShader()
{
	try
	{
		_shader.CompileShader("Shaders/BezierCurve/BezierCurve.vert");
		_shader.CompileShader("Shaders/BezierCurve/BezierCurve.tesc");
		_shader.CompileShader("Shaders/BezierCurve/BezierCurve.tese");
		_shader.CompileShader("Shaders/BezierCurve/BezierCurve.frag");
		_shader.Link();
		_shader.Validate();
		_shader.Use();

		_solidShader.CompileShader("Shaders/BezierCurve/Solid.vert");
		_solidShader.CompileShader("Shaders/BezierCurve/Solid.frag");
		_solidShader.Link();
		_solidShader.Validate();
		_solidShader.Use();
	}
	catch (ShaderProgramException& e)
	{
		std::cerr << e.what() << std::endl;
		//exit(EXIT_FAILURE);
	}
}