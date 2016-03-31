#include "GammaCorrection.h"
#include <gtx/transform.hpp>
#include <iostream>
#include <GLFW/glfw3.h>
#include "../../Core/TGA.h"
using glm::vec3;

GammaCorrection::GammaCorrection() : _angle(0), _prevTime(0), _autorotate(1), _rotateLeft(0), _rotateRight(0), _gamma(2.2f)
{
}

void GammaCorrection::ProcessInput(int key, int action)
{
    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
        _autorotate = !_autorotate;

    if (key == GLFW_KEY_UP && action == GLFW_PRESS)
        _gamma += 0.1f;
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
        _gamma -= 0.1f;

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

void GammaCorrection::InitScene()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    CompileAndLinkShader();
    _ogre = new Mesh("./media/bs_ears.obj", false, true, true);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    float c = 2.5f;
    _projection = glm::ortho(-0.4f * c, 0.4f * c, -0.3f *c, 0.3f*c, 0.1f, 100.0f);

    _plane = new Plane(50.0f, 50.0f, 1, 1);
    c = 1.5f;
    _torus = new Torus(0.7f * c, 0.3f * c, 50, 50);

    _angle = glm::pi<float>() / 2.0f;
    _shader.SetUniform("Light.Intensity", vec3(1.0f, 1.0f, 1.0f));
    _shader.SetUniform("Gamma", _gamma);
}

void GammaCorrection::Render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _angle = 45;
    _view = glm::lookAt(vec3(3.0f * cos(_angle), 0.0f, 3.0f * sin(_angle)), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

    _shader.SetUniform("Light.Position", _view * vec4(10.0f, 0.0f, 0.0f, 1.0f));
    _shader.SetUniform("Material.Kd", 1.0f, 1.0f, 1.0f);
    _shader.SetUniform("Material.Ks", 0.0f, 0.0f, 0.0f);
    _shader.SetUniform("Material.Ka", 0.0f, 0.0f, 0.0f);
    _shader.SetUniform("Material.Shininess", 100.0f);

    _model = mat4(1.0f);

    _shader.SetUniform("Gamma", _gamma);
    SetMatrices();
    _ogre->Render();
}

void GammaCorrection::Update(float t)
{
    float dt = t - _prevTime;
    if (_prevTime == 0.0f) dt = 0.0f;
    _prevTime = t;

    if (_autorotate)
        _angle += 0.25f * dt;
    if (_angle > glm::two_pi<float>())
        _angle -= glm::two_pi<float>();
}


void GammaCorrection::Shutdown()
{
    delete _ogre;
}

void GammaCorrection::SetMatrices()
{
    mat4 mv = _view * _model * glm::rotate(_angle, vec3(0, 1, 0));
    _shader.SetUniform("ModelViewMatrix", mv);
    _shader.SetUniform("NormalMatrix", mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    _shader.SetUniform("MVP", _projection * mv);
}

void GammaCorrection::Resize(int w, int h)
{
    glViewport(0, 0, w, h);
    _width = w;
    _height = h;
    _projection = glm::perspective(glm::radians(60.0f), (float)w / h, 0.3f, 100.0f);
}

void GammaCorrection::CompileAndLinkShader()
{
    try
    {
        _shader.CompileShader("Shaders/GammaCorrection/GammaCorrection.vert");
        _shader.CompileShader("Shaders/GammaCorrection/GammaCorrection.frag");
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
