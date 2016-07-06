#include "TessTeapot.h"
#include <ostream>
#include <iostream>
#include <ctime>
#include "../../Items/MeshAdj.h"
#include "../../../ingredients/vbomeshadj.h"
using glm::vec3;

#include <gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <gtx/transform.hpp>

TessTeapot::TessTeapot() : _inner(4), _outer(4), _angle(0.0), _tPrev(0.0f), _rotSpeed(glm::pi<float>() / 8.0f)
{
}

void TessTeapot::ProcessInput(int key, int action)
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

void TessTeapot::InitScene()
{
	CompileAndLinkShader();
	_projection = glm::perspective(glm::radians(60.0f), (float)1024 / (float)768, 0.3f, 100.0f);

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	glEnable(GL_DEPTH_TEST);
	_angle = glm::pi<float>() / 3.0f;
	_teapot = new VBOTeapotPatch();

	_shader.Use();
	_shader.SetUniform("TessLevel", 4);
	_shader.SetUniform("LineWidth", 1.5f);
	_shader.SetUniform("LineColor", vec4(0.05f, 0.0f, 0.05f, 1.0f));
	_shader.SetUniform("LightPosition", vec4(0.0f, 0.0f, 0.0f, 1.0f));
	_shader.SetUniform("LightIntensity", vec3(1.0f, 1.0f, 1.0f));
	_shader.SetUniform("Kd", vec3(0.9f, 0.9f, 1.0f));


	glPatchParameteri(GL_PATCH_VERTICES, 16);

}

void TessTeapot::Update(float t)
{
	float deltaT = t - _tPrev;
	if (_tPrev == 0.0f) deltaT = 0.0f;
	_tPrev = t;

	_angle += _rotSpeed * deltaT;
	if (_angle > glm::two_pi<float>()) _angle -= glm::two_pi<float>();
}

void TessTeapot::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	vec3 cameraPos(4.25f * cos(_angle), 3.0f, 4.25f * sin(_angle));
	_view = glm::lookAt(cameraPos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	_model = glm::mat4(1.0f);
	_model = glm::translate(_model, vec3(0.0f, -1.5f, 0.0f));
	_model = glm::rotate(_model, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
	_shader.SetUniform("TessLevel", _inner);
	SetMatrices();

	_shader.Use();
	_teapot->render();

	glFinish();
}

void TessTeapot::Shutdown()
{
	delete _teapot;
}

void TessTeapot::Resize(int w, int h)
{
	glViewport(0, 0, w, h);
	_projection = glm::perspective(glm::radians(60.0f), (float)w / h, 0.3f, 100.0f);

	float w2 = w / 2.0f;
	float h2 = h / 2.0f;
	_viewport = glm::mat4(vec4(w2, 0.0f, 0.0f, 0.0f),
		vec4(0.0f, h2, 0.0f, 0.0f),
		vec4(0.0f, 0.0f, 1.0f, 0.0f),
		vec4(w2 + 0, h2 + 0, 0.0f, 1.0f));
}

void TessTeapot::SetMatrices()
{
	glm::mat4 mv = _view * _model;
	_shader.Use();
	_shader.SetUniform("MVP", _projection*mv);
	_shader.SetUniform("NormalMatrix",
		mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
	_shader.SetUniform("MVP", _projection * mv);
	_shader.SetUniform("ViewportMatrix", _viewport);
}

void TessTeapot::CompileAndLinkShader()
{
	try
	{
		_shader.CompileShader("Shaders/TessTeapot/TessTeapot.vert");
		_shader.CompileShader("Shaders/TessTeapot/TessTeapot.tesc");
		_shader.CompileShader("Shaders/TessTeapot/TessTeapot.tese");
		_shader.CompileShader("Shaders/TessTeapot/TessTeapot.geom");
		_shader.CompileShader("Shaders/TessTeapot/TessTeapot.frag");
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