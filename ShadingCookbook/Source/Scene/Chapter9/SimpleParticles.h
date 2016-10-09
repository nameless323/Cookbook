#pragma once

#include <glm.hpp>
#include <GL/glew.h>

#include "../../Core/ShaderProgram.h"
#include "../Scene.h"
#include "../../Items/Teapot.h"
#include "../../Items/Plane.h"

namespace ShadingCookbook
{
class SimpleParticles : public Scene
{
public:
    SimpleParticles();
    void ProcessInput(int key, int action) override;
    void InitScene() override;
    void Update(float t) override;
    void Render() override;
    void Shutdown() override;
    void Resize(int x, int y) override;

private:
    ShaderProgram _shader;
    GLuint _initVel, _startTime, _particles;
    GLuint _nParticles;
    Plane* _plane;
    glm::mat4 _model;
    glm::mat4 _view;
    glm::mat4 _projection;
    glm::vec4 _lightPos;
    GLuint _vao;
    float _angle;
    float _time;
    int _width;
    int _height;

    void SetMatrices();
    void CompileAndLinkShader();

    void InitBuffers();
    float RandFloat();
};
}