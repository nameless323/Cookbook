//
// Vertex blinn phong model.
//

#pragma once

#include <glm.hpp>

#include "../../Core/ShaderProgram.h"
#include "../Scene.h"
#include "../../Items/Mesh.h"
#include "../../../ingredients/vbomesh.h"

namespace ShadingCookbook
{
class Flat : public Scene
{
public:
    Flat();
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
    void Resize(int w, int h) override;

private:
    ShaderProgram _shader;
    float _angle;
    Mesh* _ogre;
    int _width;
    int _height;
    mat4 _model;
    mat4 _view;
    mat4 _projection;

    /**
     * \brief Set matrices to shader.
     */
    void CompileAndLinkShader();
    /**
     * \brief Compile and link shader from file.
     */
    void SetMatrices();
};
}
