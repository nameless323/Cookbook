//
// Using compute shader to simulate cloth.
//

#pragma once

#include <glm.hpp>

#include "../../Core/ShaderProgram.h"
#include "../Scene.h"

namespace ShadingCookbook
{
using glm::mat4;

class Cloth : public Scene
{
public:
    Cloth();
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
    ShaderProgram _computeShader;
    ShaderProgram _computeShaderNorm;
    GLuint _clothVao;
    GLuint _numElements;

    int _width, _height;
    float _time, _dt, _speed, _angle;
    GLuint _vao;
    glm::ivec2 _numParticles;
    glm::ivec2 _clothSize;
    GLuint _readBuf;
    GLuint _posBufs[2], _velBufs[2];
    GLuint _normBuf, _elBuf, _tcBuf;

    mat4 _view, _model, _projection;

    /**
     * \brief Set matrices to shader.
     */
    void SetMatrices();
    /**
     * \brief Compile and link shader from file.
     */
    void CompileAndLinkShader();
    /**
     * \brief Initialize simulation buffers.
     */
    void InitBuffers();
};
}
