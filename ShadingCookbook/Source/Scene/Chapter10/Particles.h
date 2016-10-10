//
// Using compute shader to simulate cool particles behavior.
//

#pragma once

#include <glm.hpp>

#include "../../Core/ShaderProgram.h"
#include "../Scene.h"

namespace ShadingCookbook
{
using glm::mat4;

class Particles : public Scene
{
public:
    Particles();
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
    int _width, _height;
    float _time, _dt, _speed, _angle;
    GLuint _vao;
    glm::ivec3 _numParticles;
    GLuint _totalParticles;
    GLuint _bhVao, _bhBuf;
    vec4 _bh1, _bh2;

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
     * \brief Initialize velocity and position buffers.
     */
    void InitBuffers();
};
}
