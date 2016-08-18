#include "ParticlesFeedback.h"

#include <ostream>
#include <iostream>
#include <ctime>
#include "../../Items/NoiseTex.h"
#include "../../../ingredients/bmpreader.h"
using glm::vec3;
using glm::mat4;

#include <gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <gtx/transform.hpp>

ParticlesFeedback::ParticlesFeedback() : _width(1024), _height(768), _drawBuf(0), _time(0), _dt(0)
{
}

void ParticlesFeedback::ProcessInput(int key, int action)
{
}

void ParticlesFeedback::InitScene()
{
	CompileAndLinkShaders();

	GLuint shaderHandle = _shader.GetHandle();
	_renderSub = glGetSubroutineIndex(shaderHandle, GL_VERTEX_SHADER, "render");
	_updateSub = glGetSubroutineIndex(shaderHandle, GL_VERTEX_SHADER, "update");

	glClearColor(0.1f, 0.1f, 0.1f, 0.0f);

	glPointSize(10.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	_projection = mat4(1.0);
	_model = mat4(1.0f);

	InitBuffers();

	const char* texName = "./media/texture/bluewater.bmp";
	glActiveTexture(GL_TEXTURE0);
	BMPReader::loadTex(texName);

	_shader.SetUniform("ParticleTex", 0);
	_shader.SetUniform("ParticleLifetime", 3.5f);
	_shader.SetUniform("Accel", vec3(0.0f, -0.4f, 0.0f));
}

void ParticlesFeedback::Update(float t)
{
	_dt = t - _time;
	_time = t;
}

void ParticlesFeedback::Render()
{
	glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &_updateSub);
	_shader.SetUniform("Time", _time);
	_shader.SetUniform("H", _dt);

	glEnable(GL_RASTERIZER_DISCARD);

	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, _feedback[_drawBuf]);

	glBeginTransformFeedback(GL_POINTS);
	glBindVertexArray(_particleArray[1 - _drawBuf]);
	glDrawArrays(GL_POINTS, 0, _nParticles);
	glEndTransformFeedback();

	glDisable(GL_RASTERIZER_DISCARD);

	glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &_renderSub);
	glClear(GL_COLOR_BUFFER_BIT);
	_view = glm::lookAt(vec3(3.0f * cos(_angle), 1.5f, 3.0f * sin(_angle)), vec3(0.0f, 1.5f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	SetMatrices();

	glBindVertexArray(_particleArray[_drawBuf]);
	glDrawTransformFeedback(GL_POINTS, _feedback[_drawBuf]);

	_drawBuf = 1 - _drawBuf;
}

void ParticlesFeedback::Shutdown()
{
}

void ParticlesFeedback::Resize(int x, int y)
{
	glViewport(0, 0, x, y);
	_width = x;
	_height = y;
	_projection = glm::perspective(glm::radians(60.0f), (float)x / y, 0.3f, 100.0f);
}

void ParticlesFeedback::InitBuffers()
{


}

float ParticlesFeedback::RandFloat()
{
	return (float)rand() / RAND_MAX;
}

void ParticlesFeedback::CompileAndLinkShaders()
{
	try
	{
		_shader.CompileShader("Shaders/ParticlesFeedback/ParticlesFeedback.vert");
		_shader.CompileShader("Shaders/ParticlesFeedback/ParticlesFeedback.frag");

		GLuint shaderHandle = _shader.GetHandle();
		const char* outputNames[] = { "Position", "Velocity", "StartTime" };
		glTransformFeedbackVaryings(shaderHandle, 3, outputNames, GL_SEPARATE_ATTRIBS);

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

void ParticlesFeedback::SetMatrices()
{
	mat4 mv = _view * _model;
	_shader.SetUniform("MVP", _projection*mv);
}