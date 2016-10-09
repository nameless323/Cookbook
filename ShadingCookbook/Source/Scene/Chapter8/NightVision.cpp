#include "NightVision.h"

#include <gtx/transform.hpp>
#include <iostream>
#include <GLFW/glfw3.h>
#include <GL/glew.h>

#include "../../Core/TGA.h"
#include "../../Items/NoiseTex.h"

namespace ShadingCookbook
{
using glm::vec3;

NightVision::NightVision() : _angle(0), _prevTime(0), _autorotate(1), _rotateLeft(0), _rotateRight(0), _width(1024), _height(768)
{
}

void NightVision::ProcessInput(int key, int action)
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

void NightVision::InitScene()
{
    CompileAndLinkShader();

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    _plane = new Plane(50.0f, 50.0f, 1, 1);
    _teapot = new Teapot(14, mat4(1.0f));
    float c = 1.5f;
    _torus = new Torus(0.7f * c, 0.3f * c, 50, 50);

    _projection = mat4(1.0f);


    _angle = glm::pi<float>() / 4.0f;

    SetupFBO();

    GLfloat verts[] =
    {
        -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f
    };
    GLfloat tc[] =
    {
        0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
    };
    unsigned int handle[2];
    glGenBuffers(2, handle);

    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(float), verts, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), tc, GL_STATIC_DRAW);

    glGenVertexArrays(1, &_fsQuad);
    glBindVertexArray(_fsQuad);

    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte*)NULL + (0)));
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, ((GLubyte*)NULL + (0)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    GLuint programHandle = _shader.GetHandle();
    _pass1Index = glGetSubroutineIndex(programHandle, GL_FRAGMENT_SHADER, "pass1");
    _pass2Index = glGetSubroutineIndex(programHandle, GL_FRAGMENT_SHADER, "pass2");

    _shader.SetUniform("Width", _width);
    _shader.SetUniform("Height", _height);
    _shader.SetUniform("Radius", 385.0f);
    _shader.SetUniform("Light.Intensity", vec3(1.0f, 1.0f, 1.0f));

    _noiseTex = NoiseTex::GeneratePeriodic2DTex(200.0f, 0.5f, 512, 512);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _noiseTex);
}

void NightVision::Render()
{
    Pass1();
    Pass2();
}

void NightVision::Update(float t)
{
    float dt = t - _prevTime;
    if (_prevTime == 0.0f) dt = 0.0f;
    _prevTime = t;

    _angle += 0.25f * dt;
    if (_angle > glm::two_pi<float>())
        _angle -= glm::two_pi<float>();
}

void NightVision::SetupFBO()
{
    glGenFramebuffers(1, &_renderFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, _renderFBO);

    glGenTextures(1, &_renderTex);
    glBindTexture(GL_TEXTURE_2D, _renderTex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, _width, _height);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _renderTex, 0);

    GLuint depthBuf;
    glGenRenderbuffers(1, &depthBuf);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _width, _height);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf);

    GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, drawBuffers);

    GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (result == GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer complete" << std::endl;
    else
        std::cout << "Framebuffer error: " << result << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void NightVision::Pass1()
{
    glBindFramebuffer(GL_FRAMEBUFFER, _renderFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &_pass1Index);

    _view = glm::lookAt(vec3(7.0f * cos(_angle), 4.0f, 7.0f * sin(_angle)), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    _projection = glm::perspective(glm::radians(60.0f), (float)_width / _height, 0.3f, 100.0f);

    _shader.SetUniform("Light.Position", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    _shader.SetUniform("Material.Kd", 0.9f, 0.9f, 0.9f);
    _shader.SetUniform("Material.Ks", 0.95f, 0.95f, 0.95f);
    _shader.SetUniform("Material.Ka", 0.1f, 0.1f, 0.1f);
    _shader.SetUniform("Material.Shininess", 100.0f);

    _model = mat4(1.0f);
    _model *= glm::translate(vec3(0.0f, 0.0f, 0.0f));
    _model *= glm::rotate(glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
    SetMatrices();
    _teapot->Render();

    _shader.SetUniform("Material.Kd", 0.4f, 0.4f, 0.4f);
    _shader.SetUniform("Material.Ks", 0.0f, 0.0f, 0.0f);
    _shader.SetUniform("Material.Ka", 0.1f, 0.1f, 0.1f);
    _shader.SetUniform("Material.Shininess", 1.0f);
    _model = mat4(1.0f);
    _model *= glm::translate(vec3(0.0f, -0.75f, 0.0f));
    SetMatrices();
    _plane->Render();

    _shader.SetUniform("Light.Position", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    _shader.SetUniform("Material.Kd", 0.9f, 0.5f, 0.2f);
    _shader.SetUniform("Material.Ks", 0.95f, 0.95f, 0.95f);
    _shader.SetUniform("Material.Ka", 0.1f, 0.1f, 0.1f);
    _shader.SetUniform("Material.Shininess", 100.0f);
    _model = mat4(1.0f);
    _model *= glm::translate(vec3(1.0f, 1.0f, 3.0f));
    _model *= glm::rotate(glm::radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
    SetMatrices();
    _torus->Render();
}

void NightVision::Pass2()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _renderTex);
    glDisable(GL_DEPTH_TEST);

    glClear(GL_COLOR_BUFFER_BIT);

    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &_pass2Index);
    _model = mat4(1.0f);
    _view = mat4(1.0f);
    _projection = mat4(1.0f);
    SetMatrices();

    glBindVertexArray(_fsQuad);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void NightVision::Shutdown()
{
    delete _plane;
    delete _teapot;
    delete _torus;
}

void NightVision::SetMatrices()
{
    mat4 mv = _view * _model;
    _shader.SetUniform("ModelViewMatrix", mv);
    _shader.SetUniform("NormalMatrix",
                       glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    _shader.SetUniform("MVP", _projection * mv);
}

void NightVision::Resize(int w, int h)
{
    glViewport(0, 0, w, h);
    _width = w;
    _height = h;
    _projection = glm::perspective(glm::radians(50.0f), (float)w / h, 0.3f, 100.0f);
}

void NightVision::CompileAndLinkShader()
{
    try
    {
        _shader.CompileShader("Shaders/NightVision/NightVision.vert");
        _shader.CompileShader("Shaders/NightVision/NightVision.frag");
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
