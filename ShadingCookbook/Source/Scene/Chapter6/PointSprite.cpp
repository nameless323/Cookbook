#include "PointSprite.h"
#include <ostream>
#include <iostream>
#include <ctime>
using glm::vec3;

#include <gtc/matrix_transform.hpp>
#include <gtx/transform.hpp>

PointSprite::PointSprite()
{
}

void PointSprite::ProcessInput(int key, int action)
{
}

void PointSprite::InitScene()
{
	CompileAndLinkShader();

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	glEnable(GL_DEPTH_TEST);

	_numSprites = 50;
	_locations = new float[_numSprites * 3];
	srand((unsigned int)time(0));
	for (int i = 0; i < _numSprites; i++)
	{
		vec3 p(((float)rand() / RAND_MAX * 2.0f) - 1.0f,
			((float)rand() / RAND_MAX * 2.0f) - 1.0f,
			((float)rand() / RAND_MAX * 2.0f) - 1.0f);
		_locations[i * 3] = p.x;
		_locations[i * 3 + 1] = p.y;
		_locations[i * 3 + 2] = p.z;
	}

	GLuint handle;
	glGenBuffers(1, &handle);

	glBindBuffer(GL_ARRAY_BUFFER, handle);
	glBufferData(GL_ARRAY_BUFFER, _numSprites * 3 * sizeof(float), _locations, GL_STATIC_DRAW);
	delete[] _locations;
	_locations = nullptr;

	glGenVertexArrays(1, &_sprites);
	glBindVertexArray(_sprites);

	glBindBuffer(GL_ARRAY_BUFFER, handle);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte *)NULL + (0)));
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	GLuint w, h;
	const char * texName = "../media/texture/flower.bmp";
	BMPReader::loadTex(texName, w, h);

	_shader.SetUniform("SpriteTex", 0);
	_shader.SetUniform("Size2", 0.15f);
}

void PointSprite::Update(float t)
{
}

void PointSprite::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	vec3 cameraPos(0.0f, 0.0f, 3.0f);
	_view = glm::lookAt(cameraPos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	_model = mat4(1.0f);
	SetMatrices();

	glBindVertexArray(_sprites);
	glDrawArrays(GL_POINTS, 0, _numSprites);

	glFinish();
}

void PointSprite::Shutdown()
{
}

void PointSprite::Resize(int w, int h)
{
	glViewport(0, 0, w, h);
	_projection = glm::perspective(glm::radians(60.0f), (float)w / h, 0.3f, 100.0f);
}

void PointSprite::SetMatrices()
{
	mat4 mv = _view * _model;
	_shader.SetUniform("ModelViewMatrix", mv);
	_shader.SetUniform("ProjectionMatrix", _projection);
}

void PointSprite::CompileAndLinkShader()
{
	try
	{
		_shader.CompileShader("Shaders/PointSprite/PointSprite.vert");
		_shader.CompileShader("Shaders/PointSprite/PointSprite.geom");
		_shader.CompileShader("Shaders/PointSprite/PointSprite.frag");
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