#pragma once

#include <glm.hpp>

#include "../../Core/ShaderProgram.h"
#include "../Scene.h"
#include "../../ingredients/bmpreader.h"
#include "../../ingredients/vbomeshadj.h"
#include "../../Items/Mesh.h"
#include "../../Items/MeshAdj.h"

namespace ShadingCookbook
{
class BezierCurve : public Scene
{
public:
    BezierCurve();
    void ProcessInput(int key, int action) override;
    void InitScene() override;
    void Update(float t);
    void Render() override;
    void Shutdown();
    void Resize(int x, int y);

private:
    ShaderProgram _shader;
    ShaderProgram _solidShader;
    glm::mat4 _model;
    glm::mat4 _view;
    glm::mat4 _viewport;
    glm::mat4 _projection;
    GLuint _vao;
    float _angle, _tPrev, _rotSpeed;
    int _numSegments;
    bool _uniformDirty;

    void SetMatrices();
    void CompileAndLinkShader();
};
}
