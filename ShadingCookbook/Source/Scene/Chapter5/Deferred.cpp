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

    glGenVertexArrays(1, &_quad);
    glBindVertexArray(_quad);

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
    
}

void Deferred::Update(float t)
{
    /*if (_time == 0.0f)
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
    if (_angle > 360.0f) _angle -= 360.0f;*/
}


void Deferred::Shutdown()
{
}

void Deferred::SetMatrices()
{
}

void Deferred::Resize(int w, int h)
{
    glViewport(0, 0, w, h);
    _width = w;
    _height = h;
    //    _projection = glm::perspective(glm::radians(60.0f), (float)w / h, 0.3f, 100.0f);
}

void Deferred::CompileAndLinkShader()
{
    try
    {
        _shader.CompileShader("Shaders/ADS/ADS.vert");
        _shader.CompileShader("Shaders/ADS/ADS.frag");
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
