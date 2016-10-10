//
// Scene with bloom effect.
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

class Bloom : public Scene
{
public:
    Bloom();
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
    void Update(float t);
    /**
     * \brief Render scene.
     */
    void Render() override;
    /**
     * \brief Shutdown scene. Preforms cleanup of the scene.
     */
    void Shutdown();
    /**
     * \brief Call when window is resized to reconstruct matrices etc.
     */
    void Resize(int x, int y);

private:
    ShaderProgram _shader;
    int _width, _height;
    Teapot* _teapot;
    Plane* _plane;
    Cube* _cube;
    Torus* _torus;
    Sphere* _sphere;

    GLuint _hdrFBO;
    GLuint _blurFBO;
    GLuint _fsQuad;
    GLuint _hdrTex;
    GLuint _tex1;
    GLuint _tex2;
    GLuint _linearSampler;
    GLuint _nearestSampler;
    GLuint _pass1Index;
    GLuint _pass2Index;
    GLuint _pass3Index;
    GLuint _pass4Index;
    GLuint _pass5Index;

    GLuint _bloomBufWidth;
    GLuint _bloomBufHeight;


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
     * \brief Generate HDR and blur frame buffers.
     */
    void SetupFBO();
    /**
     * \brief Render scene to HDR framebuffer.
     */
    void Pass1();
    /**
     * \brief Generate bloom texture.
     */
    void Pass2();
    /**
     * \brief Vertical blur of bloom texture.
     */
    void Pass3();
    /**
     * \brief Horizontal blur of bloom texture.
     */
    void Pass4();
    /**
     * \brief Merge pass.
     */
    void Pass5();
    /**
     * \brief Get gaus for blur.
     */
    float Gauss(float x, float sigma2);
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
