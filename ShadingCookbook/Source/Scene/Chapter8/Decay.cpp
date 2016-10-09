#include "Decay.h"

#include <ostream>
#include <iostream>
#include <ctime>
#include <gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <gtx/transform.hpp>

#include "../../Items/NoiseTex.h"

namespace ShadingCookbook
{
using glm::vec3;
using glm::mat4;

Decay::Decay() : _width(1024), _height(768)
{
}

void Decay::ProcessInput(int key, int action)
{
}

void Decay::InitScene()
{
    CompileAndLinkShader();

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    glEnable(GL_DEPTH_TEST);
    _shader.Use();
    _shader.SetUniform("NoiseTex", 0);

    GLuint noiseTex = NoiseTex::Generate2DTex(10.0f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, noiseTex);

    _teapot = new Teapot(14, mat4(1.0f));

    _shader.SetUniform("Light.Intensity", vec3(1.0f, 1.0f, 1.0f));
    _lightPos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    _shader.SetUniform("LowThreshold", 0.45f);
    _shader.SetUniform("HighThreshold", 0.65f);
}

void Decay::Update(float t)
{
}

void Decay::Render()
{
    _shader.SetUniform("Light.Position", _lightPos);
    vec3 camPos = 0.85f * vec3(0.0f, 5.0f, 6.0f);
    _view = glm::lookAt(camPos, vec3(0.0f, 0.0f, -3.0f), vec3(0.0f, 1.0f, 0.0f));

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    DrawScene();
    glFinish();
}

void Decay::Shutdown()
{
    delete _teapot;
}

void Decay::Resize(int w, int h)
{
    glViewport(0, 0, w, h);
    _width = w;
    _height = h;
    _projection = glm::perspective(glm::radians(55.0f), (float)w / h, 0.3f, 100.0f);
}

void Decay::DrawScene()
{
    _model = mat4(1.0f);
    _model = glm::rotate(_model, glm::radians(-15.0f), vec3(0.0f, 1.0f, 0.0f));
    _model = glm::rotate(_model, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
    SetMatrices();

    _shader.SetUniform("Material.Kd", 0.7f, 0.5f, 0.3f);
    _shader.SetUniform("Material.Ks", 1.0f, 1.0f, 1.0f);
    _shader.SetUniform("Material.Shininess", 100.0f);
    _teapot->Render();
}

void Decay::SetMatrices()
{
    mat4 mv = _view * _model;
    _shader.SetUniform("ModelViewMatrix", mv);
    _shader.SetUniform("NormalMatrix",
                       mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    _shader.SetUniform("MVP", _projection * mv);
}

void Decay::CompileAndLinkShader()
{
    try
    {
        _shader.CompileShader("Shaders/Decay/Decay.vert");
        _shader.CompileShader("Shaders/Decay/Decay.frag");
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
