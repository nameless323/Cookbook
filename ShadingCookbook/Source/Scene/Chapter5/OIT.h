#pragma once

#include "../../Core/ShaderProgram.h"
#include "../Scene.h"

#include <glm.hpp>
#include "../../Items/Cube.h"
#include "../../Items/Plane.h"
#include "../../Items/Torus.h"
#include "../../Items/Mesh.h"
#include "../../Items/Teapot.h"
#include "../../Items/Sphere.h"
using namespace glm;

enum BufferNames
{
    COUNTER_BUFFER = 0,
    LINKED_LIST_BUFFER
};

struct ListNode
{
    vec4 Color;
    GLfloat Depth;
    GLuint Next;
};

class OIT : public Scene
{
public:
    OIT();
    void ProcessInput(int key, int action) override;
    void InitScene() override;
    void Update(float t);
    void Render() override;
    void Shutdown();
    void Resize(int x, int y);
private:
    ShaderProgram _shader;
    int _width, _height;
    GLuint _buffers[2], _headPtrTex;
    GLuint _quadVAO, _pass1Index, _pass2Index;
    GLuint _clearBuf;

    Sphere* _sphere;
    Cube* _cube;

    mat4 _model;
    mat4 _view;
    mat4 _projection;
    float _angle, _tPrev, _rotSpeed;

    void SetMatrices();
    void CompileAndLinkShader();
    void Pass1();
    void Pass2();
    void DrawScene();
    void InitShaderStorage();
    void ClearBuffers();
};