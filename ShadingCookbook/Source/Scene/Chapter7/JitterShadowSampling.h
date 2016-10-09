#pragma once

#include <glm.hpp>

#include "../../Core/ShaderProgram.h"
#include "../../Core/Frustum.h"
#include "../Scene.h"
#include "../../Items/Cube.h"
#include "../../Items/Plane.h"
#include "../../Items/Torus.h"
#include "../../Items/Mesh.h"
#include "../../Items/Teapot.h"

namespace ShadingCookbook
{
using glm::mat4;

class JitterShadowSampling : public Scene
{
public:
    JitterShadowSampling();
    void ProcessInput(int key, int action) override;
    void InitScene() override;
    void Update(float t);
    void Render() override;
    void Shutdown();
    void Resize(int x, int y);

private:
    ShaderProgram _shader;
    GLuint _shadowFBO, _pass1Index, _pass2Index;

    Teapot* _teapot;
    Plane* _plane;
    Torus* _torus;
    Mesh* _mesh;

    int _width, _height;
    int _samplesU, _samplesV;
    int _jitterMapSize;
    float _radius;
    int _shadowmapWidth, _shadowmapHeight;
    float _tPrev;

    mat4 _model, _view, _projection;
    mat4 _lightView, _lightProjection, _lightPV;
    mat4 _shadowScale;
    glm::vec3 _lightPos;
    float _angle;

    Frustum* _lightFrustum;

    void SetupFBO();
    void SetMatrices();
    void CompileAndLinkShader();
    void DrawScene();
    float Jitter();
    void BuildJitterTex();
    void DrawBuildingScene();
};
}
