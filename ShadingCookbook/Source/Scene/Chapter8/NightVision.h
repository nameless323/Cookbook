//
// Create night vision like effect.
//

#pragma once

#include <glm.hpp>
#include <GL/glew.h>

#include "../../Core/ShaderProgram.h"
#include "../Scene.h"
#include "../../Items/Teapot.h"
#include "../../Items/Plane.h"
#include "../../Items/Torus.h"
#include "../../Items/Cube.h"

namespace ShadingCookbook
{
using glm::mat4;

class NightVision : public Scene
{
public:
    NightVision();
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
    GLuint _renderFBO;
    GLuint _fsQuad;
    GLuint _renderTex;
    GLuint _noiseTex;
    Teapot* _teapot;
    Plane* _plane;
    Cube* _cube;
    Torus* _torus;
    mat4 _model;
    mat4 _view;
    mat4 _projection;
    float _angle;
    float _prevTime;
    float _rotationSpeed;
    bool _autorotate;
    bool _rotateLeft;
    bool _rotateRight;
    GLuint _pass1Index;
    GLuint _pass2Index;
    /**
     * \brief Set matrices to shader.
     */
    void SetMatrices();
    /**
     * \brief Compile and link shader from file.
     */
    void CompileAndLinkShader();    
    /**
     * \brief Setup framebuffers.
     */
    void SetupFBO();
    /**
     * \brief Render scene to texture.
     */
    void Pass1();
    /**
     * \brief Apply and show night vision.
     */
    void Pass2();
};
}
