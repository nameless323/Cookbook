#include "PixelLight.h"

#include <gtx/transform.hpp>
#include <iostream>
#include <GLFW/glfw3.h>

namespace ShadingCookbook
{
using glm::vec3;

PixelLight::PixelLight() : _angle(0), _prevTime(0), _autorotate(1), _rotateLeft(0), _rotateRight(0)
{
}

void PixelLight::ProcessInput(int key, int action)
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

void PixelLight::InitScene()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    CompileAndLinkShader();
    _plane = new Plane(50.0f, 50.0f, 1, 1);
    _teapot = new Teapot(14, glm::mat4(1.0f));
    _torus = new Torus(0.7f * 2, 0.3f * 2, 50, 50);

    _view = glm::lookAt(vec3(0.0f, 3.0f, 5.0f), vec3(0.0f, 0.75f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    _projection = mat4(1.0f);

    _angle = 0.957283f;

    _shader.SetUniform("LightIntensity", vec3(0.9f, 0.9f, 0.9f));
}

void PixelLight::Render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _shader.SetUniform("LightPosition", _view * vec4(10.0f * cos(_angle), 3.0f, 10.0f * sin(_angle), 1.0f));
    _shader.SetUniform("Kd", 0.9f, 0.5f, 0.3f);
    _shader.SetUniform("Ks", 0.95f, 0.95f, 0.95f);
    _shader.SetUniform("Ka", 0.1f, 0.1f, 0.1f);
    _shader.SetUniform("Shininess", 100.0f);

    _model = mat4(1.0f);
    _model *= glm::translate(vec3(0.0f, 0.0f, 0.0f));
    _model *= glm::rotate(glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
    SetMatrices();
    _teapot->Render();

    _shader.SetUniform("Kd", 0.7f, 0.7f, 0.7f);
    _shader.SetUniform("Ks", 0.9f, 0.9f, 0.9f);
    _shader.SetUniform("Ka", 0.1f, 0.1f, 0.1f);
    _shader.SetUniform("Shininess", 180.0f);

    _model = mat4(1.0f);
    _model *= glm::translate(vec3(0.0f, -0.45f, 0.0f));
    SetMatrices();
    _plane->Render();
}

void PixelLight::Update(float t)
{
    float dt = t - _prevTime;
    if (_prevTime == 0.0f) dt = 0.0f;
    _prevTime = t;

    _angle += 0.25f * dt;
    if (_angle > glm::two_pi<float>())
        _angle -= glm::two_pi<float>();
}

void PixelLight::Shutdown()
{
    delete _teapot;
    delete _torus;
    delete _plane;
}

void PixelLight::SetMatrices()
{
    mat4 mv = _view * _model;
    _shader.SetUniform("ModelViewMatrix", mv);
    _shader.SetUniform("NormalMatrix", mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    _shader.SetUniform("MVP", _projection * mv);
}

void PixelLight::Resize(int w, int h)
{
    glViewport(0, 0, w, h);
    _width = w;
    _height = h;
    _projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);
}

void PixelLight::CompileAndLinkShader()
{
    try
    {
        //		_shader.CompileShader("Shaders/Phong/PhongPixel/PhongPixel.vert");
        //		_shader.CompileShader("Shaders/Phong/PhongPixel/PhongPixel.frag");
        _shader.CompileShader("Shaders/Phong/PhongHalfway/PhongHalfway.vert");
        _shader.CompileShader("Shaders/Phong/PhongHalfway/PhongHalfway.frag");
        //		_shader.CompileShader("Shaders/Celshading/Celshading.vert");
        //		_shader.CompileShader("Shaders/Celshading/Celshading.frag");
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
}
