#pragma once

#include <glm.hpp>

#include "../../Core/ShaderProgram.h"
#include "../Scene.h"

namespace ShadingCookbook
{
using glm::mat4;

class Particles : public Scene
{
public:
    Particles();
    void ProcessInput(int key, int action) override;
    void InitScene() override;
    void Update(float t);
    void Render() override;
    void Shutdown();
    void Resize(int x, int y);

private:
    ShaderProgram _shader;
    ShaderProgram _computeShader;
    int _width, _height;
    float _time, _dt, _speed, _angle;
    GLuint _vao;
    glm::ivec3 _numParticles;
    GLuint _totalParticles;
    GLuint _bhVao, _bhBuf;
    vec4 _bh1, _bh2;

    mat4 _view, _model, _projection;

    void SetMatrices();
    void CompileAndLinkShader();
    void InitBuffers();
};
}
