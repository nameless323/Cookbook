#pragma once

#include <glm.hpp>

#include "../../Core/ShaderProgram.h"
#include "../Scene.h"

namespace ShadingCookbook
{
using glm::mat4;

class Cloth : public Scene
{
public:
    Cloth();
    void ProcessInput(int key, int action) override;
    void InitScene() override;
    void Update(float t);
    void Render() override;
    void Shutdown();
    void Resize(int x, int y);

private:
    ShaderProgram _shader;
    ShaderProgram _computeShader;
    ShaderProgram _computeShaderNorm;
    GLuint _clothVao;
    GLuint _numElements;

    int _width, _height;
    float _time, _dt, _speed, _angle;
    GLuint _vao;
    glm::ivec2 _numParticles;
    glm::ivec2 _clothSize;
    GLuint _readBuf;
    GLuint _posBufs[2], _velBufs[2];
    GLuint _normBuf, _elBuf, _tcBuf;

    mat4 _view, _model, _projection;

    void SetMatrices();
    void CompileAndLinkShader();
    void InitBuffers();
};
}
