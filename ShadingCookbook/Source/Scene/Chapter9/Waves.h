#pragma once

#include <glm.hpp>
#include <GL/glew.h>

#include "../../Core/ShaderProgram.h"
#include "../Scene.h"
#include "../../Items/Plane.h"

namespace ShadingCookbook
{
class Waves : public Scene
{
public:
    Waves();
    void ProcessInput(int key, int action) override;
    void InitScene() override;
    void Update(float t) override;
    void Render() override;
    void Shutdown() override;
    void Resize(int x, int y) override;

private:
    void SetMatrices();
    void CompileAndLinkShader();

    ShaderProgram _shader;
    int _width, _height;
    Plane* _plane;

    glm::mat4 _model;
    glm::mat4 _view;
    glm::mat4 _projection;
    float _angle, _time;
};
}
