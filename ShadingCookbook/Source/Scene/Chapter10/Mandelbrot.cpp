#include "Mandelbrot.h"

#include <gtx/transform.hpp>
#include <iostream>
#include <GLFW/glfw3.h>
#include <vector>

#include "../../Core/TGA.h"

namespace ShadingCookbook
{
using glm::vec3;
using std::vector;

Mandelbrot::Mandelbrot() :
    _width(600), _height(600), _dataBuf(0), _fsQuad(0),
    _center(0.001643721971153f, 0.822467633298876f), _cHeight(2.0f),
    _time(0.0f), _dt(0.0f), _speed(200.0f), _angle(0.0f), _rotSpeed(60.0f),
    _paused(false)
{
}

void Mandelbrot::ProcessInput(int key, int action)
{
    if (key == GLFW_KEY_P && action == GLFW_PRESS)
        _paused = !_paused;
}

void Mandelbrot::SetWindow()
{
    _computeShader.Use();
    float ar = 1.0f;
    float cwidth = _cHeight * ar;

    glm::vec4 bbox(_center.x - cwidth / 2.0f, _center.y - _cHeight / 2.0f,
                   _center.x + cwidth / 2.0f, _center.y + _cHeight / 2.0f);
    _computeShader.SetUniform("CompWindow", bbox);
}

void Mandelbrot::InitScene()
{
    CompileAndLinkShader();
    InitBuffers();
    SetWindow();

    glEnable(GL_DEPTH_TEST);
    _shader.Use();
    _shader.SetUniform("LightPosition", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    _shader.SetUniform("LightIntensity", glm::vec3(1.0f));
    _shader.SetUniform("Kd", glm::vec3(0.8f));
    _shader.SetUniform("Ka", glm::vec3(0.2f));
    _shader.SetUniform("Ks", glm::vec3(0.2f));
    _shader.SetUniform("Shininess", 80.0f);

    _cube = new Cube;
}

void Mandelbrot::Render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _computeShader.Use();
    glDispatchCompute(256 / 32, 256 / 32, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    _shader.Use();
    mat4 view = lookAt(vec3(0, 0, 2), vec3(0, 0, 0), vec3(0, 1, 0));
    mat4 model = rotate(mat4(1.0f), glm::radians(_angle), vec3(1, 1.5f, 0.5f));
    mat4 mv = view * model;
    mat3 norm = mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2]));
    mat4 proj = glm::perspective(glm::radians(60.0f), (float)_width / _height, 1.0f, 100.0f);

    _shader.SetUniform("ModelViewMatrix", mv);
    _shader.SetUniform("NormalMatrix", norm);
    _shader.SetUniform("MVP", proj * mv);

    _cube->Render();
}

void Mandelbrot::InitBuffers()
{
    GLuint imgTex;
    glGenTextures(1, &imgTex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, imgTex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 256, 256);
    glBindImageTexture(0, imgTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);
}

void Mandelbrot::Update(float t)
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
    if (_paused) return;
    _angle += _speed * _dt;
    if (_angle > 360.0f) _angle -= 360.0f;
}

void Mandelbrot::Shutdown()
{
}

void Mandelbrot::SetMatrices()
{
}

void Mandelbrot::Resize(int w, int h)
{
    glViewport(0, 0, w, h);
    _width = w;
    _height = h;
    //    _projection = glm::perspective(glm::radians(60.0f), (float)w / h, 0.3f, 100.0f);
}

void Mandelbrot::CompileAndLinkShader()
{
    try
    {
        _shader.CompileShader("Shaders/ADS/ADS.vert");
        _shader.CompileShader("Shaders/ADS/ADS.frag");
        _shader.Link();
        _shader.Validate();
        _shader.Use();

        _computeShader.CompileShader("Shaders/Mandelbrot/Mandelbrot.comp");
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
