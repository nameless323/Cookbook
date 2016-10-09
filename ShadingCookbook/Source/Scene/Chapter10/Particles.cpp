#include "Particles.h"

#include <gtx/transform.hpp>
#include <iostream>
#include <GLFW/glfw3.h>
#include <vector>

#include "../../Core/TGA.h"

namespace ShadingCookbook
{
using glm::vec3;
using std::vector;

#define PRIM_RESTART 0xffffff

Particles::Particles() :
    _width(600), _height(600),
    _numParticles(100, 100, 100),
    _time(0.0f), _dt(0.0f), _speed(35.0f), _angle(0.0f),
    _bh1(5, 0, 0, 1), _bh2(-5, 0, 0, 1)
{
    _totalParticles = _numParticles.x * _numParticles.y * _numParticles.z;
}

void Particles::ProcessInput(int key, int action)
{
}

void Particles::InitScene()
{
    CompileAndLinkShader();
    InitBuffers();

    glClearColor(1, 1, 1, 1);
    _projection = glm::perspective(glm::radians(50.0f), (float)_width / _height, 1.0f, 100.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Particles::Render()
{
    mat4 rot = glm::rotate(mat4(1.0f), glm::radians(_angle), glm::vec3(0, 0, 1));
    vec3 att1 = vec3(rot * _bh1);
    vec3 att2 = vec3(rot * _bh2);

    _computeShader.Use();
    _computeShader.SetUniform("BlackHolePos1", att1);
    _computeShader.SetUniform("BlackHolePos2", att2);
    glDispatchCompute(_totalParticles / 1000, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    _shader.Use();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _view = lookAt(vec3(2, 0, 20), vec3(0, 0, 0), vec3(0, 1, 0));
    _model = glm::mat4(1.0f);
    SetMatrices();

    glPointSize(1.0f);
    _shader.SetUniform("Color", vec4(0, 0, 0, 0.2f));
    glBindVertexArray(_vao);
    glDrawArrays(GL_POINTS, 0, _totalParticles);
    glBindVertexArray(0);

    glPointSize(5.0f);
    GLfloat data[] = {att1.x, att1.y, att1.z, 1.0f, att2.x, att2.y, att2.z, 1.0f};
    glBindBuffer(GL_ARRAY_BUFFER, _bhBuf);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 8 * sizeof(GLfloat), data);
    _shader.SetUniform("Color", vec4(1, 1, 0, 1.0));
    glBindVertexArray(_bhVao);
    glDrawArrays(GL_POINTS, 0, 2);

    glBindVertexArray(0);
}

void Particles::InitBuffers()
{
    vector<GLfloat> initPos;
    vector<GLfloat> initVel(_totalParticles * 4, 0.0f);
    vec4 p(0.0f, 0.0f, 0.0f, 1.0f);
    GLfloat dx = 2.0f / (_numParticles.x - 1);
    GLfloat dy = 2.0f / (_numParticles.y - 1);
    GLfloat dz = 2.0f / (_numParticles.z - 1);
    mat4 transf = translate(mat4(1.0f), vec3(-1, -1, -1));
    for (int i = 0; i < _numParticles.x; i++)
    {
        for (int j = 0; j < _numParticles.y; j++)
        {
            for (int k = 0; k < _numParticles.z; k++)
            {
                p.x = dx * i;
                p.y = dy * j;
                p.z = dz * k;
                p.w = 1.0f;
                p = transf * p;
                initPos.push_back(p.x);
                initPos.push_back(p.y);
                initPos.push_back(p.z);
                initPos.push_back(p.w);
            }
        }
    }

    GLuint bufs[2];
    glGenBuffers(2, bufs);
    GLuint posBuf = bufs[0];
    GLuint velBuf = bufs[1];

    GLuint buffSize = _totalParticles * 4 * sizeof(GLfloat);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, posBuf);
    glBufferData(GL_SHADER_STORAGE_BUFFER, buffSize, &initPos[0], GL_DYNAMIC_DRAW);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, velBuf);
    glBufferData(GL_SHADER_STORAGE_BUFFER, buffSize, &initVel[0], GL_DYNAMIC_COPY);

    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    glBindBuffer(GL_ARRAY_BUFFER, posBuf);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    glGenBuffers(1, &_bhBuf);
    glBindBuffer(GL_ARRAY_BUFFER, _bhBuf);
    GLfloat data[] = {_bh1.x, _bh1.y, _bh1.z, _bh1.w, _bh2.x, _bh2.y, _bh2.z, _bh2.w};
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), data, GL_DYNAMIC_DRAW);

    glGenVertexArrays(1, &_bhVao);
    glBindVertexArray(_bhVao);

    glBindBuffer(GL_ARRAY_BUFFER, _bhBuf);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void Particles::Update(float t)
{
    if (_time == 0.0f)
    {
        _dt = 0.0f;
    }
    else
    {
        _dt = t - _time;
    }
    _time = t;
    _angle += _speed * _dt;
    if (_angle > 360.0f) _angle -= 360.0f;
}

void Particles::Shutdown()
{
}

void Particles::SetMatrices()
{
    _shader.Use();
    mat4 mv = _view * _model;
    _shader.SetUniform("ModelViewMatrix", mv);
    _shader.SetUniform("NormalMatrix", mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    _shader.SetUniform("MVP", _projection * mv);
}

void Particles::Resize(int w, int h)
{
    glViewport(0, 0, w, h);
    _width = w;
    _height = h;
    //    _projection = glm::perspective(glm::radians(60.0f), (float)w / h, 0.3f, 100.0f);
}

void Particles::CompileAndLinkShader()
{
    try
    {
        _shader.CompileShader("Shaders/Particles/Particles.vert");
        _shader.CompileShader("Shaders/Particles/Particles.frag");
        _shader.Link();
        _shader.Validate();
        _shader.Use();

        _computeShader.CompileShader("Shaders/Particles/Particles.comp");
        _computeShader.Link();
        _computeShader.Validate();
        _computeShader.Use();
    }
    catch (ShaderProgramException& e)
    {
        std::cerr << e.what() << std::endl;
        //exit(EXIT_FAILURE);
    }
}
}
