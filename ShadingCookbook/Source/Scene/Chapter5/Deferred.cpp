#include "Deferred.h"
#include <gtx/transform.hpp>
#include <iostream>
#include <GLFW/glfw3.h>
#include "../../Core/TGA.h"
#include <vector>
using glm::vec3;
using std::vector;

Deferred::Deferred() :
    _width(600), _height(600), _angle(0.0f), _tPrev(0.0f), _rotSpeed(glm::pi<float>()/8.0f)
{
}

void Deferred::ProcessInput(int key, int action)
{
    //if (key == GLFW_KEY_P && action == GLFW_PRESS)
    //    _paused = !_paused;
}

void Deferred::InitScene()
{
    CompileAndLinkShader();
    glEnable(GL_DEPTH_TEST);

    float c = 1.5f;
    _plane = new Plane(50.0f, 50.0f, 1, 1);
    _torus = new Torus(0.7f * c, 0.3f * c, 50, 50);
    _teapot = new Teapot(14, mat4(1.0));

    _angle = glm::pi<float>() / 2.0f;

    GLfloat verts[] =
    {
        -1.0f, -1.0f, 0.0f, 
        1.0f,  -1.0f, 0.0f, 
        1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f, 
        1.0f, 1.0f, 0.0f, 
        -1.0f, 1.0f, 0.0f
    };

    GLfloat tc[] =
    {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };
    unsigned int handle[2];
    glGenBuffers(2, handle);

    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(GLfloat), verts, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(GLfloat), tc, GL_STATIC_DRAW);

    glGenVertexArrays(1, &_quadVAO);
    glBindVertexArray(_quadVAO);

    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    SetupFBO();

    GLuint shaderHandle = _shader.GetHandle();
    _pass1Index = glGetSubroutineIndex(shaderHandle, GL_FRAGMENT_SHADER, "pass1");
    _pass2Index = glGetSubroutineIndex(shaderHandle, GL_FRAGMENT_SHADER, "pass2");

    _shader.SetUniform("Light.Intensity", vec3(1.0f, 1.0f, 1.0f));
}

void Deferred::Render()
{
    Pass1();
    Pass2();
}

void Deferred::Pass1()
{
    glBindFramebuffer(GL_FRAMEBUFFER, _deferredFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &_pass1Index);

    _view = glm::lookAt(vec3(5.0f , 4.0f, 10.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    _projection = glm::perspective(glm::radians(60.0f), (float)_width / _height, 0.3f, 100.0f);

    _shader.SetUniform("Light.Position", vec4(0.0f, 0.0f, 0.0f, 1.0f));
    _shader.SetUniform("Material.Kd", 0.9f, 0.9f, 0.9f);

    _model = mat4(1.0f);
    _model *= glm::translate(vec3(0.0f, 0.0f, 0.0f));
    _model *= glm::rotate(glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
    SetMatrices();
    _teapot->Render();

    _shader.SetUniform("Material.Kd", 0.4f, 0.4f, 0.4f);
    _model = mat4(1.0f);
    _model *= glm::translate(vec3(0.0f, -0.75f, 0.0f));
    SetMatrices();
    _plane->Render();

    _shader.SetUniform("Light.Position", vec4(0.0f, 0.0f, 0.0f, 1.0f));
    _shader.SetUniform("Material.Kd", 0.9f, 0.5f, 0.2f);
    _model = mat4(1.0f);
    _model *= glm::translate(vec3(1.0f, 1.0f, 3.0f));
    _model *= glm::rotate(glm::radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
    SetMatrices();
    _torus->Render();
}

void Deferred::Pass2()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &_pass2Index);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    _view = mat4(1.0);
    _model = mat4(1.0);
    _projection = mat4(1.0);
    SetMatrices();

    glBindVertexArray(_quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Deferred::Update(float t)
{
    float dt = t - _tPrev;
    if (_tPrev == 0.0f) 
        dt = 0.0f;
    _tPrev = t;
    _angle += _rotSpeed*dt;
    if (_angle > glm::two_pi<float>()) 
        _angle -= glm::two_pi<float>();
}


void Deferred::Shutdown()
{
}

void Deferred::SetupFBO()
{
    GLuint depthBuf, posTex, normTex, colorTex;

    glGenFramebuffers(1, &_deferredFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, _deferredFBO);

    glGenRenderbuffers(1, &depthBuf);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _width, _height);

    CreateGBufTex(GL_TEXTURE0, GL_RGB32F, posTex);
    CreateGBufTex(GL_TEXTURE1, GL_RGB32F, normTex);
    CreateGBufTex(GL_TEXTURE2, GL_RG8, colorTex);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, posTex, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normTex, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, colorTex, 0);

    GLenum drawBuffers[] = { GL_NONE, GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(4, drawBuffers);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Deferred::CreateGBufTex(GLenum texUnit, GLenum format, GLuint& texID)
{
    glActiveTexture(texUnit);
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexStorage2D(GL_TEXTURE_2D, 1, format, _width, _height);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void Deferred::SetMatrices()
{
    mat4 mv = _view * _model;
    _shader.SetUniform("ModelViewMatrix", mv);
    _shader.SetUniform("NormalMatrix", mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    _shader.SetUniform("MVP", _projection*mv);
}

void Deferred::Resize(int w, int h)
{
    glViewport(0, 0, w, h);
    _width = w;
    _height = h;
    _projection = glm::perspective(glm::radians(60.0f), (float)w / h, 0.3f, 100.0f);
}

void Deferred::CompileAndLinkShader()
{
    try
    {
        _shader.CompileShader("Shaders/Deferred/Deferred.vert");
        _shader.CompileShader("Shaders/Deferred/Deferred.frag");
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

