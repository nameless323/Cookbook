#include "ParticlesInstancing.h"

#include <ostream>
#include <iostream>
#include <ctime>

#include <gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <gtx/transform.hpp>

#include "../../Items/NoiseTex.h"
#include "../../../ingredients/bmpreader.h"

using glm::vec3;
using glm::mat4;

namespace ShadingCookbook
{
ParticlesInstancing::ParticlesInstancing() : _width(1024), _height(768), _time(0.0f)
{
}

void ParticlesInstancing::ProcessInput(int key, int action)
{
}

void ParticlesInstancing::InitScene()
{
    CompileAndLinkShader();

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    glEnable(GL_DEPTH_TEST);

    _plane = new Plane(13.0f, 10.0f, 200, 2);
    _projection = mat4(1.0f);
    float c = 0.15f;
    _torus = new Torus(0.7f * c, 0.3f * c, 20, 20);

    _angle = glm::half_pi<float>();

    InitBuffers();


    _shader.SetUniform("Light.Intensity", vec3(1.0f, 1.0f, 1.0f));
    _shader.SetUniform("ParticleLifetime", 3.5f);
    _shader.SetUniform("Gravity", vec3(0.0f, -0.2f, 0.0f));
}

void ParticlesInstancing::Update(float t)
{
    _dt = t - _time;
    _time = t;
}

void ParticlesInstancing::Render()
{
    _shader.SetUniform("Time", _time);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _view = glm::lookAt(vec3(3.0f * cos(_angle), 1.5f, 3.0f * sin(_angle)), vec3(0.0f, 1.5f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    _shader.SetUniform("Light.Position", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    _shader.SetUniform("Material.Kd", 0.9f, 0.5f, 0.2f);
    _shader.SetUniform("Material.Ks", 0.95f, 0.95f, 0.95f);
    _shader.SetUniform("Material.Ka", 0.1f, 0.1f, 0.1f);
    _shader.SetUniform("Material.Shininess", 100.0f);
    _model = mat4(1.0f);
    SetMatrices();

    glBindVertexArray(_torus->GetVertexArrayHandle());
    glDrawElementsInstanced(GL_TRIANGLES, 6 * 20 * 20, GL_UNSIGNED_INT, 0, _nParticles);
}

void ParticlesInstancing::Shutdown()
{
    delete _plane;
    delete _torus;
}

void ParticlesInstancing::Resize(int w, int h)
{
    glViewport(0, 0, w, h);
    _width = w;
    _height = h;
    _projection = glm::perspective(glm::radians(55.0f), (float)w / h, 0.3f, 100.0f);
}

void ParticlesInstancing::SetMatrices()
{
    mat4 mv = _view * _model;
    _shader.SetUniform("ModelViewMatrix", mv);
    _shader.SetUniform("NormalMatrix",
                       mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    _shader.SetUniform("ProjectionMatrix", _projection);
}

void ParticlesInstancing::CompileAndLinkShader()
{
    try
    {
        _shader.CompileShader("Shaders/ParticlesInstancing/ParticlesInstancing.vert");
        _shader.CompileShader("Shaders/ParticlesInstancing/ParticlesInstancing.frag");
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

void ParticlesInstancing::InitBuffers()
{
    _nParticles = 500;
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

    glBindVertexArray(_torus->GetVertexArrayHandle());
    glBindBuffer(GL_ARRAY_BUFFER, _initVel);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(3);

    glBindBuffer(GL_ARRAY_BUFFER, _startTime);
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(4);

    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);

    glBindVertexArray(0);
}

float ParticlesInstancing::RandFloat()
{
    return (float)rand() / RAND_MAX;
}
}
