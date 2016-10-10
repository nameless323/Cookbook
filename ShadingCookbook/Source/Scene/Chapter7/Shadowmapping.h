//
// Shadowmapping scene.
//

#pragma once

#include <glm.hpp>

#include "../../Core/ShaderProgram.h"
#include "../../Core/Frustum.h"
#include "../Scene.h"

#include "../../Items/Plane.h"
#include "../../Items/Torus.h"
#include "../../Items/Teapot.h"

namespace ShadingCookbook
{
using glm::mat4;

class Shadowmapping : public Scene
{
public:
    Shadowmapping();
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
    ShaderProgram _shader, _solidShader;
    GLuint _shadowFBO, _pass1Index, _pass2Index;

    Teapot* _teapot;
    Plane* _plane;
    Torus* _torus;

    int _width, _height;
    int _shadowmapWidth, _shadowmapHeight;
    float _tPrev;

    mat4 _model, _view, _projection;
    mat4 _lightPV;
    mat4 _shadowBias;
    float _angle;

    Frustum* _lightFrustum;

    /**
     * \brief Setup framebuffer for shadow map.
     */
    void SetupFBO();
    /**
    * \brief Set matrices to shader.
    */
    void SetMatrices();
    /**
    * \brief Compile and link shader from file.
    */
    void CompileAndLinkShader();
    /**
     * \brief Draw scene objects.
     */
    void DrawScene();
};
}
