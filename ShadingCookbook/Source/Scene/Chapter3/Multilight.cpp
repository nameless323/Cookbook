#include "Multilight.h"

#include <gtx/transform.hpp>
#include <iostream>
#include <sstream>
#include <GLFW/glfw3.h>

namespace ShadingCookbook
{
using glm::vec3;

Multilight::Multilight() : _angle(0), _prevTime(0), _autorotate(1), _rotateLeft(0), _rotateRight(0)
{
}

void Multilight::ProcessInput(int key, int action)
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

void Multilight::InitScene()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    CompileAndLinkShader();

    _plane = new Plane(10.0f, 10.0f, 100, 100);
    _piggy = new Mesh("./media/pig_triangulated.obj", true);
    _view = lookAt(vec3(0.5f, 0.75f, 0.75f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

    _projection = mat4(1.0f);
    float x, z;
    for (int i = 0; i < 5; i++)
    {
        std::stringstream name;
        name << "lights[" << i << "].Position";
        x = 2.0f * cosf((glm::two_pi<float>() / 5) * i);
        z = 2.0f * sinf((glm::two_pi<float>() / 5) * i);
        _shader.SetUniform(name.str().c_str(), _view * vec4(x, 1.2f, z + 1.0f, 1.0f));
    }
    _shader.SetUniform("lights[0].Intensity", vec3(0.0f, 0.8f, 0.8f));
    _shader.SetUniform("lights[1].Intensity", vec3(0.0f, 0.0f, 0.8f));
    _shader.SetUniform("lights[2].Intensity", vec3(0.8f, 0.0f, 0.0f));
    _shader.SetUniform("lights[3].Intensity", vec3(0.0f, 0.8f, 0.0f));
    _shader.SetUniform("lights[4].Intensity", vec3(0.8f, 0.8f, 0.8f));
}

void Multilight::Render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _shader.SetUniform("Kd", 0.4f, 0.4f, 0.4f);
    _shader.SetUniform("Ks", 0.9f, 0.9f, 0.9f);
    _shader.SetUniform("Ka", 0.1f, 0.1f, 0.1f);
    _shader.SetUniform("Shininess", 180.0f);

    _model = mat4(1.0f);
    _model *= glm::rotate(glm::radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
    SetMatrices();
    _piggy->Render();
    _shader.SetUniform("Kd", 0.1f, 0.1f, 0.1f);
    _shader.SetUniform("Ks", 0.9f, 0.9f, 0.9f);
    _shader.SetUniform("Ka", 0.1f, 0.1f, 0.1f);
    _shader.SetUniform("Shininess", 180.0f);

    _model = mat4(1.0f);
    _model *= glm::translate(vec3(0.0f, -0.45f, 0.0f));
    SetMatrices();
    _plane->Render();
}

void Multilight::Update(float t)
{
}

void Multilight::Shutdown()
{
    if (_piggy != nullptr)
    {
        delete _piggy;
        _piggy = nullptr;
    }
    if (_plane != nullptr)
    {
        delete _plane;
        _plane = nullptr;
    }
}

void Multilight::SetMatrices()
{
    mat4 mv = _view * _model;
    _shader.SetUniform("ModelViewMatrix", mv);
    _shader.SetUniform("NormalMatrix", mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    _shader.SetUniform("MVP", _projection * mv);
}

void Multilight::Resize(int w, int h)
{
    glViewport(0, 0, w, h);
    _width = w;
    _height = h;
    _projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);
}

void Multilight::CompileAndLinkShader()
{
    try
    {
        _shader.CompileShader("Shaders/Multilight/Multilight.vert");
        _shader.CompileShader("Shaders/Multilight/Multilight.frag");
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
