#include "SimpleParticles.h"

#include <ostream>
#include <iostream>
#include <ctime>

#include <gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <gtx/transform.hpp>

#include "../../Items/NoiseTex.h"
#include "../../../ingredients/bmpreader.h"

namespace ShadingCookbook
{
using glm::vec3;
using glm::mat4;

SimpleParticles::SimpleParticles() : _width(1024), _height(768), _time(0.0f)
{
}

void SimpleParticles::ProcessInput(int key, int action)
{
}

void SimpleParticles::InitScene()
{
    CompileAndLinkShader();

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPointSize(10.0f);

    _plane = new Plane(13.0f, 10.0f, 200, 2);
    _projection = mat4(1.0f);
    _angle = glm::half_pi<float>();

    InitBuffers();

    const char* texName = "./media/texture/bluewater.bmp";
    glActiveTexture(GL_TEXTURE0);
    BMPReader::loadTex(texName);

    _shader.SetUniform("ParticleTex", 0);
    _shader.SetUniform("ParticleLifetime", 3.5f);
    _shader.SetUniform("Gravity", vec3(0.0f, -0.2f, 0.0f));
}

void SimpleParticles::Update(float t)
{
    _time = t;
}

void SimpleParticles::Render()
{
    _shader.SetUniform("Time", _time);
    glClear(GL_COLOR_BUFFER_BIT);

    _view = glm::lookAt(vec3(3.0f * cos(_angle), 1.5f, 3.0f * sin(_angle)), vec3(0.0f, 1.5f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    _model = mat4(1.0f);
    SetMatrices();

    glBindVertexArray(_particles);
    glDrawArrays(GL_POINTS, 0, _nParticles);
}

void SimpleParticles::Shutdown()
{
    delete _plane;
}

void SimpleParticles::Resize(int w, int h)
{
    glViewport(0, 0, w, h);
    _width = w;
    _height = h;
    _projection = glm::perspective(glm::radians(55.0f), (float)w / h, 0.3f, 100.0f);
}

void SimpleParticles::SetMatrices()
{
    mat4 mv = _view * _model;
    _shader.SetUniform("MVP", _projection * mv);
}

void SimpleParticles::CompileAndLinkShader()
{
    try
    {
        _shader.CompileShader("Shaders/SimpleParticles/SimpleParticles.vert");
        _shader.CompileShader("Shaders/SimpleParticles/SimpleParticles.frag");
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

void SimpleParticles::InitBuffers()
{
    _nParticles = 8000;
    glGenBuffers(1, &_initVel);
    glGenBuffers(1, &_startTime);

    int size = _nParticles * 3 * sizeof(float);
    glBindBuffer(GL_ARRAY_BUFFER, _initVel);
    glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, _startTime);
    glBufferData(GL_ARRAY_BUFFER, _nParticles * sizeof(float), nullptr, GL_STATIC_DRAW);

    vec3 v(0.0);
    float velocity, theta, phi;
    GLfloat* data = new GLfloat[_nParticles * 3];

    for (unsigned int i = 0; i < _nParticles; i++)
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

    glBindBuffer(GL_ARRAY_BUFFER, _initVel);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);

    delete[] data;
    data = new GLfloat[_nParticles];
    float time = 0.0f;
    float rate = 0.00075f;
    for (unsigned int i = 0; i < _nParticles; i++)
    {
        data[i] = time;
        time += rate;
    }
    glBindBuffer(GL_ARRAY_BUFFER, _startTime);
    glBufferSubData(GL_ARRAY_BUFFER, 0, _nParticles * sizeof(float), data);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    delete[] data;

    glGenVertexArrays(1, &_particles);
    glBindVertexArray(_particles);
    glBindBuffer(GL_ARRAY_BUFFER, _initVel);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, _startTime);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

float SimpleParticles::RandFloat()
{
    return (float)rand() / RAND_MAX;
}
}
