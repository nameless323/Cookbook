#include "NightVision.h"
#include <gtx/transform.hpp>
#include <iostream>
#include <GLFW/glfw3.h>
#include "../../Core/TGA.h"
#include <GL/glew.h>
using glm::vec3;

NightVision::NightVision() : _angle(0), _prevTime(0), _autorotate(1), _rotateLeft(0), _rotateRight(0)
{
}

void NightVision::ProcessInput(int key, int action)
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

void NightVision::InitScene()
{
	CompileAndLinkShader();

	glEnable(GL_DEPTH_TEST);

	_plane = new Plane(50.0f, 50.0f, 1, 1);
	_cube = new Cube();

	_projection = mat4(1.0f);

	_teapot = new Teapot(14, mat4(1.0f));

	_angle = glm::radians(140.0f);

	_shader.SetUniform("Light.Intensity", vec3(1.0f, 1.0f, 1.0f));
	SetupFBO();

	GLuint whiteTexHandle;
	GLubyte whiteTex[] = { 255, 255, 255, 255 };
	glActiveTexture(GL_TEXTURE1);
	glGenTextures(1, &whiteTexHandle);
	glBindTexture(GL_TEXTURE_2D, whiteTexHandle);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 1, 1);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, whiteTex);
}

void NightVision::Render()
{
	glBindFramebuffer(GL_FRAMEBUFFER, _renderFBO);
	Pass1();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	RenderScene();
}

void NightVision::Update(float t)
{
	float dt = t - _prevTime;
	if (_prevTime == 0.0f) dt = 0.0f;
	_prevTime = t;

	_angle += 0.25f * dt;
	if (_angle > glm::two_pi<float>())
		_angle -= glm::two_pi<float>();
}


void NightVision::SetupFBO()
{
	glGenFramebuffers(1, &_renderFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, _renderFBO);

	GLuint renderTex;
	glGenTextures(1, &renderTex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, renderTex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 512, 512);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTex, 0);

	GLuint depthBuf;
	glGenRenderbuffers(1, &depthBuf);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 512, 512);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf);

	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);

	GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (result == GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer complete" << std::endl;
	else
		std::cout << "Framebuffer error: " << result << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void NightVision::Pass1()
{
}

void NightVision::Pass2()
{
}

void NightVision::RenderScene()
{
	_shader.SetUniform("RenderTex", 0);
	glViewport(0, 0, _width, _height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	vec3 cameraPos = vec3(2.0f * cos(_angle), 1.5f, 2.0f * sin(_angle));
	_view = glm::lookAt(cameraPos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	_projection = glm::perspective(glm::radians(45.0f), (float)_width / _height, 0.3f, 100.0f);

	_shader.SetUniform("Light.Position", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	_shader.SetUniform("Material.Kd", 0.4f, 0.4f, 0.4f);
	_shader.SetUniform("Material.Ks", 0.0f, 0.0f, 0.0f);
	_shader.SetUniform("Material.Ka", 0.1f, 0.1f, 0.1f);
	_shader.SetUniform("Material.Shininess", 1.0f);
	_model = mat4(1.0f);
	SetMatrices();
	_cube->Render();
}


void NightVision::Shutdown()
{
	delete _plane;
	delete _cube;
	delete _teapot;
}

void NightVision::SetMatrices()
{
	mat4 mv = _view * _model;
	_shader.SetUniform("ModelViewMatrix", mv);
	_shader.SetUniform("NormalMatrix",
	                   glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
	_shader.SetUniform("MVP", _projection * mv);
}

void NightVision::Resize(int w, int h)
{
	glViewport(0, 0, w, h);
	_width = w;
	_height = h;
	_projection = glm::perspective(glm::radians(50.0f), (float)w / h, 0.3f, 100.0f);
}

void NightVision::CompileAndLinkShader()
{
	try
	{
		_shader.CompileShader("Shaders/NightVision/NightVision.vert");
		_shader.CompileShader("Shaders/NightVision/NightVision.frag");
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