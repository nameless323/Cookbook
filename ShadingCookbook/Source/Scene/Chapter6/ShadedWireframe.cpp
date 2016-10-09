#include "ShadedWireframe.h"

#include <ostream>
#include <iostream>
#include <ctime>

#include <gtc/matrix_transform.hpp>
#include <gtx/transform.hpp>

using glm::vec3;

namespace ShadingCookbook
{
ShadedWireframe::ShadedWireframe()
{
}

void ShadedWireframe::ProcessInput(int key, int action)
{
}

void ShadedWireframe::InitScene()
{
    CompileAndLinkShader();

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    glEnable(GL_DEPTH_TEST);
    float c = 1.5f;
    //	_projection = glm::ortho(-0.4f * c, 0.4f * c, -0.3f *c, 0.3f*c, 0.1f, 100.0f);

    _ogre = new Mesh("./media/bs_ears.obj.txt");
    _shader.SetUniform("SpriteTex", 0);
    _shader.SetUniform("Size2", 0.15f);

    _shader.SetUniform("Line.Width", 0.75f);
    _shader.SetUniform("Line.Color", vec4(0.05f, 0.0f, 0.05f, 1.0f));
    _shader.SetUniform("Material.Kd", 0.7f, 0.7f, 0.7f);
    _shader.SetUniform("Light.Position", vec4(0.0f, 0.0f, 0.0f, 1.0f));
    _shader.SetUniform("Material.Ka", 0.2f, 0.2f, 0.2f);
    _shader.SetUniform("Light.Intensity", 1.0f, 1.0f, 1.0f);
    _shader.SetUniform("Material.Ks", 0.8f, 0.8f, 0.8f);
    _shader.SetUniform("Material.Shininess", 100.0f);
}

void ShadedWireframe::Update(float t)
{
}

void ShadedWireframe::Render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    vec3 cameraPos(0.0f, 0.0f, 3.0f);
    _view = glm::lookAt(cameraPos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    _model = glm::mat4(1.0f);
    SetMatrices();

    _ogre->Render();

    glFinish();
}

void ShadedWireframe::Shutdown()
{
    delete _ogre;
}

void ShadedWireframe::Resize(int w, int h)
{
    glViewport(0, 0, w, h);
    float w2 = w / 2.0f;
    float h2 = w / 2.0f;
    _projection = glm::perspective(glm::radians(60.0f), (float)w / h, 0.3f, 100.0f);
    _viewport = glm::mat4
    (
        vec4(w2, 0.0f, 0.0f, 0.0f),
        vec4(0.0f, h2, 0.0f, 0.0f),
        vec4(0.0f, 0.0f, 1.0f, 0.0f),
        vec4(w2, h2, 0.0f, 1.0f)
    );
}

void ShadedWireframe::SetMatrices()
{
    glm::mat4 mv = _view * _model;
    _shader.SetUniform("ModelViewMatrix", mv);
    _shader.SetUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    _shader.SetUniform("MVP", _projection * mv);
    _shader.SetUniform("ViewportMatrix", _viewport);
}

void ShadedWireframe::CompileAndLinkShader()
{
    try
    {
        _shader.CompileShader("Shaders/ShadedWireframe/ShadedWireframe.vert");
        _shader.CompileShader("Shaders/ShadedWireframe/ShadedWireframe.geom");
        _shader.CompileShader("Shaders/ShadedWireframe/ShadedWireframe.frag");
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
