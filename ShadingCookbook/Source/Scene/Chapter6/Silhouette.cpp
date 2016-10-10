#include "Silhouette.h"

#include <ostream>
#include <iostream>
#include <ctime>

#include <gtc/matrix_transform.hpp>
#include <gtx/transform.hpp>

#include "../../Items/MeshAdj.h"
#include "../../../ingredients/vbomeshadj.h"

namespace ShadingCookbook
{
using glm::vec3;

Silhouette::Silhouette() : _angle(0.0f), _tPrev(0.0f), _rotSpeed(glm::pi<float>() / 8.0f)
{
}

void Silhouette::ProcessInput(int key, int action)
{
}

void Silhouette::InitScene()
{
    CompileAndLinkShader();

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    glEnable(GL_DEPTH_TEST);
    float c = 1.5f;
    //	_projection = glm::ortho(-0.4f * c, 0.4f * c, -0.3f *c, 0.3f*c, 0.1f, 100.0f);

    _ogre = new MeshAdj("./media/spot/spot_triangulated.obj.txt");
    _angle = glm::half_pi<float>();

    _shader.SetUniform("EdgeWidth", 0.015f);
    _shader.SetUniform("PctExtend", 0.25f);
    _shader.SetUniform("LineColor", vec4(0.05f, 0.0f, 0.05f, 1.0f));
    _shader.SetUniform("Material.Kd", 0.7f, 0.5f, 0.2f);
    _shader.SetUniform("Light.Position", vec4(0.0f, 0.0f, 0.0f, 1.0f));
    _shader.SetUniform("Material.Ka", 0.2f, 0.2f, 0.2f);
    _shader.SetUniform("Light.Intensity", 1.0f, 1.0f, 1.0f);
}

void Silhouette::Update(float t)
{
    float dt = t - _tPrev;
    if (_tPrev == 0.0f)
        dt = 0.0f;
    _tPrev = t;

    _angle += _rotSpeed * dt;
    if (_angle > glm::two_pi<float>()) _angle -= glm::two_pi<float>();
}

void Silhouette::Render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    vec3 cameraPos(1.5f * cos(_angle), 0.0f, 1.5f * sin(_angle));
    _view = glm::lookAt(cameraPos, vec3(0.0f, -0.2f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    _model = glm::mat4(1.0f);
    SetMatrices();

    _ogre->Render();

    glFinish();
}

void Silhouette::Shutdown()
{
    delete _ogre;
}

void Silhouette::Resize(int w, int h)
{
    glViewport(0, 0, w, h);
    float c = 4.5f;
    _projection = glm::ortho(-0.4f * c, 0.4f * c, -0.3f * c, 0.3f * c, 0.1f, 100.0f);
}

void Silhouette::SetMatrices()
{
    glm::mat4 mv = _view * _model;
    _shader.SetUniform("ModelViewMatrix", mv);
    _shader.SetUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    _shader.SetUniform("MVP", _projection * mv);
}

void Silhouette::CompileAndLinkShader()
{
    try
    {
        _shader.CompileShader("Shaders/Silhouette/Silhouette.vert");
        _shader.CompileShader("Shaders/Silhouette/Silhouette.geom");
        _shader.CompileShader("Shaders/Silhouette/Silhouette.frag");
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
