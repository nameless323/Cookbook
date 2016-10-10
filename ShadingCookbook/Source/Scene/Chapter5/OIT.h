//
// Make order independent transparency (works not so good).
//

#pragma once

#include <glm.hpp>

#include "../../Core/ShaderProgram.h"
#include "../Scene.h"
#include "../../Items/Cube.h"
#include "../../Items/Sphere.h"

namespace ShadingCookbook
{
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
    /**
     * \brief Handle system input.
     * \param key Key code.
     * \param action Key action (pressed etc).
     */
    void ProcessInput(int key, int action) override;
    /**
     * \brief Make scene initialization (object creation, create initial matrix etc).
     */
    void InitScene() override;
    /**
     * \brief Update scene logic.
     */
    void Update(float t) override;
    /**
     * \brief Render scene.
     */
    void Render() override;
    /**
     * \brief Shutdown scene. Preforms cleanup of the scene.
     */
    void Shutdown() override;
    /**
     * \brief Call when window is resized to reconstruct matrices etc.
     */
    void Resize(int x, int y) override;

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

    /**
     * \brief Set matrices to shader.
     */
    void SetMatrices();
    /**
     * \brief Compile and link shader from file.
     */
    void CompileAndLinkShader();
    /**
     * \brief Make OIT buffer from objects.
     */
    void Pass1();
    /**
     * \brief Calculate color from buffers.
     */
    void Pass2();
    /**
     * \brief Draw scene.
     */
    void DrawScene();
    /**
     * \brief Init OIT buffers.
     */
    void InitShaderStorage();
    /**
     * \brief Clear OIT buffers.
     */
    void ClearBuffers();
};
}
