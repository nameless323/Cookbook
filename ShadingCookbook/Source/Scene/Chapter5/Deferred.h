#pragma once

#include "../../Core/ShaderProgram.h"
#include "../Scene.h"

#include <glm.hpp>
#include "../../Items/Cube.h"
#include "../../Items/Plane.h"
#include "../../Items/Torus.h"
#include "../../Items/Mesh.h"
#include "../../Items/Teapot.h"
using glm::mat4;

class Deferred : public Scene
{
public:
    Deferred();
    void ProcessInput(int key, int action) override;
    void InitScene() override;
    void Update(float t);
    void Render() override;
    void Shutdown();
    void Resize(int x, int y);
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
    
    void SetupFBO();
    void CreateGBufTex(GLenum texUnit, GLenum format, GLuint& texID);
    void SetMatrices();
    void CompileAndLinkShader();
    void Pass1();
    void Pass2();
};