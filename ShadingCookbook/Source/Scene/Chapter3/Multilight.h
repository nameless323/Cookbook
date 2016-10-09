#pragma once

#include <glm.hpp>

#include "../../Core/ShaderProgram.h"
#include "../Scene.h"
#include "../../Items/Plane.h"
#include "../../Items/Mesh.h"
#include "../../Items/Teapot.h"

namespace ShadingCookbook
{
using glm::mat4;

class Multilight : public Scene
{
public:
    Multilight();
    void ProcessInput(int key, int action) override;
    void InitScene() override;
    void Update(float t) override;
    void Render() override;
    void Shutdown() override;
    void Resize(int x, int y) override;

private:
    ShaderProgram _shader;
    int _width, _height;
    Mesh* _piggy;
    Plane* _plane;
    Teapot* _teapot;
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
