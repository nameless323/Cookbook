//
// Straightforward particles simulaion.
#pragma once

#include <glm.hpp>
#include <GL/glew.h>

#include "../../Core/ShaderProgram.h"
#include "../Scene.h"
#include "../../Items/Plane.h"

namespace ShadingCookbook
{
class SimpleParticles : public Scene
{
public:
    SimpleParticles();
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
    GLuint _initVel, _startTime, _particles;
    GLuint _nParticles;
    Plane* _plane;
    glm::mat4 _model;
    glm::mat4 _view;
    glm::mat4 _projection;
    glm::vec4 _lightPos;
    GLuint _vao;
    float _angle;
    float _time;
    int _width;
    int _height;

    /**
     * \brief Compile and link shader from file.
     */
    void SetMatrices();
    /**
     * \brief Set matrices to shader.
     */
    void CompileAndLinkShader();
    /**
     * \brief Init position and velocity buffers.
     */
    void InitBuffers();
    /**
     * \brief Get random float in [0,1]
     */
    float RandFloat();
};
}
