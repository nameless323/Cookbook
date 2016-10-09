#include "TeapotTessDepth.h"

#include <ostream>
#include <iostream>
#include <ctime>

#include <gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <gtx/transform.hpp>

#include "../../Items/MeshAdj.h"
#include "../../../ingredients/vbomeshadj.h"

namespace ShadingCookbook
{
using glm::vec3;

TeapotTessDepth::TeapotTessDepth() : _inner(4), _outer(4), _angle(0.0), _tPrev(0.0f), _rotSpeed(glm::pi<float>() / 8.0f)
{
}

void TeapotTessDepth::ProcessInput(int key, int action)
{
    if (key == GLFW_KEY_RIGHT && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        _inner += 1;
        _inner = glm::min<int>(_inner, 32);
    }
    if (key == GLFW_KEY_LEFT && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        _inner -= 1;
        _inner = glm::max<int>(_inner, 1);
    }
    if (key == GLFW_KEY_UP && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        _outer += 1;
        _outer = glm::min<int>(_outer, 32);
    }
    if (key == GLFW_KEY_DOWN && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        _outer -= 1;
        _outer = glm::max<int>(_outer, 1);
    }
}

void TeapotTessDepth::InitScene()
{
    CompileAndLinkShader();
    _projection = glm::perspective(glm::radians(60.0f), (float)1024 / (float)768, 0.3f, 100.0f);

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    glEnable(GL_DEPTH_TEST);
    _angle = glm::pi<float>() / 3.0f;
    _teapot = new VBOTeapotPatch();

    _shader.Use();
    _shader.SetUniform("MinTessLevel", 2);
    _shader.SetUniform("MaxTessLevel", 32);
    _shader.SetUniform("MaxDepth", 20.0f);
    _shader.SetUniform("MinDepth", 2.0f);
    _shader.SetUniform("LineWidth", 0.8f);
    _shader.SetUniform("LineColor", vec4(0.05f, 0.0f, 0.05f, 1.0f));
    _shader.SetUniform("LightPosition", vec4(0.0f, 0.0f, 0.0f, 1.0f));
    _shader.SetUniform("LightIntensity", vec3(1.0f, 1.0f, 1.0f));
    _shader.SetUniform("Kd", vec3(0.9f, 0.9f, 1.0f));

    glPatchParameteri(GL_PATCH_VERTICES, 16);
}

void TeapotTessDepth::Update(float t)
{
    float deltaT = t - _tPrev;
    if (_tPrev == 0.0f) deltaT = 0.0f;
    _tPrev = t;

    _angle += _rotSpeed * deltaT;
    if (_angle > glm::two_pi<float>()) _angle -= glm::two_pi<float>();
}

void TeapotTessDepth::Render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    vec3 cameraPos(0.0f, 1.0f, 6.25f);
    _view = lookAt(cameraPos,
                   vec3(0.0f, 0.0f, 0.0f),
                   vec3(0.0f, 1.0f, 0.0f));

    _model = mat4(1.0f);
    _model = translate(_model, vec3(-2.0f, 0.0f, 0.0f));
    _model = translate(_model, vec3(0.0f, -1.5f, 0.0f));
    _model = rotate(_model, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
    SetMatrices();
    _teapot->render();

    _model = mat4(1.0f);
    _model = translate(_model, vec3(2.0f, 0.0f, -5.0f));
    _model = translate(_model, vec3(0.0f, -1.5f, 0.0f));
    _model = rotate(_model, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
    SetMatrices();
    _teapot->render();

    _model = mat4(1.0f);
    _model = translate(_model, vec3(7.0f, 0.0f, -10.0f));
    _model = translate(_model, vec3(0.0f, -1.5f, 0.0f));
    _model = rotate(_model, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
    SetMatrices();
    _teapot->render();

    _model = mat4(1.0f);
    _model = translate(_model, vec3(17.0f, 0.0f, -20.0f));
    _model = translate(_model, vec3(0.0f, -1.5f, 0.0f));
    _model = rotate(_model, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
    SetMatrices();
    _teapot->render();
}

void TeapotTessDepth::Shutdown()
{
    delete _teapot;
}

void TeapotTessDepth::Resize(int w, int h)
{
    glViewport(0, 0, w, h);
    _projection = glm::perspective(glm::radians(60.0f), (float)w / h, 0.3f, 100.0f);

    float w2 = w / 2.0f;
    float h2 = h / 2.0f;
    _viewport = mat4(vec4(w2, 0.0f, 0.0f, 0.0f),
                     vec4(0.0f, h2, 0.0f, 0.0f),
                     vec4(0.0f, 0.0f, 1.0f, 0.0f),
                     vec4(w2 + 0, h2 + 0, 0.0f, 1.0f));
}

void TeapotTessDepth::SetMatrices()
{
    mat4 mv = _view * _model;
    _shader.Use();
    _shader.SetUniform("ModelViewMatrix", mv);
    _shader.SetUniform("MVP", _projection * mv);
    _shader.SetUniform("NormalMatrix",
                       mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    _shader.SetUniform("MVP", _projection * mv);
    _shader.SetUniform("ViewportMatrix", _viewport);
}

void TeapotTessDepth::CompileAndLinkShader()
{
    try
    {
        _shader.CompileShader("Shaders/TeapotTessDepth/TeapotTessDepth.vert");
        _shader.CompileShader("Shaders/TeapotTessDepth/TeapotTessDepth.tesc");
        _shader.CompileShader("Shaders/TeapotTessDepth/TeapotTessDepth.tese");
        _shader.CompileShader("Shaders/TeapotTessDepth/TeapotTessDepth.geom");
        _shader.CompileShader("Shaders/TeapotTessDepth/TeapotTessDepth.frag");
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
