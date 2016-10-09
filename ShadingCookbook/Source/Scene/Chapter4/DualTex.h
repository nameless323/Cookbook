#pragma once

#include <glm.hpp>

#include "../../Core/ShaderProgram.h"
#include "../Scene.h"
#include "../../Items/Teapot.h"
#include "../../Items/Plane.h"
#include "../../Items/Torus.h"
#include "../../Items/Cube.h"

namespace ShadingCookbook
{
using glm::mat4;

class DualTex : public Scene
{
public:
    DualTex();
    void ProcessInput(int key, int action) override;
    void InitScene() override;
    void Update(float t);
    void Render() override;
    void Shutdown();
    void Resize(int x, int y);

private:
    ShaderProgram _shader;
    int _width, _height;
    Teapot* _teapot;
    Plane* _plane;
    Cube* _cube;
    Torus* _torus;
    mat4 _model;
    mat4 _view;
    mat4 _projection;
    float _angle;
    float _prevTime;
    bool _autorotate;
    bool _rotateLeft;
    bool _rotateRight;

    void SetMatrices();
    void CompileAndLinkShader();
};
}
