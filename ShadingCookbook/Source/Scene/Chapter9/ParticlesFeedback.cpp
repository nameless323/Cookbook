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

	const char* texName = "./media/texture/brick1.bmp";
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
	_nParticles = 4000;

	glGenBuffers(2, _posBuf);
	glGenBuffers(2, _velBuf);
	glGenBuffers(2, _startTime);
	glGenBuffers(1, &_initVel);

	int size = _nParticles * 3 * sizeof(GLfloat);
	glBindBuffer(GL_ARRAY_BUFFER, _posBuf[0]);
	glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, _posBuf[1]);
	glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, _velBuf[0]);
	glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, _velBuf[1]);
	glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, _initVel);
	glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, _startTime[0]);
	glBufferData(GL_ARRAY_BUFFER, _nParticles * sizeof(float), nullptr, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, _startTime[1]);
	glBufferData(GL_ARRAY_BUFFER, _nParticles * sizeof(float), nullptr, GL_DYNAMIC_COPY);

	GLfloat* data = new GLfloat[_nParticles * 3];
	for (int i = 0; i < _nParticles * 3; i++)
		data[i] = 0.0f;
	glBindBuffer(GL_ARRAY_BUFFER, _posBuf[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);

	vec3 v(0.0f);
	float velocity, theta, phi;
	for(int i = 0; i < _nParticles; i++)
	{
		theta = glm::mix(0.0f, glm::pi<float>() / 6.0f, RandFloat());
		phi = glm::mix(0.0f, glm::two_pi<float>(), RandFloat());

		v.x = sinf(theta) * cosf(phi);
		v.y = cosf(theta);
		v.z = sinf(theta) * sinf(phi);

		velocity = glm::mix(1.25f, 1.5f, RandFloat());
		v = glm::normalize(v) * velocity;

		data[3 * i] = v.x;
		data[3 * i + 1] = v.y;
		data[3 * i + 2] = v.z;
	}
	glBindBuffer(GL_ARRAY_BUFFER, _velBuf[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	glBindBuffer(GL_ARRAY_BUFFER, _initVel);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);

	delete[] data;
	data = new GLfloat[_nParticles];
	float time = 0.0f;
	float rate = 0.001f;
	for (int i = 0; i < _nParticles; i++)
	{
		data[i] = time;
		time += rate;
	}
	glBindBuffer(GL_ARRAY_BUFFER, _startTime[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, _nParticles * sizeof(float), data);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	delete[] data;

	glGenVertexArrays(2, _particleArray);

	glBindVertexArray(_particleArray[0]);
	glBindBuffer(GL_ARRAY_BUFFER, _posBuf[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, _velBuf[0]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, _startTime[0]);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, _initVel);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(3);

	glBindVertexArray(_particleArray[1]);
	glBindBuffer(GL_ARRAY_BUFFER, _posBuf[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, _velBuf[1]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, _startTime[1]);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, _initVel);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(3);

	glBindVertexArray(0);

	glGenTransformFeedbacks(2, _feedback);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, _feedback[0]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, _posBuf[0]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, _velBuf[0]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, _startTime[0]);

	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, _feedback[1]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, _posBuf[1]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, _velBuf[1]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, _startTime[1]);

	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

	GLint value;
	glGetIntegerv(GL_MAX_TRANSFORM_FEEDBACK_BUFFERS, &value);
	printf("MAX_TRANSFORM_FEEDBACK = %d\n", value);
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