#include "TexDiscard.h"

#include <gtx/transform.hpp>
#include <iostream>
#include <GLFW/glfw3.h>
#include "../../Core/TGA.h"

namespace ShadingCookbook
{
using glm::vec3;

TexDiscard::TexDiscard() : _angle(0), _prevTime(0), _autorotate(1), _rotateLeft(0), _rotateRight(0)
{
}

void TexDiscard::ProcessInput(int key, int action)
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

void TexDiscard::InitScene()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    CompileAndLinkShader();
    _plane = new Plane(50.0f, 50.0f, 1, 1);
    _cube = new Cube();

    _view = glm::lookAt(vec3(1.0f, 1.25f, 1.25f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    _projection = mat4(1.0f);

    _angle = 0.0f;

    _shader.SetUniform("Light.Intensity", vec3(0.9f, 0.9f, 0.9f));

    GLint w, h;
    glActiveTexture(GL_TEXTURE0);
    GLubyte* data = TGA::Read("./media/texture/brick1.tga", w, h);

    GLuint texId[2];
    glGenTextures(2, texId);

    glBindTexture(GL_TEXTURE_2D, texId[0]);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, w, h);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    delete[] data;

    glActiveTexture(GL_TEXTURE1);
    data = TGA::Read("./media/texture/moss.tga", w, h);

    glBindTexture(GL_TEXTURE_2D, texId[1]);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, w, h);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    delete[] data;
}

void TexDiscard::Render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _shader.SetUniform("Light.Position", vec4(0.0f, 0.0f, 0.0f, 1.0f));
    _shader.SetUniform("Material.Kd", 0.9f, 0.9f, 0.3f);
    _shader.SetUniform("Material.Ks", 0.95f, 0.95f, 0.95f);
    _shader.SetUniform("Material.Ka", 0.1f, 0.1f, 0.1f);
    _shader.SetUniform("Material.Shininess", 100.0f);

    _model = mat4(1.0f);

    SetMatrices();
    _cube->Render();
}

void TexDiscard::Update(float t)
{
    float dt = t - _prevTime;
    if (_prevTime == 0.0f) dt = 0.0f;
    _prevTime = t;

    _angle += 0.25f * dt;
    if (_angle > glm::two_pi<float>())
        _angle -= glm::two_pi<float>();
}

void TexDiscard::Shutdown()
{
    delete _cube;
    delete _plane;
}

void TexDiscard::SetMatrices()
{
    mat4 mv = _view * _model * glm::rotate(_angle, vec3(0, 1, 0));
    _shader.SetUniform("ModelViewMatrix", mv);
    _shader.SetUniform("NormalMatrix", mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    _shader.SetUniform("MVP", _projection * mv);
}

void TexDiscard::Resize(int w, int h)
{
    glViewport(0, 0, w, h);
    _width = w;
    _height = h;
    _projection = glm::perspective(glm::radians(60.0f), (float)w / h, 0.3f, 100.0f);
}

void TexDiscard::CompileAndLinkShader()
{
    try
    {
        _shader.CompileShader("Shaders/TexDiscard/TexDiscard.vert");
        _shader.CompileShader("Shaders/TexDiscard/TexDiscard.frag");
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
