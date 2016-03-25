#include "Bloom.h"
#include <gtx/transform.hpp>
#include <iostream>
#include <sstream>
#include <GLFW/glfw3.h>
#include "../../Core/TGA.h"
using glm::vec3;

Bloom::Bloom() : _angle(0), _prevTime(0), _autorotate(1), _rotateLeft(0), _rotateRight(0), _rotationSpeed(glm::pi<float>() / 8.0f), _width(1024), _height(768), _doToneMap(true)
{
}

void Bloom::ProcessInput(int key, int action)
{
	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
		_autorotate = !_autorotate;
	if (key == GLFW_KEY_T && action == GLFW_PRESS)
		_doToneMap = !_doToneMap;

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

void Bloom::InitScene()
{
	CompileAndLinkShader();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	_plane = new Plane(20.0f, 10.0f, 1, 1);
	_teapot = new Teapot(14, mat4(1.0f));
	_sphere = new Sphere(2.0f, 50, 50);

	_projection = mat4(1.0f);
	_angle = glm::pi<float>() / 2.0f;
	
	vec3 intens = vec3(0.6f);
	_shader.SetUniform("Lights[0].Intensity", intens);
	_shader.SetUniform("Lights[1].Intensity", intens);
	_shader.SetUniform("Lights[2].Intensity", intens);
	
	GLfloat verts[] =
	{
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f
	};
	GLfloat tc[] =
	{
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f
	};

	glGenVertexArrays(1, &_fsQuad);
	glBindVertexArray(_fsQuad);

	GLuint handle[2];
	glGenBuffers(2, handle);
	glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
	glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(GLfloat), verts, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
	glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(GLfloat), tc, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
	SetupFBO();

	GLuint shaderHandle = _shader.GetHandle();
	_pass1Index = glGetSubroutineIndex(shaderHandle, GL_FRAGMENT_SHADER, "pass1");
	_pass2Index = glGetSubroutineIndex(shaderHandle, GL_FRAGMENT_SHADER, "pass2");
	_pass3Index = glGetSubroutineIndex(shaderHandle, GL_FRAGMENT_SHADER, "pass3");
	_pass4Index = glGetSubroutineIndex(shaderHandle, GL_FRAGMENT_SHADER, "pass4");
	_pass5Index = glGetSubroutineIndex(shaderHandle, GL_FRAGMENT_SHADER, "pass5");

	_shader.SetUniform("LumThresh", 1.7f);

	float weights[10], sum, sigma2 = 25.0f;

	weights[0] = Gauss(0, sigma2);
	sum = weights[0];
	for (int i = 1; i < 10; i++) {
		weights[i] = Gauss(float(i), sigma2);
		sum += 2 * weights[i];
	}

	for (int i = 0; i < 10; i++) {
		std::stringstream uniName;
		uniName << "Weight[" << i << "]";
		float val = weights[i] / sum;
		_shader.SetUniform(uniName.str().c_str(), val);
	}

	GLuint samplers[2];
	glGenSamplers(2, samplers);
	_linearSampler = samplers[0];
	_nearestSampler = samplers[1];

	GLfloat border[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	glSamplerParameteri(_nearestSampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glSamplerParameteri(_nearestSampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glSamplerParameteri(_nearestSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glSamplerParameteri(_nearestSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glSamplerParameterfv(_nearestSampler, GL_TEXTURE_BORDER_COLOR, border);

	glSamplerParameteri(_linearSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameteri(_linearSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glSamplerParameteri(_linearSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glSamplerParameteri(_linearSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glSamplerParameterfv(_linearSampler, GL_TEXTURE_BORDER_COLOR, border);

	glBindSampler(0, _nearestSampler);
	glBindSampler(1, _nearestSampler);
	glBindSampler(2, _nearestSampler);
}

void Bloom::Render()
{
	Pass1();
	ComputeAveLuminance();
	Pass2();
	Pass3();
	Pass4();
	Pass5();
}

void Bloom::Update(float t)
{
	float dt = t - _prevTime;
	if (_prevTime == 0.0f) dt = 0.0f;
	_prevTime = t;

	_angle += 0.25f * dt;
	if (_angle > glm::two_pi<float>())
		_angle -= glm::two_pi<float>();
}


void Bloom::SetupFBO()
{
	glGenFramebuffers(1, &_hdrFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, _hdrFBO);

	glGenTextures(1, &_hdrTex);
	glBindTexture(GL_TEXTURE_2D, _hdrTex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, _width, _height);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _hdrTex, 0);

	GLuint depth;
	glGenRenderbuffers(1, &depth);
	glBindRenderbuffer(GL_RENDERBUFFER, depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _width, _height);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth);

	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);


	glGenFramebuffers(1, &_blurFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, _blurFBO);

	glGenTextures(1, &_tex1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _tex1);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, _bloomBufWidth, _bloomBufHeight);

	glGenTextures(1, &_tex2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, _tex2);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, _bloomBufWidth, _bloomBufHeight);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _tex1, 0);
	glDrawBuffers(1, drawBuffers);


	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Bloom::Pass1()
{
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glViewport(0, 0, _width, _height);
	glBindFramebuffer(GL_FRAMEBUFFER, _hdrFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &_pass1Index);

	_view = glm::lookAt(vec3(2.0f, 0.0f, 14.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	_projection = glm::perspective(glm::radians(60.0f), (float)_width / _height, 0.3f, 100.0f);

	DrawScene();
	glFinish();
}

void Bloom::Pass2()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &_pass2Index);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	_model = mat4(1.0f);
	_view = mat4(1.0f);
	_projection = mat4(1.0f);
	SetMatrices();

	glBindVertexArray(_fsQuad);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Bloom::Pass3()
{
	
}

void Bloom::Pass4()
{
	
}

void Bloom::Pass5()
{
	
}


float Bloom::Gauss(float x, float sigma2)
{
	double coeff = 1.0 / (glm::two_pi<double>() * sigma2);
	double expon = -(x*x) / (2.0 * sigma2);
	return (float)(coeff * exp(expon));
}


void Bloom::DrawScene()
{
	vec3 intense = vec3(1.0f);
	_shader.SetUniform("Lights[0].Intensity", intense);
	_shader.SetUniform("Lights[1].Intensity", intense);
	_shader.SetUniform("Lights[2].Intensity", intense);

	vec4 lightPos = vec4(0.0f, 4.0f, 2.5f, 1.0f);
	lightPos.x = -7.0f;
	_shader.SetUniform("Lights[0].Position", _view * lightPos);
	lightPos.x = 0.0f;
	_shader.SetUniform("Lights[1].Position", _view * lightPos);
	lightPos.x = 7.0f;
	_shader.SetUniform("Lights[2].Position", _view * lightPos);

	_shader.SetUniform("Material.Kd", 0.9f, 0.3f, 0.2f);
	_shader.SetUniform("Material.Ks", 1.0f, 1.0f, 1.0f);
	_shader.SetUniform("Material.Ka", 0.2f, 0.2f, 0.2f);
	_shader.SetUniform("Material.Shine", 100.0f);

	_model = glm::rotate(mat4(1.0f), glm::radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
	SetMatrices();
	_plane->Render();

	// The bottom plane
	_model = glm::translate(mat4(1.0f), vec3(0.0f, -5.0f, 0.0f));
	SetMatrices();
	_plane->Render();

	// Top plane
	_model = glm::translate(mat4(1.0f), vec3(0.0f, 5.0f, 0.0f));
	_model = glm::rotate(_model, glm::radians(180.0f), vec3(1.0f, 0.0f, 0.0f));
	SetMatrices();
	_plane->Render();

	_shader.SetUniform("Material.Kd", vec3(0.4f, 0.9f, 0.4f));
	_model = glm::translate(mat4(1.0f), vec3(-3.0f, -3.0f, 2.0f));
	SetMatrices();
	_sphere->Render();

	_shader.SetUniform("Material.Kd", vec3(0.4f, 0.4f, 0.9f));
	_model = glm::translate(mat4(1.0f), vec3(3.0f, -5.0f, 1.5f));
	_model = glm::rotate(_model, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
	//model = glm::scale(model, vec3(0.f));
	SetMatrices();
	_teapot->Render();
}

void Bloom::ComputeAveLuminance()
{
	GLfloat* data = new GLfloat[_width * _height * 3];
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _hdrTex);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, data);
	float sum = 0.0f;
	for (int i = 0; i < _width * _height; i++)
	{
		float lum = glm::dot(vec3(data[i * 3 + 0], data[i * 3 + 1], data[i * 3 + 2]), vec3(0.2126f, 0.7152f, 0.0722f));
		sum += logf(lum + 0.00001f);
	}
	_shader.SetUniform("AveLum", expf(sum / (_width*_height)));
	delete[] data;
}


void Bloom::Shutdown()
{
	delete _plane;
	delete _sphere;
	delete _teapot;
	//del framebufs and renderbufs
}

void Bloom::SetMatrices()
{
	mat4 mv = _view * _model;
	_shader.SetUniform("ModelViewMatrix", mv);
	_shader.SetUniform("NormalMatrix",
		mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
	_shader.SetUniform("MVP", _projection * mv);
}

void Bloom::Resize(int w, int h)
{
	glViewport(0, 0, w, h);
	_width = w;
	_height = h;
	_projection = glm::perspective(glm::radians(50.0f), (float)w / h, 0.3f, 100.0f);
}

void Bloom::CompileAndLinkShader()
{
	try
	{
		_shader.CompileShader("Shaders/Tonemapping/Tonemapping.vert");
		_shader.CompileShader("Shaders/Tonemapping/Tonemapping.frag");
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