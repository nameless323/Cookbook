#pragma once

#include <glm.hpp>

#include "../../Core/ShaderProgram.h"
#include "../Scene.h"
#include "../../Items/Torus.h"
#include "../../Items/Teapot.h"


namespace ShadingCookbook
{
using glm::mat4;

class PhongScene : public Scene
{
public:
    PhongScene();
    void ProcessInput(int key, int action) override;
    void InitScene() override;
    void Update(float t);
    void Render() override;
    void Shutdown();
    void Resize(int x, int y);

private:
    ShaderProgram _shader;
    int _width, _height;
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
