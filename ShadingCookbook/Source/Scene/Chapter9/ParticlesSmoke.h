//
// Using particles to simulate smoke.
//

#pragma once

#include <glm.hpp>
#include <GL/glew.h>

#include "../../Core/ShaderProgram.h"
#include "../Scene.h"
#include "../../Items/Plane.h"

namespace ShadingCookbook
{
using glm::mat4;

class ParticlesSmoke : public Scene
{
public:
    ParticlesSmoke();
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
    GLuint _posBuf[2], _velBuf[2];
    GLuint _particleArray[2];
    GLuint _feedback[2], _initVel, _startTime[2];
    GLuint _drawBuf, _query;
    GLuint _renderSub, _updateSub;
    int _nParticles;
    Plane* _plane;

    mat4 _model, _view, _projection;
    float _angle;
    float _time, _dt;

    /**
     * \brief Feedback buffers initialization.
     */
    void InitBuffers();
    /**
     * \brief Get random float in [0,1]
     */
    float RandFloat();
    /**
     * \brief Set matrices to shader.
     */
    void CompileAndLinkShaders();
    /**
     * \brief Compile and link shader from file.
     */
    void SetMatrices();
};
}
