//
// Texture projectors scene (shadowmap like technique).
//

#pragma once

#include <glm.hpp>

#include "../../Core/ShaderProgram.h"
#include "../Scene.h"
#include "../../Items/Teapot.h"
#include "../../Items/Plane.h"
#include "../../Items/Torus.h"
#include "../../Items/Skybox.h"

namespace ShadingCookbook
{
using glm::mat4;

class ProjectTex : public Scene
{
public:
    ProjectTex();
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
    Skybox* _skybox;
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
