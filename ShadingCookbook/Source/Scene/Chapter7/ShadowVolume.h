//
// Using geometry shader to create stencil based shadow volumes.
//

#pragma once

#include <glm.hpp>
#include <GL/glew.h>

#include "../../Core/ShaderProgram.h"
#include "../Scene.h"
#include "../../Items/Plane.h"
#include "../../Items/MeshAdj.h"

namespace ShadingCookbook
{
using glm::mat4;
using glm::vec4;
using glm::vec3;

class ShadowVolume : public Scene
{
public:
    ShadowVolume();
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
    ShaderProgram _volumeShader, _renderShader, _compShader;
    GLuint _colorDepthFBO, _fsQuad;
    GLuint _spotTex, _brickTex;

    Plane* _plane;
    MeshAdj* _spot;

    int _width, _height;

    vec4 _lightPos;
    mat4 _model, _view, _projection;
    float _angle, _tPrev, _rotSpeed;

    /**
     * \brief Set matrices to shader.
     */
    void SetMatrices(ShaderProgram& shader);
    /**
     * \brief Compile and link shader from file.
     */
    void CompileAndLinkShader();
    /**
     * \brief Setpu framebuffers.
     */
    void SetupFBO();
    /**
     * \brief Draw scene.
     * \param shader Shader to draw.
     * \param onlyShadowCasters Draw all scene or only shadow casters.
     */
    void DrawScene(ShaderProgram& shader, bool onlyShadowCasters);
    /**
     * \brief Draw scene without shadows to framebuffer.
     */
    void Pass1();
    /**
     * \brief Draw only stencil for shadows.
     */
    void Pass2();
    /**
     * \brief Draw scene with shadows.
     */
    void Pass3();
    /**
     * \brief Update light position.
     */
    void UpdateLight();
};
}
