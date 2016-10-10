//
// Applying tonemapping to HDR image.
//

#pragma once

#include <glm.hpp>

#include "../../Core/ShaderProgram.h"
#include "../Scene.h"
#include "../../Items/Teapot.h"
#include "../../Items/Plane.h"
#include "../../Items/Torus.h"
#include "../../Items/Cube.h"
#include "../../Items/Sphere.h"

namespace ShadingCookbook
{
using glm::mat4;

class Tonemapping : public Scene
{
public:
    Tonemapping();
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
    Teapot* _teapot;
    Plane* _plane;
    Cube* _cube;
    Torus* _torus;
    Sphere* _sphere;

    GLuint _hdrFBO;
    GLuint _fsQuad;
    GLuint _hdrTex;
    GLuint _avgTex;
    GLuint _pass1Index;
    GLuint _pass2Index;

    mat4 _model;
    mat4 _view;
    mat4 _projection;

    float _angle;
    float _prevTime;
    float _rotationSpeed;
    bool _autorotate;
    bool _rotateLeft;
    bool _rotateRight;
    bool _doToneMap;

    /**
     * \brief Set matrices to shader.
     */
    void SetMatrices();
    /**
     * \brief Compile and link shader from file.
     */
    void CompileAndLinkShader();
    /**
     * \brief Setup HDR buffer.
     */
    void SetupFBO();
    /**
     * \brief Draw scene to buffer.
     */
    void Pass1();
    /**
     * \brief Apply tonemapping.
     */
    void Pass2();
    /**
     * \brief Get image average luminance.
     */
    void ComputeAveLuminance();
    /**
     * \brief Draw scene.
     */
    void DrawScene();
};
}
