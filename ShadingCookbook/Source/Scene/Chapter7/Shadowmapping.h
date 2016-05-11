#pragma once

#include "../../Core/ShaderProgram.h"
#include "../../Core/Frustum.h"
#include "../Scene.h"

#include <glm.hpp>
#include "../../Items/Cube.h"
#include "../../Items/Plane.h"
#include "../../Items/Torus.h"
#include "../../Items/Mesh.h"
#include "../../Items/Teapot.h"
using glm::mat4;

class Shadowmapping : public Scene
{
public:
    Shadowmapping();
    void ProcessInput(int key, int action) override;
    void InitScene() override;
    void Update(float t);
    void Render() override;
    void Shutdown();
    void Resize(int x, int y);
private:
    ShaderProgram _shader, _solidShader;
    GLuint _shadowFBO, _pass1Index, _pass2Index;


    Teapot* _teapot;
    Plane* _plane;
    Torus* _torus;

    int _width, _height;
    int _shadowmapWidth, _shadowmapHeight;
    float _tPrev;

    mat4 _model, _view, _projection;
    mat4 _lightPV;
    mat4 _shadowBias;
    float _angle;

    Frustum* _lightFrustum;

    void SetupFBO();
    void SetMatrices();
    void CompileAndLinkShader();
    void DrawScene();
    void SpitOutDepthBuffer();
};