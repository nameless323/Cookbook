#include "OIT.h"
#include <gtx/transform.hpp>
#include <iostream>
#include <GLFW/glfw3.h>
#include "../../Core/TGA.h"
#include <vector>
using glm::vec3;
using std::vector;

OIT::OIT() :
    _width(600), _height(600), _angle(0.0f), _tPrev(0.0f), _rotSpeed(glm::pi<float>() / 8.0f)
{
}

void OIT::ProcessInput(int key, int action)
{
    //if (key == GLFW_KEY_P && action == GLFW_PRESS)
    //    _paused = !_paused;
}

void OIT::InitScene()
{
    CompileAndLinkShader();
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    float c = 1.5f;

    _angle = radians(210.0f);
    _cube = new Cube;
    _sphere = new Sphere(1.0f, 40, 40);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    InitShaderStorage();
    
    GLuint shaderHandle = _shader.GetHandle();
    _pass1Index = glGetSubroutineIndex(shaderHandle, GL_FRAGMENT_SHADER, "pass1");
    _pass2Index = glGetSubroutineIndex(shaderHandle, GL_FRAGMENT_SHADER, "pass2");

    GLfloat verts[] =
    {
        -1.0f, -1.0f, 0.0f,
        1.0f,  -1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f
    };
    GLuint bufHandle;
    glGenBuffers(1, &bufHandle);
    glBindBuffer(GL_ARRAY_BUFFER, bufHandle);
    glBufferData(GL_ARRAY_BUFFER, 3 * 4 * sizeof(GLfloat), verts, GL_STATIC_DRAW);

    glGenVertexArrays(1, &_quadVAO);
    glBindVertexArray(_quadVAO);

    glBindBuffer(GL_ARRAY_BUFFER, bufHandle);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void OIT::Render()
{
    ClearBuffers();
    Pass1();
    Pass2();
}

void OIT::Pass1()
{
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &_pass1Index);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _view = glm::lookAt(vec3(11.0f * cos(_angle), 2.0f, 11.0f * sin(_angle)), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

    _projection = glm::perspective(glm::radians(50.0f), (float)_width / _height, 1.0f, 1000.0f);

    glDepthMask(GL_FALSE);

    DrawScene();
    glFinish();
}

void OIT::Pass2()
{
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &_pass2Index);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _view = mat4(1.0f);
    _projection = mat4(1.0f);
    _model = mat4(1.0f);

    SetMatrices();

    glBindVertexArray(_quadVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
}

void OIT::DrawScene()
{
}

void OIT::InitShaderStorage()
{
}

void OIT::ClearBuffers()
{
    GLuint zero = 0;
    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, _buffers[COUNTER_BUFFER]);
    glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &zero);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _clearBuf);
    glBindTexture(GL_TEXTURE_2D, _headPtrTex);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _width, _height, GL_RED_INTEGER, GL_UNSIGNED_INT, nullptr);
}

void OIT::Update(float t)
{
    float dt = t - _tPrev;
    if (_tPrev == 0.0f)
        dt = 0.0f;
    _tPrev = t;
    _angle += _rotSpeed*dt;
    if (_angle > glm::two_pi<float>())
        _angle -= glm::two_pi<float>();
}


void OIT::Shutdown()
{
}

void OIT::SetMatrices()
{
    mat4 mv = _view * _model;
    _shader.SetUniform("ModelViewMatrix", mv);
    _shader.SetUniform("NormalMatrix", mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    _shader.SetUniform("MVP", _projection*mv);
}

void OIT::Resize(int w, int h)
{
    glViewport(0, 0, w, h);
    _width = w;
    _height = h;
    _projection = glm::perspective(glm::radians(60.0f), (float)w / h, 0.3f, 100.0f);
}

void OIT::CompileAndLinkShader()
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

