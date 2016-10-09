#pragma once

#include <glm.hpp>
#include <GL/glew.h>

#include "../../Core/ShaderProgram.h"
#include "../../Core/Frustum.h"
#include "../Scene.h"
#include "../../Items/Cube.h"
#include "../../Items/Plane.h"
#include "../../Items/Torus.h"
#include "../../Items/Mesh.h"
#include "../../Items/Teapot.h"
#include "../../Items/MeshAdj.h"
#include "../../../ingredients/vbomeshadj.h"

namespace ShadingCookbook
{
using glm::mat4;
using glm::vec4;
using glm::vec3;

class ShadowVolume : public Scene
{
public:
    ShadowVolume();
    void ProcessInput(int key, int action) override;
    void InitScene() override;
    void Update(float t);
    void Render() override;
    void Shutdown();
    void Resize(int x, int y);

private:
    ShaderProgram _volumeShader, _renderShader, _compShader;
    GLuint _colorDepthFBO, _fsQuad;
    GLuint _spotTex, _brickTex;

    Plane* _plane;
    MeshAdj* _spot;

    int _width, _height;

    vec4 _lightPos;
    mat4 _model, _view, _projection;
    float _angle, _tPrev, _rotSpeed;

    void SetMatrices(ShaderProgram& shader);
    void CompileAndLinkShader();
    void SetupFBO();
    void DrawScene(ShaderProgram& shader, bool onlyShadowCasters);
    void Pass1();
    void Pass2();
    void Pass3();
    void UpdateLight();
};
}
