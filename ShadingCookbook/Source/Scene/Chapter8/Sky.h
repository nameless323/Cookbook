#pragma once

#include <glm.hpp>
#include <GL/glew.h>

#include "../../Core/ShaderProgram.h"
#include "../Scene.h"

namespace ShadingCookbook
{
class Sky : public Scene
{
public:
    Sky();
    void ProcessInput(int key, int action) override;
    void InitScene() override;
    void Update(float t) override;
    void Render() override;
    void Shutdown() override;
    void Resize(int x, int y) override;
    void DrawScene();

private:
    ShaderProgram _shader;
    glm::mat4 _model;
    glm::mat4 _view;
    glm::mat4 _projection;
    glm::vec3 _lightPos;
    GLuint _vao;
    float _angle;
    int _width;
    int _height;

    void SetMatrices();
    void CompileAndLinkShader();
};
}
