//
// Vertex based waves simulaion.
//

#pragma once

#include <glm.hpp>
#include <GL/glew.h>

#include "../../Core/ShaderProgram.h"
#include "../Scene.h"
#include "../../Items/Plane.h"

namespace ShadingCookbook
{
class Waves : public Scene
{
public:
    Waves();
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
    Plane* _plane;

    glm::mat4 _model;
    glm::mat4 _view;
    glm::mat4 _projection;
    float _angle, _time;

    /**
     * \brief Compile and link shader from file.
     */
    void SetMatrices();
    /**
     * \brief Set matrices to shader.
     */
    void CompileAndLinkShader();
};
}
