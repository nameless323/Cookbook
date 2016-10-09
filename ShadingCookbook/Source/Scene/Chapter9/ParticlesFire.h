#pragma once

#include <glm.hpp>
#include <GL/glew.h>

#include "../../Core/ShaderProgram.h"
#include "../Scene.h"
#include "../../Items/Teapot.h"
#include "../../Items/Plane.h"

namespace ShadingCookbook
{
using glm::mat4;

class ParticlesFire : public Scene
{
public:
    ParticlesFire();
    void ProcessInput(int key, int action) override;
    void InitScene() override;
    void Update(float t) override;
    void Render() override;
    void Shutdown() override;
    void Resize(int x, int y) override;

private:
    ShaderProgram _shader;
    int _width, _height;
    GLuint _posBuf[2], _velBuf[2];
    GLuint _particleArray[2];
    GLuint _feedback[2], _initVel, _startTime[2];
    GLuint _drawBuf, _query;
    GLuint _renderSub, _updateSub;

    int _nParticles;

    Plane* _plane;

    mat4 _model, _view, _projection;
    float _angle;
    float _time, _dt;

    void InitBuffers();
    float RandFloat();
    void CompileAndLinkShaders();
    void SetMatrices();
};
}
