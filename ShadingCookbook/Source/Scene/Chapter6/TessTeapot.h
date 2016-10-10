//
// Teapot tesselation.
//

#pragma once

#include <glm.hpp>
#include <GL/glew.h>

#include "../../Core/ShaderProgram.h"
#include "../Scene.h"
#include "../../../ingredients/vboteapotpatch.h"

namespace ShadingCookbook
{
class TessTeapot : public Scene
{
public:
    TessTeapot();
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
    VBOTeapotPatch* _teapot;
    float _angle;
    float _tPrev;
    float _rotSpeed;
    ShaderProgram _shader;
    glm::mat4 _model;
    glm::mat4 _view;
    glm::mat4 _viewport;
    glm::mat4 _projection;
    GLuint _vao;
    int _inner;
    int _outer;

    /**
     * \brief Set matrices to shader.
     */
    void SetMatrices();
    /**
     * \brief Compile and link shader from file.
     */
    void CompileAndLinkShader();
};
}
