#include "Cloth.h"
#include <gtx/transform.hpp>
#include <iostream>
#include <GLFW/glfw3.h>
#include "../../Core/TGA.h"
#include <vector>
#include "../../../ingredients/tgaio.h"
using glm::vec3;
using std::vector;
#define PRIM_RESTART 0xffffff

Cloth::Cloth() :
    _width(600), _height(600), _clothVao(0), _numElements(0), _numParticles(40, 40), _clothSize(4.0f, 3.0f), _time(0.0f), _dt(0.0f), _speed(200.0), _readBuf(0)
{
}

void Cloth::ProcessInput(int key, int action)
{
}

void Cloth::InitScene()
{
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(PRIM_RESTART);
    CompileAndLinkShader();
    InitBuffers();

    _projection = glm::perspective(glm::radians(50.0f), (float)_width / _height, 1.0f, 100.0f);

    _shader.Use();
    _shader.SetUniform("LightPosition", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    _shader.SetUniform("LightIntensity", glm::vec3(1.0f));
    _shader.SetUniform("Kd", glm::vec3(0.8f));
    _shader.SetUniform("Ka", glm::vec3(0.2f));
    _shader.SetUniform("Ks", glm::vec3(0.2f));
    _shader.SetUniform("Shininess", 80.0f);

    _computeShader.Use();
    float dx = _clothSize.x / (_numParticles.x - 1);
    float dy = _clothSize.y / (_numParticles.y - 1);
    _computeShader.SetUniform("RestLengthHoriz", dx);
    _computeShader.SetUniform("RestLengthVert", dy);
    _computeShader.SetUniform("RestLengthDiag", sqrtf(dx * dx + dy * dy));

    glActiveTexture(GL_TEXTURE0);
    TGA::LoadTex("../media/texture/me_textile.tga");

}

void Cloth::Render()
{
    _computeShader.Use();
    for (int i = 0; i < 1000; i++)
    {
        glDispatchCompute(_numParticles.x / 10, _numParticles.y / 10, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        _readBuf = 1 - _readBuf;
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _posBufs[_readBuf]);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, _posBufs[1 - _readBuf]);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, _posBufs[_readBuf]);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, _posBufs[1 - _readBuf]);
    }
    _computeShaderNorm.Use();
    glDispatchCompute(_numParticles.x / 10, _numParticles.y / 10, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    _shader.Use();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _view = glm::lookAt(glm::vec3(3, 2, 5), glm::vec3(2, 1, 0), glm::vec3(0, 1, 0));
    _model = glm::mat4(1.0f);
    SetMatrices();

    glBindVertexArray(_clothVao);
    glDrawElements(GL_TRIANGLE_STRIP, _numElements, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void Cloth::InitBuffers()
{
    mat4 transf = glm::translate(glm::mat4(1.0), glm::vec3(0, _clothSize.y, 0));
    transf = glm::rotate(transf, glm::radians(-80.0f), glm::vec3(1, 0, 0));
    transf = glm::translate(transf, glm::vec3(0, -_clothSize.y, 0));

    vector<GLfloat> initPos;
    vector<GLfloat> initVel(_numParticles.x * _numParticles.y * 4, 0.0f);
    vector<float> initTc;
    float dx = _clothSize.x / (_numParticles.x - 1);
    float dy = _clothSize.y / (_numParticles.y - 1);
    float ds = 1.0f / (_numParticles.x - 1);
    float dt = 1.0f / (_numParticles.y - 1);
    vec4 p(0.0f, 0.0f, 0.0f, 1.0f);

    for (int i = 0; i < _numParticles.y; i++)
    {
        for (int j = 0; j < _numParticles.x; j++)
        {
            p.x = dx * j;
            p.y = dy * i;
            p.z = 0.0f;
            p = transf * p;
            initPos.push_back(p.x);
            initPos.push_back(p.y);
            initPos.push_back(p.z);
            initPos.push_back(1.0);

            initTc.push_back(ds * j);
            initTc.push_back(dt * i);
        }
    }

    vector<GLuint> el;
    for (int row = 0; row < _numParticles.y - 1; row++)
    {
        for (int col = 0; col < _numParticles.x; col++)
        {
            el.push_back((row + 1) * _numParticles.x + col);
            el.push_back(row * _numParticles.x + col);
        }
        el.push_back(PRIM_RESTART);
    }
}

void Cloth::Update(float t)
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


void Cloth::Shutdown()
{
}

void Cloth::SetMatrices()
{
    _shader.Use();
    mat4 mv = _view * _model;
    _shader.SetUniform("ModelViewMatrix", mv);
    _shader.SetUniform("NormalMatrix", mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    _shader.SetUniform("MVP", _projection * mv);
}

void Cloth::Resize(int w, int h)
{
    glViewport(0, 0, w, h);
    _width = w;
    _height = h;
    //    _projection = glm::perspective(glm::radians(60.0f), (float)w / h, 0.3f, 100.0f);
}

void Cloth::CompileAndLinkShader()
{
    try
    {
        _shader.CompileShader("Shaders/Particles/Particles.vert");
        _shader.CompileShader("Shaders/Particles/Particles.frag");
        _shader.Link();
        _shader.Validate();
        _shader.Use();

        _computeShader.CompileShader("Shaders/Particles/Particles.cs");
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
