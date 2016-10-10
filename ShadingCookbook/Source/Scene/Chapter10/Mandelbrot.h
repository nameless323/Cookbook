//
// Draw mandelbrot set using compute shaders.
//

#pragma once

#include <glm.hpp>

#include "../../Core/ShaderProgram.h"
#include "../Scene.h"

#include "../../Items/Cube.h"

namespace ShadingCookbook
{
using glm::mat4;

class Mandelbrot : public Scene
{
public:
    Mandelbrot();
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
    /**
     * \brief Set compute shader window for mandelbrot.
     */
    void SetWindow();

private:
    ShaderProgram _shader;
    ShaderProgram _computeShader;
    int _width, _height;
    float _time, _dt, _speed, _angle, _rotSpeed;
    float _cHeight;
    GLuint _dataBuf, _fsQuad;
    Cube* _cube;
    vec2 _center;
    bool _paused;

    /**
     * \brief Set matrices to shader.
     */
    void SetMatrices();
    /**
     * \brief Compile and link shader from file.
     */
    void CompileAndLinkShader();
    /**
     * \brief Init image to draw set into.
     */
    void InitBuffers();
};
}
