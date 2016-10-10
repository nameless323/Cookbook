//
// Deferred shading implementation.
//

#pragma once

#include <glm.hpp>

#include "../../Core/ShaderProgram.h"
#include "../Scene.h"
#include "../../Items/Cube.h"
#include "../../Items/Plane.h"
#include "../../Items/Torus.h"
#include "../../Items/Mesh.h"
#include "../../Items/Teapot.h"

namespace ShadingCookbook
{
using glm::mat4;

class Deferred : public Scene
{
public:
    Deferred();
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
    GLuint _deferredFBO;
    GLuint _quadVAO, _pass1Index, _pass2Index;

    Plane* _plane;
    Torus* _torus;
    Mesh* _ogre;
    Cube* _cube;
    Teapot* _teapot;

    mat4 _model;
    mat4 _view;
    mat4 _projection;
    float _angle, _tPrev, _rotSpeed;

    /**
     * \brief Setup frame buffer and render targets.
     */
    void SetupFBO();
    /**
     * \brief Create geometry buffer texture.
     */
    void CreateGBufTex(GLenum texUnit, GLenum format, GLuint& texID);
    /**
    * \brief Set matrices to shader.
    */
    void SetMatrices();
    /**
    * \brief Compile and link shader from file.
    */
    void CompileAndLinkShader();
    /**
     * \brief Draw scene to g-buffers.
     */
    void Pass1();
    /**
     * \brief Apply light.
     */
    void Pass2();
};
}
