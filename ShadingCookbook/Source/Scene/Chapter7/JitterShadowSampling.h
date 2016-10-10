//
// Using jitter texture to preform sampling from shadow map.
//

#pragma once

#include <glm.hpp>

#include "../../Core/ShaderProgram.h"
#include "../../Core/Frustum.h"
#include "../Scene.h"
#include "../../Items/Plane.h"
#include "../../Items/Torus.h"
#include "../../Items/Mesh.h"
#include "../../Items/Teapot.h"

namespace ShadingCookbook
{
using glm::mat4;

class JitterShadowSampling : public Scene
{
public:
    JitterShadowSampling();
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
    GLuint _shadowFBO, _pass1Index, _pass2Index;

    Teapot* _teapot;
    Plane* _plane;
    Torus* _torus;
    Mesh* _mesh;

    int _width, _height;
    int _samplesU, _samplesV;
    int _jitterMapSize;
    float _radius;
    int _shadowmapWidth, _shadowmapHeight;
    float _tPrev;

    mat4 _model, _view, _projection;
    mat4 _lightView, _lightProjection, _lightPV;
    mat4 _shadowScale;
    glm::vec3 _lightPos;
    float _angle;

    Frustum* _lightFrustum;

    /**
     * \brief Build frame buffer for shadow map.
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
     * \brief Get offset value.
     */
    float Jitter();
    /**
     * \brief Build jitter texture.
     */
    void BuildJitterTex();
    /**
     * \brief Draw scene objects.
     */
    void DrawBuildingScene();
};
}
