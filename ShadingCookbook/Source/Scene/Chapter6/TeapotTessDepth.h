#pragma once

#include <glm.hpp>
#include <GL/glew.h>

#include "../../Core/ShaderProgram.h"
#include "../Scene.h"
#include "TessTeapot.h"

namespace ShadingCookbook
{
class TeapotTessDepth : public Scene
{
public:
    TeapotTessDepth();
    void ProcessInput(int key, int action) override;
    void InitScene() override;
    void Update(float t);
    void Render() override;
    void Shutdown();
    void Resize(int x, int y);

private:
    VBOTeapotPatch* _teapot;
    float _angle;
    float _tPrev;
    float _rotSpeed;
    ShaderProgram _shader;
    glm::mat4 _model;
    glm::mat4 _view;
    glm::mat4 _viewport;
    glm::mat4 _projection;
    GLuint _vao;
    int _inner;
    int _outer;

    void SetMatrices();
    void CompileAndLinkShader();
};
}
